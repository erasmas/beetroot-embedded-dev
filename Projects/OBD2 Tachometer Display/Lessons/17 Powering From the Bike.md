# 17 — Powering From the Bike

This is the lesson all the scary parts were waiting for. Fuse, relay, TVS diode, buck converter. Each one exists for exactly one reason, and once you know the reason, it's just a part in a chain.

## What the bike gives you

Idea 5 from lesson 02. Battery voltage sits around 12.6 V with the engine off, climbs to 14.5 V with the engine running and charging, sags below 10 V for a moment when the starter turns, and — this is the one that kills things — occasionally spikes to 60–80 V for a few microseconds when a heavy load switches off and the alternator's stored energy has nowhere to go. Mechanics call that *load dump*.

Your ESP32 wants a steady 5 V into its `5V`/`VIN` pin (its own onboard regulator makes 3.3 V from that). The chain below turns the bike's mess into that 5 V, and stays safe while doing it.

## The chain, one part at a time

**Battery → fuse.** A fuse is a deliberately weak link. If a wire chafes through its insulation and touches the frame, the fuse blows instead of the wire glowing red. It protects the *wire*, not your device — so it goes as close to the battery as possible, where the wire starts. A 2 A blade fuse in an inline holder. If the fuse were at the device end, the whole run to it would be unprotected.

**Fuse → relay.** A relay is a switch operated by electricity: a small current through its coil pulls a contact closed, letting a larger current through. You want the device to turn on with the ignition key and off without it, so it doesn't drain the battery overnight. But you don't want to draw the device's power from whatever the key switches (those circuits have their own fuses sized for their own loads). So: the relay's *coil* is wired to something that's only live with the key on — the tail light is the usual choice, it's ~150 mA of coil and the tail light won't notice. The relay's *contact* passes your fused battery feed through. Key on → coil energises → contact closes → device powered.

Relay pins follow a German standard everyone uses: **30** = power in, **87** = power out, **85** and **86** = coil. You'll see those numbers printed on the relay body.

**Relay → TVS diode → buck.** The TVS (transient voltage suppressor) is a diode that does absolutely nothing below its rated voltage — 18 V for the SMBJ18A — and above it becomes a near-short to ground, in under a nanosecond. It sits *across* the buck converter's input, positive to negative. Normal 12–14.5 V passes untouched. An 80 V load-dump spike gets clamped to ~25 V before the buck ever sees it. It's a one-part insurance policy against the thing most likely to destroy the device. Polarity matters: the end with the band (cathode) goes to the positive side.

**Buck converter.** A switching regulator: it chops the input on and off thousands of times a second and smooths the result to a steady 5 V. Far more efficient than a linear regulator (which would just burn the excess as heat). "Automotive-rated" means it's designed for the 8–30 V input range and transients — the $1 USB car charger modules aren't, and they brown out during cranking and reset the ESP32 mid-ride. Get one that says 2 A and automotive.

**Ground.** One point. The buck's negative output, the ESP32's GND, the buttons' ground, the light sensor's ground — all meet at a single node, and from there one wire goes to battery negative (or the frame, which is the same thing). Never let anything find its own way to ground through a different path; that's a ground loop, and it shows up as noise, phantom button presses, and CAN errors you'll never trace.

## What to do

Wire it per [[Wiring Diagrams]] §4, which has wire gauges and colours. Crimp connectors and heat-shrink, not twisted-and-taped — this is going to vibrate for years.

**Before you connect the ESP32:** power the chain (key on) and put the multimeter on the buck's output. You want **5.0 V, ±0.1**. If it's 12 V, the buck is wired wrong or set wrong (some are adjustable — there's a tiny potentiometer; turn it while watching the meter). If it's 0, check the relay is clicking when you turn the key — you can hear it.

Only when the meter says 5.0 does the ESP32 get connected. Habit from lesson 14: measure before you trust.

## Done when

Key on → device boots and shows the gauge. Key off → device off. Buck output measured and written down. Engine started with the device on and it didn't reset during cranking — that's the buck earning its keep.

Next: [[18 Light Sensor and Real Buttons]].
