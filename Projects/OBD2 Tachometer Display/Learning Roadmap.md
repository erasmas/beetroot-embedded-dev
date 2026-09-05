# Learning Roadmap

This project is a course you're teaching yourself. Each lesson is its own note in the `Lessons/` folder. Do them in order — every one builds on the one before, and each ends with something you can see working.

The secret that makes this manageable: **it's three small projects, not one big one.** A screen project, a listening project, and a motorcycle project. The first two happen on your desk with a USB cable and can't break anything. The motorcycle comes last.

## Part 0 — Before you touch anything
- [[00 Start Here]] — how to think about this project, and the one habit that saves you weeks
- [[01 Count the Pins]] — the five-minute check that decides whether the plan is right
- [[02 The Five Ideas You Need]] — all the electronics theory you need, and no more
- [[03 Set Up the Toolchain]] — ESP-IDF, and getting "hello world" out of the chip
- [[04 Your First Real Program]] — blink, and why an RTOS is already running

## Part 1 — The screen
- [[05 Wiring the Display]] — your first wiring, and what each of the six wires does
- [[06 Bringing Up the Display]] — the driver, what "nothing works" looks like, and how to fix it
- [[07 Drawing the Gauge]] — LVGL, and a tachometer that lies
- [[08 Buttons]] — interrupts, and the pattern every embedded program uses

## Part 2 — Listening to the bike (without the bike)
- [[09 What CAN Actually Is]] — the bus, explained for someone who knows networking
- [[10 Build a Fake Bike]] — two boards, two transceivers, one twisted pair
- [[11 Talking and Listening]] — transmitter on one board, listener on the other
- [[12 Real Data Into the Gauge]] — the decoder, and the end-to-end product on your desk
- [[13 The Replay Tool]] — so you never need the bike running in your living room

## Part 3 — On the motorcycle
- [[14 The Multimeter Check]] — proving the bus exists before you plug anything in
- [[15 Finding the RPM Frame]] — reverse-engineering the bike's traffic
- [[16 First Contact]] — your device on the real bus, on a power bank
- [[17 Powering From the Bike]] — fuse, relay, TVS, buck: what each one is for
- [[18 Light Sensor and Real Buttons]] — I²C, backlight dimming, and why the wires need a capacitor
- [[19 Enclosure and the Sun Test]] — the moment of truth

## Part 4 — After it works
- [[20 Beyond the Tachometer]] — the capstone features, in the order to build them
- [[21 When Things Don't Work]] — the hardware debugging cheat sheet

Reference notes (pin tables, wiring drawings, part list): [[OBD2 Tachometer Display]] and [[Wiring Diagrams]]. The lessons tell you *when* to open them.
