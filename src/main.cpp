#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Preferences.h>
#include <time.h>
#include "driver/uart.h"
#include "driver/gpio.h"

/* ==========================================================================
 * ESP32-C6-LCD-1.47 Onboard Pin Definitions
 * ========================================================================== */
#define LCD_PIN_MOSI    6
#define LCD_PIN_SCLK    7
#define LCD_PIN_CS      14
#define LCD_PIN_DC      15
#define LCD_PIN_RST     21
#define LCD_PIN_BL      22
#define SD_PIN_CS       4

#define RGB_LED_PIN     8
#define BOOT_BTN_PIN    9

/* UART1 Link to STM32F407 Console (P11 Header: PA0=TX -> GP19, PA1=RX <- GP18) */
#define STM32_UART_TX   18
#define STM32_UART_RX   19
#define STM32_UART_BAUD 115200

/* 1.47" ST7789 Panel Geometry in Landscape (320 x 172, Y-offset = 34) */
#define LCD_W           320
#define LCD_H           172
#define LCD_X_OFFSET    0
#define LCD_Y_OFFSET    34

/* ==========================================================================
 * Console Themes (matching STM32F407 UI Manager)
 * ========================================================================== */
struct CompanionTheme {
    const char *name;
    uint16_t    bg565;
    uint16_t    bar565;
    uint16_t    accent565;
    uint8_t     led_r;
    uint8_t     led_g;
    uint8_t     led_b;
};

static constexpr uint16_t RGB888_TO_565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((r & 0xF8U) << 8) | ((g & 0xFCU) << 3) | (b >> 3));
}

static const CompanionTheme kThemes[4] = {
    { "Cyber Blue",  RGB888_TO_565(0x12, 0x18, 0x26), RGB888_TO_565(0x0B, 0x0F, 0x18), RGB888_TO_565(0x00, 0xE5, 0xFF),  0, 36, 48 },
    { "Emerald",     RGB888_TO_565(0x0B, 0x1E, 0x16), RGB888_TO_565(0x06, 0x11, 0x0C), RGB888_TO_565(0x00, 0xFF, 0x66),  0, 44, 16 },
    { "Retro Amber", RGB888_TO_565(0x20, 0x16, 0x0B), RGB888_TO_565(0x12, 0x0C, 0x05), RGB888_TO_565(0xFF, 0xAE, 0x00), 48, 28,  0 },
    { "Synthwave",   RGB888_TO_565(0x1D, 0x10, 0x2A), RGB888_TO_565(0x10, 0x08, 0x18), RGB888_TO_565(0xFF, 0x40, 0xFF), 42,  6, 42 }
};

static constexpr uint16_t COL_WHITE  = 0xFFFFU;
static constexpr uint16_t COL_GRAY   = RGB888_TO_565(0x90, 0xA4, 0xAE);
static constexpr uint16_t COL_GREEN  = RGB888_TO_565(0x00, 0xFF, 0x66);
static constexpr uint16_t COL_YELLOW = RGB888_TO_565(0xFF, 0xD6, 0x00);
static constexpr uint16_t COL_CARD   = RGB888_TO_565(0x1A, 0x22, 0x34);

static SPIClass s_spi(FSPI);
static uint8_t  s_theme_idx    = 0;
static uint32_t s_boot_presses = 0;
static uint32_t s_uart_tx_cnt  = 0;
static uint32_t s_uart_rx_cnt  = 0;

static Preferences s_prefs;
static String   s_wifi_ssid        = "";
static String   s_wifi_pass        = "";
static bool     s_wifi_connecting  = false;
static uint32_t s_wifi_conn_start  = 0;
static bool     s_ntp_synced       = false;
static uint32_t s_last_ntp_sync    = 0;
static uint32_t s_last_ntp_poll    = 0;

static bool     s_stm32_online   = false;
static uint32_t s_last_stm32_rx  = 0;
static uint32_t s_stm32_heap     = 0;
static int32_t  s_stm32_tmcu     = 0;
static int32_t  s_stm32_text     = -999;
static char     s_stm32_app[32]  = "connecting...";

