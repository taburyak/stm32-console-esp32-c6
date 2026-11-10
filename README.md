# ESP32-C6-LCD-1.47 Companion Co-Processor for STM32F407 Console

Firmware for the **ESP32-C6-LCD-1.47** (`ESP32-C6FH4`, 160 MHz RISC-V, Wi-Fi 6, BLE 5, 1.47" 172×320 `ST7789` LCD, `WS2812` RGB LED) acting as an optional plug-and-play companion module and secondary HUD display for `my-stm32-console`.

## Hardware Pinout

### Onboard Peripherals (ESP32-C6-LCD-1.47)
| Peripheral | Signal | ESP32-C6 GPIO | Notes |
|---|---|---|---|
| **ST7789 LCD (172×320)** | `MOSI` (`DIN`) | `GPIO6` | SPI2 |
| | `SCLK` (`CLK`) | `GPIO7` | SPI2 (40 MHz) |
| | `CS` | `GPIO14` | Active Low |
| | `DC` | `GPIO15` | Data / Command |
| | `RST` | `GPIO21` | Reset |
| | `BL` | `GPIO22` | Backlight (Active High / PWM) |
| **MicroSD Slot** | `SD_CS` | `GPIO4` | Shares SPI2 with LCD |
| **Addressable RGB LED** | `WS2812` | `GPIO8` | 1× RGB LED |
| **BOOT Button** | `KEY_BOOT` | `GPIO9` | Active Low (internal pull-up) |

### Wiring to STM32F407 Console (`P11` WiFi / `P12` Header)
| Signal | STM32F407 Header | STM32 Pin | ESP32-C6 Header Pin |
|---|---|---|---|
| **GND** | `P11 Pin 2` (or `P12 Pin 2`) | `GND` | **`GND`** |
| **Power (5V or 3.3V)** | `P12 Pin 3` (`VCC5`) or `P11 Pin 7` (`VCC3.3`) | — | **`5V`** (or `3V3`) |
| **STM32 TX → ESP RX** | `P11 Pin 8` (`USART4_TX`) | `PA0` (`UART4_TX`) | **`GP19`** (`UART1_RX`) |
| **ESP TX → STM32 RX** | `P11 Pin 1` (`USART4_RX`) | `PA1` (`UART4_RX`) | **`GP18`** (`UART1_TX`) |

## Build & Flash (PlatformIO)

```powershell
pio run
pio run -t upload
pio device monitor
```
