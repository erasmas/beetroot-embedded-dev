# 10 — Build a Fake Bike

You're going to build a tiny CAN network on your desk: two ESP32s, two transceivers, one twisted pair. One board pretends to be the bike's ECU. The other is your device. This is the test rig you'll use for the rest of the project, so it's worth doing neatly.

## Why a fake bike

Developing on the real bike means the bike has to be running, under the seat, with a laptop balanced on the tank. You'd do that once to find the frames (lesson 15), and then never again if you can help it. A bench rig means you iterate on the sofa, and once you have a recording of real traffic (lesson 13), the bench is indistinguishable from the bike as far as your code knows.

You own an older ESP32-WROOM-32 as well as the S3. The WROOM becomes the bike.

## The wiring

Both boards **unplugged**. Two SN65HVD230 modules, which I'll call **A** (bike side) and **B** (device side).

**Transceiver B ↔ ESP32-S3** (your device):
- `3V3` → S3 `3V3` (idea 2 — this module is 3.3 V only)
- `GND` → S3 `GND`
- `R` → S3 **GPIO 4** (transceiver's receive output → controller's RX)
- `D` → S3 **GPIO 5** (controller's TX → transceiver's data input)

**Transceiver A ↔ ESP32-WROOM-32** (the fake bike):
- `3V3` → WROOM `3V3`
- `GND` → WROOM `GND`
- `R` → WROOM **GPIO 22**
- `D` → WROOM **GPIO 21**
(Any two free pins would do on the WROOM; these are just conventional.)

**On both modules:** there's a pin labelled `RS`. It selects between high-speed and a slow, low-noise mode. You want high-speed: connect `RS` to `GND`. Some modules have a jumper for it. `Vref` stays unconnected.

**The bus itself:** `CANH` on A → `CANH` on B. `CANL` on A → `CANL` on B. Use two jumper wires and **twist them together**, a few turns per centimetre. Keep it under 30 cm. Don't cross H and L — it's the easiest mistake and the result is silence.

**Ground between the boards.** This is the one people forget. The two boards are on two different USB cables, possibly two different ports or a hub. Their grounds aren't automatically the same (idea 1). Run a wire from a `GND` pin on the S3 to a `GND` pin on the WROOM. Without it the differential signal has no shared reference and the bus is unreliable or dead.

**Terminators:** both modules keep their 120 Ω resistor. Two nodes, two ends, two resistors. Correct.

Photograph it. This matches [[Wiring Diagrams]] §6 — compare against it.

## A note on jumper wires

A breadboard and jumpers is a terrible way to build a 500 kbit/s bus, and it'll work anyway because the distance is short. If it's flaky, the first fix is shorter wires and a tighter twist, not code.

## Done when

Wired, photographed, both boards powered, nothing warm. Nothing happens yet — there's no code. That's next.

Next: [[11 Talking and Listening]].