static char     s_rx_line[128];
static uint16_t s_rx_line_len    = 0;

/* ==========================================================================
 * Compact 5x7 ASCII Font (0x20..0x7E)
 * ========================================================================== */
static const uint8_t kFont5x7[95][5] = {
    {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00}, {0x00,0x07,0x00,0x07,0x00},
    {0x14,0x7F,0x14,0x7F,0x14}, {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00}, {0x00,0x1C,0x22,0x41,0x00},
    {0x00,0x41,0x22,0x1C,0x00}, {0x14,0x08,0x3E,0x08,0x14}, {0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00}, {0x08,0x08,0x08,0x08,0x08}, {0x00,0x60,0x60,0x00,0x00},
    {0x20,0x10,0x08,0x04,0x02}, {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31}, {0x18,0x14,0x12,0x7F,0x10},
    {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}, {0x00,0x36,0x36,0x00,0x00},
    {0x00,0x56,0x36,0x00,0x00}, {0x08,0x14,0x22,0x41,0x00}, {0x14,0x14,0x14,0x14,0x14},
    {0x00,0x41,0x22,0x14,0x08}, {0x02,0x01,0x51,0x09,0x06}, {0x32,0x49,0x79,0x41,0x3E},
    {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36}, {0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C}, {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01},
    {0x3E,0x41,0x49,0x49,0x7A}, {0x7F,0x08,0x08,0x08,0x7F}, {0x00,0x41,0x7F,0x41,0x00},
    {0x20,0x40,0x41,0x3F,0x01}, {0x7F,0x08,0x14,0x22,0x41}, {0x7F,0x40,0x40,0x40,0x40},
    {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F}, {0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06}, {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31}, {0x01,0x01,0x7F,0x01,0x01}, {0x3F,0x40,0x40,0x40,0x3F},
    {0x1F,0x20,0x40,0x20,0x1F}, {0x3F,0x40,0x38,0x40,0x3F}, {0x63,0x14,0x08,0x14,0x63},
    {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43}, {0x00,0x7F,0x41,0x41,0x00},
    {0x02,0x04,0x08,0x10,0x20}, {0x00,0x41,0x41,0x7F,0x00}, {0x04,0x02,0x01,0x02,0x04},
    {0x40,0x40,0x40,0x40,0x40}, {0x00,0x01,0x02,0x04,0x00}, {0x20,0x54,0x54,0x54,0x78},
    {0x7F,0x48,0x44,0x44,0x38}, {0x38,0x44,0x44,0x44,0x20}, {0x38,0x44,0x44,0x48,0x7F},
    {0x38,0x54,0x54,0x54,0x18}, {0x08,0x7E,0x09,0x01,0x02}, {0x0C,0x52,0x52,0x52,0x3E},
    {0x7F,0x08,0x04,0x04,0x78}, {0x00,0x44,0x7D,0x40,0x00}, {0x20,0x40,0x44,0x3D,0x00},
    {0x7F,0x10,0x28,0x44,0x00}, {0x00,0x41,0x7F,0x40,0x00}, {0x7C,0x04,0x18,0x04,0x78},
    {0x7C,0x08,0x04,0x04,0x78}, {0x38,0x44,0x44,0x44,0x38}, {0x7C,0x14,0x14,0x14,0x08},
    {0x08,0x14,0x14,0x18,0x7C}, {0x7C,0x08,0x04,0x04,0x08}, {0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3F,0x44,0x40,0x20}, {0x3C,0x40,0x40,0x20,0x7C}, {0x1C,0x20,0x40,0x20,0x1C},
    {0x3C,0x40,0x30,0x40,0x3C}, {0x44,0x28,0x10,0x28,0x44}, {0x0C,0x50,0x50,0x50,0x3C},
    {0x44,0x64,0x54,0x4C,0x44}, {0x00,0x08,0x36,0x41,0x00}, {0x00,0x00,0x7F,0x00,0x00},
    {0x00,0x41,0x36,0x08,0x00}, {0x10,0x08,0x08,0x10,0x08}
};

