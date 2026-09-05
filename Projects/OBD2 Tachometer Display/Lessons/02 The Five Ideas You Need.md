# 02 — The Five Ideas You Need

You don't need an electronics course. You need five ideas. Each one is here because it will personally bite you at a specific point in this project, and I've said where. Read this once now, then come back to it when a lesson says "remember idea 3."

## Idea 1 — Everything shares a ground

Voltage isn't a thing a wire *has*. It's a *difference* between two points, like altitude. "3.3 volts" means "3.3 volts higher than somewhere else," and that somewhere else is called **ground** (GND).

When you connect a display to the ESP32, the display's ground and the ESP32's ground have to be the same wire. Otherwise the display's idea of "zero" and the ESP32's idea of "zero" are unrelated, and the signal wires between them mean nothing. It's like two services comparing timestamps from unsynchronised clocks.

**Where it bites you:** every single wiring step. When a module does nothing at all, a missing ground wire is the first suspect — maybe 80% of the time. In lesson 10 you'll have two separate boards on two separate USB cables, and they *still* need a ground wire between them.

## Idea 2 — 3.3 volts and 5 volts are different worlds

Your ESP32 runs on 3.3 V. Its pins output 3.3 V and expect to see at most 3.3 V coming in. Feed a pin 5 V and you can kill it — quietly, with no smoke, just a chip that half-works from then on.

Your dev board has both a `5V` pin (from the USB cable) and a `3V3` pin (from the board's own little regulator). The display, the CAN transceiver, and the light sensor are all 3.3 V parts. **Power them from `3V3`, never `5V`.**

The SN65HVD230 transceiver was specifically chosen over the more common MCP2551 *because* it's a 3.3 V part. That's the kind of decision that looks arbitrary in a parts list and is actually this idea.

**Where it bites you:** lessons 05 and 10. It's the only way to break something on the bench, so it gets its own double-check in the wiring steps.

## Idea 3 — A bus is a few wires carrying packets

You'll meet three buses. They're all the same concept you know from networking — shared wires, structured messages, some rule for who talks when — with different trade-offs.

**SPI** (the display) — fast and simple. One wire for a clock, one for data, and a "chip select" wire per device so several can share the clock and data wires. The ESP32 is the master and does all the talking; the display just listens. Think of it as a one-directional firehose of pixels.

**I²C** (the light sensor) — slower, two wires, and devices have *addresses* so you can hang several on the same pair. The ESP32 sends "device 0x23, give me your reading" and the sensor replies. Request/response, like HTTP.

**CAN** (the bike) — the interesting one. Any node can talk, there are no addresses (every message is broadcast and filtered by ID), and the signal is *differential*: two wires driven in opposite directions, where the receiver reads the difference between them. That makes it nearly immune to electrical noise, which is why it won in cars. Lesson 09 covers this properly.

**Where it bites you:** mostly in vocabulary. When a wiring table says MOSI, SCLK, CS, DC, SDA, SCL, CANH, CANL — those are just the wires of these three buses, and each lesson names them as you meet them.

## Idea 4 — Fast wires need termination

A signal travelling down a wire at high speed behaves like a wave in a rope: when it hits the end, it bounces back. On a slow bus nobody cares. On a 500 kbit/s CAN bus, the reflection collides with the next bit and corrupts it.

The fix is a resistor at each end of the bus — 120 Ω for CAN — which absorbs the wave instead of reflecting it. A proper CAN bus has **exactly two**: one at each physical end. The bike already has both built in.

Your transceiver modules come with a 120 Ω resistor soldered on, because the manufacturer assumes you're building a small bus from scratch. On your bench in lesson 10, that's exactly what you're doing, so you keep both. On the bike in lesson 16, the bus already has its two, so yours becomes a third — which loads the bus down and can make it unreliable. You'll remove it from that one module.

**Where it bites you:** lesson 10 if you have too few, lesson 16 if you have too many. The failure is silence, not an error.

## Idea 5 — A motorcycle's electrical system is hostile

A USB port gives you a clean, steady 5 V. A motorcycle gives you something between 12 and 14.5 V that sags when you hit the starter and, occasionally, spikes to 60–80 V for a few microseconds when a big load switches off and the alternator's energy has nowhere to go. That spike is called **load dump** and it will destroy a regulator that isn't designed for it.

So between the battery and your 3.3 V chip there's a little chain: a **fuse** (so a chafed wire can't start a fire), a **relay** (so the device turns off with the key instead of draining the battery), a **TVS diode** (a part that does nothing until the voltage exceeds ~18 V, then shorts the spike to ground in nanoseconds), and a **buck converter** (a switching regulator that turns the messy 12–14 V into a clean 5 V).

That's all four scary words from the parts list, in one sentence each. Lesson 17 goes through them properly, with the *why* for each.

**Where it bites you:** only lesson 17. Until then, you're on USB and none of this exists.

---

That's it. That's the theory. Everything else you'll learn by doing, at the moment you need it.

Next: [[03 Set Up the Toolchain]].
