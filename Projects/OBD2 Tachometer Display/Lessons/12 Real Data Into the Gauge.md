# 12 — Real Data Into the Gauge

Two working halves — a gauge that sweeps fake numbers, and a receiver that prints real frames. This lesson joins them. It's mostly a software lesson, and it's the one where the architecture of the whole firmware gets decided.

## The shape

Remember the pattern from lesson 08: something fast and minimal produces events, a queue carries them, a task consumes them. The CAN path is the same shape:

```
TWAI driver (hardware + ISR, not yours)
   → twai_receive() in a CAN task, blocking
   → decode: is it the RPM frame? pull out the value
   → take LVGL lock, update arc + label, release
```

Your notes say "update the display straight from the CAN RX callback," and this is what that means in practice: the CAN task does the decode and the widget update itself, with no intermediate hop. Nothing else runs between a frame arriving and the arc moving. That's where the ~2 ms latency claim comes from, and it's the thing that makes this device better than anything built on a dongle.

Keep that task lean. Decode, update, loop. Logging goes elsewhere or behind a flag — `printf` in the hot path is the embedded equivalent of synchronous I/O in a request handler.

## What to do

1. In the S3 firmware, delete the fake sweep task from lesson 07.
2. Bring in the TWAI setup from lesson 11 (listen-only, 500k, accept all).
3. Write a CAN task: block on `twai_receive`; if the frame's ID matches the fake bike's ID, decode bytes 0–1 with `((A << 8) | B) / 4`; under the LVGL lock, set the arc value and the label text.
4. Give the CAN task a higher priority than the button task and the LVGL task. Priority in FreeRTOS is "who runs when several are ready," and you want a frame to pre-empt a screen switch, not the other way round.
5. Put the decoder in its own file — `can_decode.c` or similar — with a function like `bool decode_rpm(const twai_message_t *msg, uint16_t *rpm_out)`. In lesson 15 the real frame ID and byte offset will turn out to be something different from your guess, and you want that to be a one-line change in one file.

## What you'll see

The gauge on the S3 follows the ramp the WROOM is sending. Unplug the twisted pair and it freezes at the last value. Plug it back in and it resumes. You've built the product — everything after this is making it survive the motorcycle.

## A small design decision worth making now

What should the gauge show when frames *stop* arriving — key off, or a loose wire? Freezing at the last value is misleading. Add a timeout: if no RPM frame for, say, 500 ms, show zero or a dash. An `xTaskGetTickCount()` compare in the CAN task, or a FreeRTOS software timer that the task keeps resetting. Small, but it's the kind of thing that separates a demo from a device.

## Done when

Gauge tracks the fake bike; disconnecting the bus shows the timeout state; reconnecting recovers. Commit and tag it `v0.1-bench`. This is the first version you could show in the capstone.

Next: [[13 The Replay Tool]].