/* ==========================================================================
 * Minimal ST7789 172x320 Driver (Landscape)
 * ========================================================================== */
static void lcd_cmd(uint8_t cmd)
{
    digitalWrite(LCD_PIN_DC, LOW);
    digitalWrite(LCD_PIN_CS, LOW);
    s_spi.transfer(cmd);
    digitalWrite(LCD_PIN_CS, HIGH);
}

static void lcd_data(uint8_t data)
{
    digitalWrite(LCD_PIN_DC, HIGH);
    digitalWrite(LCD_PIN_CS, LOW);
    s_spi.transfer(data);
    digitalWrite(LCD_PIN_CS, HIGH);
}

static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    x0 += LCD_X_OFFSET;
    x1 += LCD_X_OFFSET;
    y0 += LCD_Y_OFFSET;
    y1 += LCD_Y_OFFSET;

    lcd_cmd(0x2A); /* CASET */
    digitalWrite(LCD_PIN_DC, HIGH);
    digitalWrite(LCD_PIN_CS, LOW);
    uint8_t caset[4] = { (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xFF), (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xFF) };
    s_spi.writeBytes(caset, 4);
    digitalWrite(LCD_PIN_CS, HIGH);

    lcd_cmd(0x2B); /* RASET */
    digitalWrite(LCD_PIN_DC, HIGH);
    digitalWrite(LCD_PIN_CS, LOW);
    uint8_t raset[4] = { (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xFF), (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xFF) };
    s_spi.writeBytes(raset, 4);
    digitalWrite(LCD_PIN_CS, HIGH);

    lcd_cmd(0x2C); /* RAMWR */
}

static void lcd_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > LCD_W) w = LCD_W - x;
    if (y + h > LCD_H) h = LCD_H - y;
    if (w <= 0 || h <= 0) return;

    lcd_set_window((uint16_t)x, (uint16_t)y, (uint16_t)(x + w - 1), (uint16_t)(y + h - 1));

    uint8_t line_buf[LCD_W * 2];
    uint8_t hi = (uint8_t)(color >> 8);
    uint8_t lo = (uint8_t)(color & 0xFF);
    for (int16_t i = 0; i < w; i++)
    {
        line_buf[i * 2]     = hi;
        line_buf[i * 2 + 1] = lo;
    }

    digitalWrite(LCD_PIN_DC, HIGH);
    digitalWrite(LCD_PIN_CS, LOW);
    for (int16_t row = 0; row < h; row++)
    {
        s_spi.writeBytes(line_buf, (uint32_t)(w * 2));
    }
    digitalWrite(LCD_PIN_CS, HIGH);
}

