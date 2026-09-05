# 13 — The Replay Tool

Half an hour of work now that pays for itself the first time you don't have to start the motorcycle to test a UI change.

## The idea

In lesson 15 you'll plug a laptop into the bike and record a few minutes of real CAN traffic to a file. If the WROOM can play that file back onto the bench bus — same IDs, same bytes, same timing — then your device can't tell the difference between the bench and the bike. Every frame Honda's ECU sends, including the proprietary ones you haven't decoded yet, is there to experiment with.

It also makes bugs reproducible. "The gauge glitched at 4,000 rpm" becomes a file you can replay until you find it.

## What to build

**On the laptop:** a small Python script that reads a `candump` log — one line per frame, in the format `(timestamp) can0 ID#DATA` — and emits a C array:

```c
typedef struct { uint32_t delta_ms; uint32_t id; uint8_t len; uint8_t data[8]; } frame_t;
const frame_t frames[] = {
    {  0, 0x18DAF110, 8, {0x0B, 0xB8, 0, 0, 0, 0, 0, 0}},
    { 20, 0x18DAF110, 8, {0x0B, 0xC4, 0, 0, 0, 0, 0, 0}},
    ...
};
```

The `delta_ms` is the gap since the previous frame, so playback preserves the real timing.

**On the WROOM:** replace the ramp loop from lesson 11 with one that walks this array, waits `delta_ms`, transmits, and loops back to the start at the end. A few minutes of traffic at a few hundred frames a second is tens of thousands of frames — at 16 bytes each, that's well under a megabyte, and the WROOM has 4 MB of flash. Mark the array `const` so it lives in flash, not RAM.

Until you have a real recording, generate a synthetic one from the script — the same RPM ramp as before — so the pipeline is proven end to end.

## Done when

The WROOM plays a generated file, the S3's gauge follows it, and swapping the file is a rebuild of the WROOM only. Commit the script and the replay firmware.

That's the end of the bench. Everything from here involves the bike.

Next: [[14 The Multimeter Check]].
