# 05 — Wiring the Display

Eight jumper wires between the dev board and the round display. This is your first real wiring, and it's where "remember idea 2" matters.

## What the wires are

Your display is a 1.28" round LCD driven by a chip called the **GC9A01**. It talks **SPI** (idea 3) — the ESP32 shoves pixels down a wire and the GC9A01 puts them on the glass. The display module has a ribbon with around a dozen labelled wires. You'll use eight. Here's what each one does, because knowing *what* a wire is for is how you'll debug it later.

**Power — two wires**
- `VCC` → the dev board's **`3V3`** pin. Not `5V`. The GC9A01 is a 3.3 V part. This is the one connection that can break something; check it twice.
- `GND` → any `GND` pin on the dev board. Idea 1: shared ground.

**The SPI bus — two wires**
- `SCLK` (or `SCL`, `CLK`) → **GPIO 12**. The clock. The ESP32 toggles it; on every tick the display reads one bit from the data wire.
- `MOSI` (or `SDA`, `DIN`) → **GPIO 11**. "Master Out, Slave In" — the data. Pixels, one bit per clock tick. There's no MISO wire because you never read anything back from the display.

**Control — three wires, specific to this display**
- `CS` → **GPIO 9**. Chip select. Pulled low to say "this transaction is for you." It exists so several SPI devices can share SCLK and MOSI. You only have one device, but the wire's still needed.
- `DC` → **GPIO 10**. Data/Command. The GC9A01 receives two kinds of bytes — commands ("set the drawing window", "turn the panel on") and pixel data. This wire tells it which kind is arriving. Not part of generic SPI; it's the display's own addition.
- `RST` (or `RES`) → **GPIO 14**. Hardware reset. The driver pulses this low at startup to put the chip in a known state. Without it the panel often won't initialise at all.

**Backlight — one wire**
- `BL` (or `LED`) → **GPIO 13**. It's literally just the power to the backlight LEDs. High = on. Later you'll drive it with PWM to dim it. For now it just needs to be high, or you'll have a working display you can't see.

**Leave unconnected:** anything starting with `TP_` (touch panel — `TP_SDA`, `TP_SCL`, `TP_INT`, `TP_RST`). Your module has a touch layer; you're not using it.

These GPIO numbers are from the pin plan in [[OBD2 Tachometer Display]] §7. They're not magic — SPI on the S3 can be routed to almost any pin — but they avoid the pins that *are* special (strapping pins 0/3/45/46, PSRAM pins 35–37).

## What to do

1. **Unplug the USB cable.** Idea: never wire live.
2. Connect the eight wires. Female-to-female jumpers if the display came with a header; otherwise the ribbon's pre-crimped connector into a breadboard and male jumpers from there.
3. Go round once more with your finger on each wire, reading the label on the display end and the GPIO on the board end out loud. Especially `VCC → 3V3`.
4. Photograph it.
5. Plug in.

## What you'll see

Nothing, yet — there's no driver code. But: **touch the display module and the dev board after thirty seconds.** Warm is fine. Hot means a short, and you unplug immediately and check the wiring. That's the whole safety procedure at 3.3 V.

## Done when

Eight wires, photographed, nothing hot.

Next: [[06 Bringing Up the Display]].
