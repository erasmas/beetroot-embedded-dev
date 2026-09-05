# 19 — Enclosure and the Sun Test

The device works. Now it has to survive weather and be readable at noon. The second one is the open question in your whole plan, and this lesson is where it gets answered.

## The enclosure

A round case for the display on the handlebar, and a box under the seat for the ESP32, transceiver, and buck. Between them, a cable with the display's eight wires and the two button wires. 3D-print if you have access; otherwise a small IP65 project box for under the seat and a printed or turned bezel for the display.

Two things matter more than they look like they should:

**Strain relief.** Every wire that leaves a box should be clamped where it exits so that vibration works on the clamp, not on the solder joint. A cable gland, or a zip tie around the wire inside the box. Motorcycles shake solder joints to death in weeks.

**The sun hood.** A lip of 15–20 mm around the top and sides of the display, like the peak of a cap. It costs nothing and it's worth more than doubling the backlight, because the problem in sunlight isn't that the screen is dim — it's that the glass reflects the sky. Shade the glass and the reflection goes away.

Also: a **matte anti-reflective film** on the display glass. A few dollars, cut from a phone screen protector. Same reasoning — kill the reflection.

## Why the sun is the enemy

Outdoor daylight is 10,000 to 100,000 lux. Your display emits about 300 nits. The glossy, unbonded cover glass reflects a few percent of whatever light hits it — and a few percent of 100,000 lux is more than 300 nits. The reflection outshines the image and the contrast drops below 1:1. You see the sky, not the gauge.

That's why the UI rules from lesson 07 exist: pure white on pure black, fat shapes, no greys. In the sun, the whites are the only thing that survive, and only if there's nothing washed-out between them.

## The test

Mount it. Pick a clear day, midday, and ride somewhere with the sun behind you, beside you, and in front of you. The honest question is: at a glance, while moving, can you read the number?

**Yes, comfortably.** Done. The 1.28" panel stays.

**Yes in shade, no in direct sun.** Decision time, and your notes already laid out the options, all the same size so nothing else changes:
- A higher-brightness GC9A01 panel (500–800 nits) — same driver, zero code change. Try this first.
- The Waveshare 1.43" AMOLED — brighter, laminated glass, and a mostly-black UI uses almost no power. Different driver, one file behind LVGL.
- The Sharp memory LCD — reflective, so brighter sun makes it *more* readable. Monochrome, needs a sidelight at night. Guaranteed to work; different character.

Because everything's behind LVGL, any of these is a panel-driver swap. The screens, the decoder, the buttons, the power chain — all untouched.

## Done when

Mounted, strain-relieved, hooded, and you've ridden it in the sun and written the verdict — and the decision it leads to — in [[OBD2 Tachometer Display]] §7.

The tachometer ships here. Next: [[20 Beyond the Tachometer]].
