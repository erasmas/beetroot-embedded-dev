# 14 — The Multimeter Check

First contact with the bike, and you're not going to plug in anything electronic. Just the adapter cable and the multimeter. This lesson is as much about learning to use the meter as about the bike.

## Why

There are three unverified assumptions stacked up: that the Honda adapter cable fits the connector, that it actually wires the CAN pins through (cheap adapters sometimes only connect power and K-Line), and that there's a live CAN bus on the other side. One measurement answers all three.

## How a multimeter's resistance mode works

Set the dial to Ω (often marked with the omega symbol, possibly a range like 200 or 2k, or "auto"). The meter pushes a tiny current through whatever's between its probes and reports the resistance. Touch the probes together: ~0 Ω. Hold them apart: the display shows "OL" or "1" meaning open, infinite.

Two rules: **the circuit must be unpowered** (resistance mode on a live circuit gives nonsense and can damage the meter), and the reading tells you what's *between* the probes — including everything connected in parallel.

## What to do

1. Ignition **off**. Key out.
2. Plug the Honda 6-pin → OBD2 adapter into the bike's connector. Note that the 16-pin end is now a standard OBD2 socket, so all the pin numbers are the standard ones — the adapter has done the Honda-specific mapping for you.
3. Find pins **6** and **14** on the 16-pin socket. The numbering is moulded into the plastic; pin 6 is on the top row, 14 directly below it, both near the middle.
4. Meter in resistance mode. One probe into pin 6, one into pin 14.

## Reading the result

**About 60 Ω.** That's the answer you want. A CAN bus has a 120 Ω terminator at each end; measured from the middle, you see the two in parallel: 60 Ω. This proves the adapter wires those pins *and* there's a real, terminated CAN bus behind them. Everything in the plan is confirmed.

**About 120 Ω.** You're seeing only one terminator. Unusual — maybe the diagnostic connector is on a short stub with one local terminator. Probably still fine; note it and carry on.

**Open / OL / infinite.** The adapter doesn't connect pins 6 and 14. This is an adapter problem, not a bike problem — get a different cable. Nothing else in the plan changes.

**Near 0 Ω.** Short between CAN-H and CAN-L. Almost certainly a faulty adapter. Don't use it.

Write the reading in [[OBD2 Tachometer Display]] §5 next to Step 1.

## While you're there

Meter to **DC volts** (V with a straight line, not the wavy AC one). Ignition still off, probes on pin **16** and pin **4**: you should see battery voltage, ~12.5 V. You'll never use this pin for power — your notes are clear on that — but it confirms the adapter's power pins too, which tells you it's a full-pinout cable and not a CAN-only one.

## Done when

The reading's recorded, and you've now used a multimeter in both modes. You'll use it again in lesson 17 before you connect anything expensive to the bike's power.

Next: [[15 Finding the RPM Frame]].
