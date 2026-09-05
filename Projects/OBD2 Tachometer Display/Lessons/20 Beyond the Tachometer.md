# 20 — Beyond the Tachometer

Everything from here is software, on the bench, with the replay file. This is the part of a capstone that distinguishes "I wired up a display" from engineering — and it's the part you already know how to do.

## The raw values

From lesson 15 you have a list of supported PIDs and the broadcast frame IDs. Your notes expect coolant temperature, battery voltage, vehicle speed, intake air temperature, throttle position, and engine load to be available. Each is a screen behind a button press. They're all the same pattern as RPM: match an ID, pull bytes, scale, display. Add them to `can_decode.c` one at a time, each with a replay-file test.

Don't expect fuel level. On motorcycles it usually goes from a float sender straight to the dash and never touches the ECU.

## The derived values, in the order to build them

**1. Alarms.** The cheapest and most valuable. Coolant over a threshold; battery under ~12.5 V with the engine running (the regulator/rectifier is the classic Honda twin failure, and a voltage that's dropping instead of rising at 3,000 rpm is the earliest warning you'll get). When one trips, take over the *whole* screen in red with one word. A number you'd have to read and interpret at speed is useless; a red screen is not.

**2. Computed gear.** If you didn't find Honda's gear byte in lesson 15, derive it. Speed divided by RPM is a constant for each gear — six distinct bands. Record a ride with the replay tool, plot speed/RPM, and the six clusters fall out. Hard-code the band edges and you have a gear indicator. It's a nice piece of the write-up: real signal processing from two standard values. If you *did* find the gear byte, use it and keep the computed one as a cross-check.

**3. Shift light.** RPM above a threshold *and* throttle open — so it doesn't fire when you're engine-braking down a hill at 7,000 rpm. A flash of the arc, or a bar across the top.

**4. Fuel economy estimate.** Engine load × RPM is a rough proxy for fuel flow; divide by speed for consumption. It's approximate — calibrate against a few tank fills — but on an 11-litre tank, knowing "you've got about 60 km left" is genuinely useful.

## The navigation arrow

The original goal — a Beeline-style heading arrow as the primary screen — is a separate project. It needs a GPS module, a source of route data, and a way to get a route onto the device without a phone, which is a design question on its own. Scope it *after* the tachometer ships, as the next thing the same hardware platform does. Everything you've built — the display layer, the buttons, the power chain, the enclosure — carries over.

## Done when

There's no "done" here. Pick the next thing, build it on the bench, replay-test it, ride it. That's the loop now.

And when something doesn't work: [[21 When Things Don't Work]].