static void lcd_draw_char(int16_t x, int16_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale)
{
    if (c < 0x20 || c > 0x7E) c = '?';
    const uint8_t *glyph = kFont5x7[c - 0x20];
    uint16_t w = 6U * scale;
    uint16_t h = 8U * scale;
    if (x < 0 || y < 0 || (x + w) > LCD_W || (y + h) > LCD_H) return;

    uint8_t buf[6 * 3 * 8 * 3 * 2];
    if (scale > 3U) scale = 3U;

    uint8_t fg_hi = (uint8_t)(fg >> 8), fg_lo = (uint8_t)(fg & 0xFF);
    uint8_t bg_hi = (uint8_t)(bg >> 8), bg_lo = (uint8_t)(bg & 0xFF);

    uint32_t idx = 0;
    for (uint8_t row = 0; row < 8U; row++)
    {
        for (uint8_t sr = 0; sr < scale; sr++)
        {
            for (uint8_t col = 0; col < 6U; col++)
            {
                bool on = (col < 5U && row < 7U) && ((glyph[col] >> row) & 0x01U);
                for (uint8_t sc = 0; sc < scale; sc++)
                {
                    buf[idx++] = on ? fg_hi : bg_hi;
                    buf[idx++] = on ? fg_lo : bg_lo;
                }
            }
        }
    }

    lcd_set_window((uint16_t)x, (uint16_t)y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    digitalWrite(LCD_PIN_DC, HIGH);
    digitalWrite(LCD_PIN_CS, LOW);
    s_spi.writeBytes(buf, idx);
    digitalWrite(LCD_PIN_CS, HIGH);
}

static void lcd_draw_text(int16_t x, int16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t scale)
{
    while (*str)
    {
        lcd_draw_char(x, y, *str++, fg, bg, scale);
        x += (int16_t)(6 * scale);
    }
}

static void lcd_init(void)
{
    pinMode(SD_PIN_CS, OUTPUT);
    digitalWrite(SD_PIN_CS, HIGH);

    pinMode(LCD_PIN_CS, OUTPUT);
    pinMode(LCD_PIN_DC, OUTPUT);
    pinMode(LCD_PIN_RST, OUTPUT);
    pinMode(LCD_PIN_BL, OUTPUT);
    digitalWrite(LCD_PIN_CS, HIGH);
    digitalWrite(LCD_PIN_BL, LOW);

    s_spi.begin(LCD_PIN_SCLK, -1, LCD_PIN_MOSI, LCD_PIN_CS);
    s_spi.setFrequency(40000000);

    digitalWrite(LCD_PIN_RST, LOW);
    delay(20);
    digitalWrite(LCD_PIN_RST, HIGH);
    delay(120);

    lcd_cmd(0x11); /* SLPOUT */
    delay(120);

    lcd_cmd(0x36); /* MADCTL: Landscape (MV | MX) */
    lcd_data(0x60);

    lcd_cmd(0x3A); /* COLMOD: 16-bit RGB565 */
    lcd_data(0x05);

    lcd_cmd(0x21); /* INVON (IPS ST7789 requires inversion on) */
    lcd_cmd(0x29); /* DISPON */
    delay(20);

    digitalWrite(LCD_PIN_BL, HIGH);
}

/* ==========================================================================
 * Dashboard Rendering
 * ========================================================================== */
static void update_wifi_card_lines(void)
{
    char line[64];
    if (WiFi.status() == WL_CONNECTED)
    {
        snprintf(line, sizeof(line), "WiFi:  [OK] %-14s (%ddBm)   ", s_wifi_ssid.c_str(), (int)WiFi.RSSI());
        lcd_draw_text(18, 52, line, COL_GREEN, COL_CARD, 1);

        char time_str[20] = "NTP: Syncing";
        time_t now = time(nullptr);
        struct tm ti;
        localtime_r(&now, &ti);
        if (ti.tm_year >= (2024 - 1900))
        {
            snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);
        }
        snprintf(line, sizeof(line), "IP:    %-15s | %-10s", WiFi.localIP().toString().c_str(), time_str);
        lcd_draw_text(18, 68, line, COL_YELLOW, COL_CARD, 1);
    }
    else if (s_wifi_connecting)
    {
        snprintf(line, sizeof(line), "WiFi:  [CONNECTING...] %-12s   ", s_wifi_ssid.c_str());
        lcd_draw_text(18, 52, line, RGB888_TO_565(0x00, 0xE5, 0xFF), COL_CARD, 1);

        snprintf(line, sizeof(line), "MAC:   %-28s", WiFi.macAddress().c_str());
        lcd_draw_text(18, 68, line, COL_GRAY, COL_CARD, 1);
    }
    else
    {
        if (s_wifi_ssid.length() > 0)
        {
            snprintf(line, sizeof(line), "WiFi:  [OFFLINE] %-18s", s_wifi_ssid.c_str());
            lcd_draw_text(18, 52, line, RGB888_TO_565(0xFF, 0x8A, 0x80), COL_CARD, 1);
        }
        else
        {
            lcd_draw_text(18, 52, "WiFi:  [NOT CONFIGURED] Set on Console", COL_GRAY, COL_CARD, 1);
        }

        snprintf(line, sizeof(line), "MAC:   %-28s", WiFi.macAddress().c_str());
        lcd_draw_text(18, 68, line, COL_YELLOW, COL_CARD, 1);
    }
}

