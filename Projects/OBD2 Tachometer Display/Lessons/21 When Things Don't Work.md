# 21 — When Things Don't Work

Hardware doesn't throw exceptions. It goes quiet, or shows garbage, or works on Tuesday and not Wednesday. This note is the stack trace you don't get. Find the symptom, work the list top to bottom.

## Before anything else

- **What changed?** Wire or code? Git answers half; your photos answer the other half. If you changed both, undo one.
- **Is it powered?** Meter on the module's VCC and GND. 3.3 V or it isn't.
- **Is the ground shared?** Every module's GND to the ESP32's GND. Separate boards need a wire between them. This is the answer more often than anything else on this page.

## "Nothing happens at all"

1. USB cable is data, not charge-only. Try another.
2. Right USB port on the S3 (the one labelled `USB`).
3. Hold `BOOT`, tap `RESET`, release `BOOT`, flash again.
4. Something on a strapping pin (GPIO 0, 3, 45, 46) held low at reset — the chip's in download mode. Unplug buttons and try.

## "The board keeps resetting"

- Monitor output says `Brownout detector was triggered` → power supply can't keep up. USB hub? Cheap buck? Long thin USB cable? Measure the 5 V rail under load.
- Says `Task watchdog got triggered` → a task is busy-looping without `vTaskDelay`. The message names the task.
- Resets when the engine cranks → the buck isn't automotive-rated, or the TVS is missing.

## Display

| You see | Check, in order |
|---|---|
| Black | Backlight pin high? Meter BL to GND: 3.3 V. Then VCC on 3V3. |
| Lit white, nothing drawn | RST wired? DC and CS swapped? |
| Garbage / stripes | Colour byte order (RGB/BGR), SPI clock too fast — drop to 10 MHz. |
| Picture but wrong orientation or mirrored | Rotation / mirror flags in the panel config. |
| Flickers or drops out | Loose jumper. Press each. |
| Works at 10 MHz, not at 80 | Wires too long for the speed. Shorten or slow down. |

## CAN

| You see | Check, in order |
|---|---|
| Receiver silent | Bitrate 500k both sides? Terminators — exactly two (60 Ω across H–L, unpowered)? H and L not crossed? Ground between boards? |
| A few frames, then silence | Transmitter went bus-off waiting for ACKs. Bench transmitter needs `NO_ACK` mode. |
| Transmitter reports bus-off | Same as above, or no receiver at all on the bus. |
| Frames arrive, wrong bytes | `D`/`R` confused with TX/RX? (`D` is input, goes to TX.) Bitrate slightly off — check the timing config. |
| Works on bench, not on bike | Removed the terminator from the bike-side module? Splitter passes pins 6 and 14? Decoder has the real ID from lesson 15? |

## Buttons

- Five presses from one push → bounce; software lockout missing.
- Presses on their own with the engine running → EMI; RC filter missing or at the wrong end of the wire; button ground not at the single ground node.
- Never registers → pull-up not enabled; button wired to 3V3 instead of GND.

## Bike power

- Device dead with key on → relay not clicking? Coil circuit actually switched? Fuse blown?
- Buck output wrong → adjustable trim pot, or IN/OUT swapped.
- Everything works but CAN is noisy → second ground path, probably through OBD pin 4/5. There should be exactly one route to ground.

## When it's none of these

Go back to the last photographed, committed state that worked. Rebuild it exactly. Then change one thing at a time toward where you were. It's slow and it always finds it.
