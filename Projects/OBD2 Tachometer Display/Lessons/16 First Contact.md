# 16 — First Contact

Your device listens to the real bike for the first time. Still on a USB power bank — the bike's power system is the next lesson — but the data is real.

## Two bits of soldering

This is the lesson where the soldering iron comes out. Both jobs are small.

**Remove the terminator from transceiver B.** Idea 4: the bike's bus already has its two 120 Ω resistors. Your module's onboard one would make a third, which pulls the bus impedance down and makes every node's signal weaker. It's a small surface-mount resistor near the CANH/CANL pins, usually labelled `R3` or `120`. Heat both ends at once with the iron tip and slide it off, or heat one end and lift with tweezers. Keep transceiver A's resistor — it stays on the bench rig.

Confirm with the meter: CANH to CANL on module B, unpowered, should now read open.

**Solder the pigtail.** The solderable OBD2 male connector has a tiny PCB with a pad per pin. You're soldering **exactly two wires**: pin **6** (CAN-H) and pin **14** (CAN-L). Nothing to pin 16 (+12 V) — power comes from the separate circuit in the next lesson. Nothing to pins 4 or 5 (ground) — your device will have its own ground through its power supply, and a second ground path through the diagnostic connector creates a loop that injects noise. Two pads, two wires, and leave the rest empty.

Twist the two wires together, keep them under 25 cm, and solder or crimp the other ends to module B's CANH and CANL. Which is which matters: pin 6 → CANH, pin 14 → CANL.

## Wire it up

Pigtail into the splitter's port A. Module B's `3V3`, `GND`, `R` and `D` to the S3 exactly as on the bench (GPIO 4 and 5). S3 powered from a USB power bank. Display still on the same eight wires from lesson 05.

This matches [[Wiring Diagrams]] §3 — check it against the drawing.

## Update the decoder

In `can_decode.c`, replace the guessed ID and offsets with the real ones from lesson 15. This should be the only code change.

## What to do

Ignition on. The gauge should wake up and show whatever the ECU reports with the engine off — usually zero RPM, but frames are flowing. Start the engine. The gauge reads idle. Blip the throttle. It follows.

Watch for latency. Your notes promised a few milliseconds, and on a listen-only bus with the display updated from the CAN task, it should feel instantaneous — no lag between engine note and needle. If it lags, something's buffering that shouldn't be.

## When it doesn't work

Same list as the bench, plus one: the splitter. Cheap splitters occasionally don't pass through all sixteen pins. Meter pin 6 to pin 6 across it, and 14 to 14.

And remember what listen-only means: your device can't break anything. If nothing arrives, the bike is fine. Go back to the bench rig with the real recording, confirm the decoder, then come back.

## Done when

Gauge tracks the engine. Film it. Tag `v0.2-bike-usb`. This is a demo-able device — everything after this is making it permanent.

Next: [[17 Powering From the Bike]].