static void draw_static_ui(void)
{
    const CompanionTheme &thm = kThemes[s_theme_idx];

    lcd_fill_rect(0, 0, LCD_W, LCD_H, thm.bg565);

    /* Top Header Bar */
    lcd_fill_rect(0, 0, LCD_W, 24, thm.bar565);
    lcd_fill_rect(0, 24, LCD_W, 2, thm.accent565);
    lcd_draw_text(8, 5, "STM32 + ESP32-C6 HUD", thm.accent565, thm.bar565, 2);

    /* Info Cards */
    lcd_fill_rect(8, 30, 304, 58, COL_CARD);
    lcd_fill_rect(8, 30, 3, 58, thm.accent565);

    lcd_fill_rect(8, 92, 304, 74, COL_CARD);
    lcd_fill_rect(8, 92, 3, 74, s_stm32_online ? COL_GREEN : RGB888_TO_565(0xC6, 0x28, 0x28));

    char line[64];
    snprintf(line, sizeof(line), "Theme: %-12s (BOOT btn)", thm.name);
    lcd_draw_text(18, 36, line, COL_WHITE, COL_CARD, 1);

    update_wifi_card_lines();

    /* Onboard addressable LED on ESP32-C6-LCD-1.47 uses GRB byte order */
    neopixelWrite(RGB_LED_PIN, thm.led_g, thm.led_r, thm.led_b);
}

static void update_dynamic_ui(void)
{
    char line[64];
    bool was_online = s_stm32_online;
    s_stm32_online = (millis() - s_last_stm32_rx) < 1800U;

    if (was_online != s_stm32_online)
    {
        draw_static_ui();
    }
    else
    {
        update_wifi_card_lines();
    }

    if (s_stm32_online)
    {
        snprintf(line, sizeof(line), "STM32: [ONLINE] App: %-14s", s_stm32_app);
        lcd_draw_text(18, 98, line, COL_GREEN, COL_CARD, 1);

        char ext_buf[16];
        if (s_stm32_text > -900)
        {
            snprintf(ext_buf, sizeof(ext_buf), "%ld.%ld C", (long)(s_stm32_text / 10), (long)(abs(s_stm32_text) % 10));
        }
        else
        {
            snprintf(ext_buf, sizeof(ext_buf), "N/A");
        }

        snprintf(line, sizeof(line), "MCU: %ld.%ld C | DS18B20: %-10s",
                 (long)(s_stm32_tmcu / 10), (long)(abs(s_stm32_tmcu) % 10), ext_buf);
        lcd_draw_text(18, 114, line, COL_WHITE, COL_CARD, 1);

        snprintf(line, sizeof(line), "Heap: %lu KB Free | TX:%lu RX:%lu   ",
                 (unsigned long)s_stm32_heap, (unsigned long)s_uart_tx_cnt, (unsigned long)s_uart_rx_cnt);
        lcd_draw_text(18, 130, line, COL_GRAY, COL_CARD, 1);

        lcd_draw_text(18, 146, "Link: PA0/PA1 <-> GP18/GP19 [OK]   ", COL_YELLOW, COL_CARD, 1);
    }
    else
    {
        lcd_draw_text(18, 98,  "STM32: [WAITING LINK...]          ", RGB888_TO_565(0xFF, 0x8A, 0x80), COL_CARD, 1);
        lcd_draw_text(18, 114, "Wiring: PA0->GP19 | PA1<-GP18     ", COL_WHITE, COL_CARD, 1);
        snprintf(line, sizeof(line), "Packets: TX=%lu | RX=%lu bytes   ",
                 (unsigned long)s_uart_tx_cnt, (unsigned long)s_uart_rx_cnt);
        lcd_draw_text(18, 130, line, COL_GRAY, COL_CARD, 1);
        lcd_draw_text(18, 146, "Press BOOT btn to test themes     ", COL_YELLOW, COL_CARD, 1);
    }
}

