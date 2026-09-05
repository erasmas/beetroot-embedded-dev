# 06 — Bringing Up the Display

The first time you try to drive a display, it won't work. That's not pessimism — it's the normal first result, and this lesson is mostly about what to do when it happens.

## What a display driver actually is

Strip away the abstraction and a display driver does two things.

**Initialisation.** When the GC9A01 powers up it's in an undefined state. The driver resets it (that `RST` wire), then sends a sequence of a few dozen commands — "use this pixel format", "this orientation", "these voltage settings", "turn on". These sequences come from the manufacturer's datasheet and are copied around the internet like incantations. Nobody fully understands them; they just work.

**Drawing.** Once initialised, there's one operation that matters: "here is a rectangle of pixels, put it at (x, y)." The driver sends a "set window" command, then streams pixel bytes over SPI. That's it. Everything you'll ever see on screen — text, arcs, numbers — is software upstream turning shapes into rectangles of pixels.

ESP-IDF's **`esp_lcd`** component splits this into two layers: the *bus* (how bytes get to the chip — SPI, in your case) and the *panel* (which chip it is, and its init sequence). The GC9A01 panel driver is built in. This split is what lets you swap the display later without touching your application — you'd replace one panel driver and keep everything above it.

## What to do

Start from the ESP-IDF example at `examples/peripherals/lcd/spi_lcd_touch`. It supports the GC9A01 as a build-time option and already uses LVGL, which is where you're heading next — but for this lesson, the goal is just the example's own demo rendering.

1. Create a project from that example.
2. In the SDK Configuration Editor, under the example's options, choose the **GC9A01** controller and disable touch.
3. Open the main source file and find the `#define`s for pin numbers near the top. Set them to yours: SCLK 12, MOSI 11, DC 10, CS 9, RST 14, backlight 13. MISO is unused — set it to -1.
4. Set the SPI clock to **10 MHz** for now (the define is usually `EXAMPLE_LCD_PIXEL_CLOCK_HZ`). It will go faster later; slow and working beats fast and mysterious.
5. Build, flash, monitor.

## What you'll see — and what it means

**The example's demo renders.** Congratulations, skip to "done when."

**A lit white screen, nothing drawn.** Backlight's on, so power and `BL` are fine. The init sequence didn't take or the SPI bytes aren't arriving. Check `RST` first — a missing reset wire gives exactly this. Then `DC` and `CS` swapped.

**Completely black.** Is the backlight actually on? Put the multimeter between `BL` and `GND` — you want ~3.3 V. If the backlight is on but the panel is black, it's probably `VCC` — confirm 3.3 V on the display's power wire.

**Garbage, stripes, or a picture that's recognisable but wrong.** SPI is working; something about the format is off. Wrong rotation, wrong colour byte order (look for an RGB/BGR option), or the clock's too fast. Since you're at 10 MHz, it's probably a format flag.

**Flickering or intermittent.** A loose jumper. Press on each one.

The common thread: work from the outside in. Power → backlight → reset → bus → format. Each stage has to work before the next one can show you anything.

## Then go faster

Once it renders, raise the SPI clock. 40 MHz is safe for the GC9A01; 80 MHz often works with short wires. If it breaks at a higher speed, back off. Jumper wires on a breadboard are about the worst possible SPI wiring, and speed is where they show it.

## Done when

The demo renders correctly at 40 MHz. Photograph the wiring again (it's the known-good state), commit.

Next: [[07 Drawing the Gauge]].
