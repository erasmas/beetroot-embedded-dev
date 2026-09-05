# 11 — Talking and Listening

Two programs. The WROOM sends frames that look like a bike's RPM broadcast; the S3 prints whatever it hears. When bytes scroll past on the S3's monitor, you have a working CAN link.

## Start with the example

ESP-IDF ships `examples/peripherals/twai/twai_network`, which has a master and slave. You'll want it for its setup code — the `twai_general_config_t`, `twai_timing_config_t`, `twai_filter_config_t` trio and the install/start calls — but its master/slave handshake is more than you need. Take the structure and write two simpler programs.

## The fake bike (WROOM-32)

Configuration:
- Timing: `TWAI_TIMING_CONFIG_500KBITS()`.
- Filter: accept all. It's a transmitter; doesn't matter.
- Mode: **`TWAI_MODE_NO_ACK`**. Remember from lesson 09 — the S3 is going to be listen-only and won't acknowledge, so the WROOM has to be told not to expect it. Without this the WROOM retries every frame forever, floods the bus with error frames, and eventually goes "bus-off" and stops. It's the single most common reason a bench CAN rig doesn't work.
- Pins: TX 21, RX 22.

Then a loop, every 20 ms: build a frame with a 29-bit ID (set `extd = 1`; use something like `0x18DAF110` — it doesn't matter yet, you'll learn the real ID in lesson 15), and encode a fake RPM into the first two data bytes the way the real OBD PID does:

```
raw = rpm * 4
data[0] = raw >> 8       // A
data[1] = raw & 0xFF     // B
// decode: rpm = ((A << 8) | B) / 4
```

Make the RPM ramp 800 → 8000 → 800 over a few seconds. Transmit with `twai_transmit`. Print something every few hundred frames so you know it's alive.

## The device (ESP32-S3)

Configuration:
- Timing: 500 kbit/s, same as above.
- Filter: `TWAI_FILTER_CONFIG_ACCEPT_ALL()`. You want to see everything.
- Mode: **`TWAI_MODE_LISTEN_ONLY`**. This is the mode it'll run in for the rest of its life.
- Pins: TX 5, RX 4.

Then a loop: `twai_receive(&msg, portMAX_DELAY)`, and print the ID in hex and the data bytes.

## What you'll see

Lines scrolling on the S3's monitor, one every 20 ms, ID matching what the WROOM sends, first two bytes changing smoothly. That's it. That's a CAN bus.

## When you see nothing

Work the list from lesson 09, in order:

1. **Bitrate.** Both sides 500k? Easy to leave one at the example's default.
2. **Termination.** Both modules still have their 120 Ω? (Check with the multimeter: between CANH and CANL with everything powered off, you should read about 60 Ω — two 120s in parallel.)
3. **H and L crossed?** Swap them and try again; it takes ten seconds.
4. **Ground wire between the boards?** The forgotten one.
5. **TX and RX the right way round?** `D` is the transceiver's *input* and connects to the controller's *TX*. `R` is its *output* and connects to *RX*. It reads backwards and everyone gets it wrong once.
6. **Is the WROOM in NO_ACK mode?** Call `twai_get_status_info` on it and print the state. `BUS_OFF` means it gave up waiting for ACKs.

If the S3 receives a frame or two and then stops, that's the NO_ACK problem on the transmitter side, not a receiver problem.

## Done when

Frames scroll continuously on the S3 with the expected ID and ramping bytes. Commit both programs — keep the fake-bike firmware in the same repo under its own directory; it's part of the project.

Next: [[12 Real Data Into the Gauge]].