static void handle_stm32_packet(const char *pkt)
{
    if (strncmp(pkt, "STM32:HB", 8) == 0)
    {
        s_last_stm32_rx = millis();

        const char *p = strstr(pkt, "HEAP=");
        if (p) s_stm32_heap = (uint32_t)strtoul(p + 5, NULL, 10);

        p = strstr(pkt, "TMCU=");
        if (p) s_stm32_tmcu = (int32_t)strtol(p + 5, NULL, 10);

        p = strstr(pkt, "TEXT=");
        if (p) s_stm32_text = (int32_t)strtol(p + 5, NULL, 10);

        p = strstr(pkt, "THM=");
        if (p)
        {
            uint8_t thm = (uint8_t)atoi(p + 4);
            if (thm < 4 && thm != s_theme_idx)
            {
                s_theme_idx = thm;
                draw_static_ui();
            }
        }

        p = strstr(pkt, "APP=");
        if (p)
        {
            strncpy(s_stm32_app, p + 4, sizeof(s_stm32_app) - 1);
            s_stm32_app[sizeof(s_stm32_app) - 1] = '\0';
            char *eol = strchr(s_stm32_app, '\r');
            if (eol) *eol = '\0';
            eol = strchr(s_stm32_app, '\n');
            if (eol) *eol = '\0';
        }

        update_dynamic_ui();
    }
    else if (strncmp(pkt, "STM32:SET_WIFI", 14) == 0)
    {
        const char *p_s = strstr(pkt, "SSID=");
        const char *p_p = strstr(pkt, "PASS=");
        if (p_s)
        {
            p_s += 5;
            size_t l = strcspn(p_s, ",\r\n");
            s_wifi_ssid = String(p_s).substring(0, l);
        }
        if (p_p)
        {
            p_p += 5;
            size_t l = strcspn(p_p, ",\r\n");
            s_wifi_pass = String(p_p).substring(0, l);
        }
        else
        {
            s_wifi_pass = "";
        }

        s_prefs.putString("ssid", s_wifi_ssid);
        s_prefs.putString("pass", s_wifi_pass);

        WiFi.disconnect();
        if (s_wifi_ssid.length() > 0)
        {
            WiFi.begin(s_wifi_ssid.c_str(), s_wifi_pass.c_str());
            s_wifi_connecting = true;
            s_wifi_conn_start = millis();
            s_ntp_synced      = false;
        }
        draw_static_ui();
        update_dynamic_ui();
    }
}

