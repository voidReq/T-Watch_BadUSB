# DISCLAIMER
The software of this project was entirely AI generated. I only published it in case anyone could find it helpful as reference.

# T-Watch BadUSB

A Rubber Ducky script execution system for LilyGo T-Watch with web-based script management.

## Features

- WiFi Access Point (416846831) with web portal at 192.168.1.1
- Upload Rubber Ducky scripts via web interface
- Select and execute scripts remotely
- Bluetooth HID keyboard emulation
- Real-time clock display on watch face
- BLE connection status indicator

## Hardware

- LilyGo T-Watch 2020 V3
- ESP32-based (not ESP32-S3)
- 1.54" LCD capacitive touch screen
- Bluetooth 4.2 for HID functionality

## Setup

1. Connect T-Watch to target device via Bluetooth:
   - On your computer, search for Bluetooth devices
   - Connect to "T-Watch" device
   - Accept pairing request

2. Connect to WiFi network:
   - SSID: 416846831
   - Navigate to: http://192.168.1.1

3. Upload scripts:
   - Click "Upload Script"
   - Select .txt file containing Rubber Ducky commands
   - Submit

4. Execute scripts:
   - Select script from dropdown
   - Click "Execute"
   - Script will run via Bluetooth HID

## Rubber Ducky Script Format

Supported commands:
- `STRING <text>` - Type text
- `DELAY <ms>` - Wait specified milliseconds
- `ENTER` - Press Enter key
- `GUI` / `WINDOWS` - Press Windows/Command key
- `ALT` - Press Alt key
- `CTRL` / `CONTROL` - Press Control key
- `SHIFT` - Press Shift key
- `TAB` - Press Tab key
- `ESCAPE` / `ESC` - Press Escape key
- `SPACE` - Press Space key
- `UPARROW` / `UP` - Press Up Arrow
- `DOWNARROW` / `DOWN` - Press Down Arrow
- `LEFTARROW` / `LEFT` - Press Left Arrow
- `RIGHTARROW` / `RIGHT` - Press Right Arrow
- `CAPSLOCK` - Press Caps Lock
- `DELETE` - Press Delete key
- `BACKSPACE` - Press Backspace key
- `REM <comment>` - Comment line (ignored)

## Example Script

```
REM Open calculator on Windows
DELAY 1000
GUI r
DELAY 500
STRING calc
ENTER
```

## Compilation

```bash
arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=min_spiffs T-Watch_BadUSB
arduino-cli upload --fqbn esp32:esp32:esp32:PartitionScheme=min_spiffs --port /dev/ttyACM0 T-Watch_BadUSB
```

## Libraries Required

- ESPAsyncWebServer
- AsyncTCP
- DNSServer (built-in)
- WiFi (built-in)
- LittleFS (built-in)
- BleKeyboard (https://github.com/T-vK/ESP32-BLE-Keyboard)
- TFT_eSPI

## Notes

- The T-Watch 2020 V3 uses standard ESP32 (not ESP32-S3), so USB HID is not available
- Bluetooth HID is used instead, which requires pairing with the target device
- Scripts are stored in LittleFS at /scripts/
- The watch displays time when not executing scripts
- BLE connection status is shown on the web portal
