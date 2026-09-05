# 03 — Set Up the Toolchain

No electronics in this lesson. Just a dev board, a USB cable, and the familiar misery of installing a toolchain. Do this while parts are in the post.

## Why this one first

Honestly: this is where most beginners stall, and it has nothing to do with hardware. Getting the compiler, the flasher, and the serial monitor all talking to the chip is fiddly the first time. Do it now, on its own, so that when the display arrives you're debugging the display and not your PATH.

## What you're installing

**ESP-IDF** is Espressif's official SDK for the ESP32 family. It's a CMake-based C framework with FreeRTOS baked in. Your notes chose it over Arduino deliberately: the display driver (`esp_lcd`), the CAN driver (`twai`), and the UI library (LVGL) all have first-class ESP-IDF support, and the Arduino layer would just be in the way.

The easiest route is the **ESP-IDF extension for VS Code**. It downloads the SDK, the cross-compiler, and Python tooling for you, and gives you buttons for build / flash / monitor. Use the "Express" install and pick the latest 5.x release.

## Plug in the right USB port

Your ESP32-S3 devkit has **two** USB-C ports. One is labelled `UART` (or `COM`) and goes through a USB-serial chip. The other is labelled `USB` and goes straight into the S3's own USB controller.

**Use the one labelled `USB`.** It does flashing, serial output, *and* a hardware debugger (JTAG) over one cable — you'll be able to set breakpoints in C later, which is a big deal. The `UART` port only does the first two.

On a Mac it should appear as `/dev/cu.usbmodem*` with no driver install.

## Hello world

1. In VS Code: command palette → *ESP-IDF: New Project* → pick the `hello_world` example.
2. Set the target to **esp32s3** (*ESP-IDF: Set Espressif Device Target*).
3. Build (the wrench icon), flash (the lightning), monitor (the plug). Or, the combined *Build, Flash and Monitor*.

You should see boot messages scroll past and then `Hello world!` with some chip info, repeating every few seconds before a restart.

If nothing appears: wrong port selected (bottom status bar), or the cable is charge-only. If flashing fails with a timeout, hold the `BOOT` button, tap `RESET`, release `BOOT`, and flash again — that forces the chip into download mode. You won't need this trick often, but it's the universal "unstick it" move.

## One configuration thing

Run *ESP-IDF: SDK Configuration Editor* (this is `menuconfig` — think of it as the feature-flag system for the whole firmware, including the OS). Find **Component config → ESP PSRAM** and enable it with **Octal** mode. Your board is an N16R8: 16 MB of flash, 8 MB of PSRAM (external RAM the chip can use for big buffers like framebuffers). The "octal" bit matters — the PSRAM uses eight data lines, which is why GPIOs 35, 36 and 37 are off-limits for anything else. Rebuild and check the boot log now mentions the PSRAM size.

## Done when

You've changed the `"Hello world!"` string to something of your own, rebuilt, flashed, and seen it in the monitor. The edit → build → flash → monitor loop is now muscle memory.

Next: [[04 Your First Real Program]].