static void stm32_uart_init(void)
{
    uart_config_t cfg = {};
    cfg.baud_rate  = STM32_UART_BAUD;
    cfg.data_bits  = UART_DATA_8_BITS;
    cfg.parity     = UART_PARITY_DISABLE;
    cfg.stop_bits  = UART_STOP_BITS_1;
    cfg.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE;
    cfg.source_clk = UART_SCLK_XTAL;

    (void)uart_driver_install(UART_NUM_1, 512, 512, 0, NULL, 0);
    (void)uart_param_config(UART_NUM_1, &cfg);
    (void)uart_set_pin(UART_NUM_1, STM32_UART_TX, STM32_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    (void)gpio_set_drive_capability((gpio_num_t)STM32_UART_TX, GPIO_DRIVE_CAP_3);
    (void)gpio_set_pull_mode((gpio_num_t)STM32_UART_RX, GPIO_PULLUP_ONLY);
}

static void stm32_uart_send(const char *str)
{
    if (str != NULL)
    {
        (void)uart_write_bytes(UART_NUM_1, str, strlen(str));
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(BOOT_BTN_PIN, INPUT_PULLUP);

    s_prefs.begin("wifi_cfg", false);
    s_wifi_ssid = s_prefs.getString("ssid", "");
    s_wifi_pass = s_prefs.getString("pass", "");

    WiFi.mode(WIFI_STA);
    if (s_wifi_ssid.length() > 0)
    {
        WiFi.begin(s_wifi_ssid.c_str(), s_wifi_pass.c_str());
        s_wifi_connecting = true;
        s_wifi_conn_start = millis();
    }

    lcd_init();
    stm32_uart_init();
    draw_static_ui();
    update_dynamic_ui();

    Serial.println("[ESP32-C6] Companion HUD initialized!");
}

void loop()
{
    static bool last_btn = true;
    static uint32_t last_tick = 0;

    bool btn = digitalRead(BOOT_BTN_PIN);
    if (!btn && last_btn)
    {
        delay(25);
        if (!digitalRead(BOOT_BTN_PIN))
        {
            s_boot_presses++;
            s_theme_idx = (s_theme_idx + 1U) % 4U;
            draw_static_ui();
            update_dynamic_ui();

            /* Notify STM32 of theme change from ESP32 */
            char cmd[32];
            snprintf(cmd, sizeof(cmd), "ESP32:SET_THM=%u\n", (unsigned int)s_theme_idx);
            stm32_uart_send(cmd);
            s_uart_tx_cnt++;

            Serial.printf("[ESP32-C6] Switched theme to %s (press #%lu)\r\n",
                          kThemes[s_theme_idx].name, (unsigned long)s_boot_presses);
        }
    }
    last_btn = btn;

    uint8_t rx_buf[64];
    int rx_len = uart_read_bytes(UART_NUM_1, rx_buf, sizeof(rx_buf), 0);
    for (int i = 0; i < rx_len; i++)
    {
        char c = (char)rx_buf[i];
        s_uart_rx_cnt++;

        if (c == '\n' || c == '\r')
        {
            if (s_rx_line_len > 0)
            {
                s_rx_line[s_rx_line_len] = '\0';
                handle_stm32_packet(s_rx_line);
                s_rx_line_len = 0;
            }
        }
        else if (s_rx_line_len < (sizeof(s_rx_line) - 1))
        {
            s_rx_line[s_rx_line_len++] = c;
        }
    }

    /* Check Wi-Fi state transitions */
    if (s_wifi_connecting)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            s_wifi_connecting = false;
            s_ntp_synced      = false;
            configTzTime("EET-2EEST,M3.5.0/3,M10.5.0/4", "pool.ntp.org", "time.google.com", "time.cloudflare.com");
            draw_static_ui();
            update_dynamic_ui();
        }
        else if ((millis() - s_wifi_conn_start) > 20000U)
        {
            s_wifi_connecting = false;
            draw_static_ui();
            update_dynamic_ui();
        }
    }

    /* NTP Synchronization (poll every 2s until synced, then refresh every hour) */
    if (WiFi.status() == WL_CONNECTED && (!s_ntp_synced || (millis() - s_last_ntp_sync > 3600000U)))
    {
        if (millis() - s_last_ntp_poll >= 2000U)
        {
            s_last_ntp_poll = millis();
            struct tm ti;
            if (getLocalTime(&ti, 50))
            {
                if (ti.tm_year >= (2024 - 1900))
                {
                    s_ntp_synced    = true;
                    s_last_ntp_sync = millis();
                    int stm_wd      = (ti.tm_wday == 0) ? 7 : ti.tm_wday; // Monday=1..Sunday=7
                    char time_buf[64];
                    snprintf(time_buf, sizeof(time_buf),
                             "ESP32:TIME,Y=%d,M=%d,D=%d,WD=%d,H=%d,MIN=%d,S=%d\n",
                             ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday,
                             stm_wd, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    stm32_uart_send(time_buf);
                    Serial.printf("[NTP] Synced time: %s", time_buf);
                    draw_static_ui();
                    update_dynamic_ui();
                }
            }
        }
    }

    if (millis() - last_tick >= 500U)
    {
        last_tick = millis();
        int wifi_code = 0;
        if (WiFi.status() == WL_CONNECTED)
        {
            wifi_code = 1;
        }
        else if (s_wifi_connecting)
        {
            wifi_code = 2;
        }
        else if (s_wifi_ssid.length() > 0)
        {
            wifi_code = -1;
        }

        char hb[128];
        snprintf(hb, sizeof(hb), "ESP32C6:HB,WIFI=%d,IP=%s,RSSI=%d,SSID=%.20s\n",
                 wifi_code,
                 (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString().c_str() : "0.0.0.0",
                 (WiFi.status() == WL_CONNECTED) ? (int)WiFi.RSSI() : 0,
                 s_wifi_ssid.c_str());
        stm32_uart_send(hb);
        s_uart_tx_cnt++;
        update_dynamic_ui();
    }
}
