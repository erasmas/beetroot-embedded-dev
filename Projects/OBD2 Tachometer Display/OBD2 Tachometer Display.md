# Rebel Tachometer — Project Handoff

**Project:** Beeline Moto clone (motorcycle navigation device) with live OBD/CAN engine telemetry
**Context:** Embedded Development capstone
**Vehicle:** Honda Rebel CMX500, **2022** (Euro 5)
**Status:** Hardware selection complete, first parts ordered. No code written yet. Bus not yet probed.
**Hardware revision (2026-08-22):** simplified to a bare ESP32-S3 N16R8 devkit + 1.28" SPI display for
the learning build. The 2.1" RGB panel path was dropped — see §4 and §7.

---

## 1. What this device is

A handlebar-mounted navigation device in the style of Beeline Moto: a round display showing a
heading arrow as the primary screen. The OBD/CAN telemetry (tachometer, temps, voltage) is a
**secondary** feature living behind a button press.

**Design principle to preserve:** Beeline works because a rider absorbs roughly one piece of
information at a glance. Do not turn the nav screen into a six-gauge dashboard. The nav arrow is
what's on screen while moving; RPM and diagnostics are what you flick to at a red light.

Explicit requirements set by the user:

- **No phone involvement** in the telemetry path. Data goes bike → device → screen.
- **Lowest possible latency.**
- **Round** display.
- **Sunlight readable** — this is a hard requirement, not a nice-to-have.
- **Buttons, not touch**, for screen switching.

---

## 2. Vehicle specifics (the part that determines everything else)

The 2022 model year matters. Euro 5 compliance means standardized OBD access, which the earlier
Rebels did not have.

| Property             | Value                                    |
| -------------------- | ---------------------------------------- |
| Diagnostic connector | Honda **6-pin** DLC, red, under the seat |
| Protocol             | **ISO 15765-4 (CAN)**                    |
| Identifier length    | **29-bit extended**                      |
| Bitrate              | **500 kbps**                             |

**Two corrections that were made during planning — do not regress on these:**

1. **Pre-2021 Rebels are different.** 2017–2020 CMX300/500 used a **4-pin** DLC running single-wire
   **K-Line** at 10400 baud with Honda's proprietary HDS protocol, not standard OBD2. Some Rebels
   switched from 4-pin to 6-pin partway through the model run. Everything in this document assumes
   6-pin CAN. **Verify the connector by eye before trusting any of it.**
2. **The 6-pin DLC carries both K-Line and CAN-H/CAN-L.** Don't assume any pin you find is the one
   you want.

**Auto-detect is known to fail on this bike.** Generic ELM327 adapters cannot identify the protocol
automatically — it must be forced manually (`ATSP7`, or "ISO 15765-4 CAN 29/500" in a scanner app).
A failed auto-detect is not evidence the bike is dead.

---

## 3. Architecture

> Mermaid harness drawings: [[Wiring Diagrams]] · Step-by-step learning path: [[Learning Roadmap]]

```
Bike 6-pin DLC
   └── Honda 6-pin → OBD2 16-pin adapter cable
         └── OBD2 splitter (1 male → 2 female)
               ├── Port A: device pigtail — CAN-H (pin 6) + CAN-L (pin 14) ONLY
               └── Port B: left free for a diagnostic scanner
                     
Device:
   CAN-H/CAN-L ──> SN65HVD230 transceiver ──> ESP32-S3 N16R8 TWAI (listen-only) ──> 1.28" GC9A01 LCD (SPI)

Power (SEPARATE circuit, NOT from the DLC):
   Battery / fuse tap ──> inline fuse ──> TVS ──> 12V→5V buck ──> ESP32
   Ignition sense: relay or MOSFET triggered from any switched circuit (tail light, switched fuse)
```

### Why listen-only mode

The ECU **broadcasts** RPM periodically (roughly every 10–20 ms) with no request needed. The device
never transmits — it just catches frames off the bus. This gives:

- **~2 ms end-to-end latency** (vs. tens of ms for an ELM327 query loop) — meets the user's stated
  latency requirement, and nothing else in this space is close.
- **Zero risk to the ECU.** No bus collisions, no possibility of tripping an immobilizer or setting
  spurious DTCs. Safe to run while riding, and safe to coexist with a scanner plugged into Port B.
- **Access to Honda proprietary frames**, which standard PID queries cannot reach.

Update the display straight from the CAN RX callback.

### Why no OBD2 dongle

This was a recurring point of confusion worth restating: **an ELM327/Vgate/OBDLink dongle is not
part of this build.** A dongle exists to translate CAN into ASCII for a phone app. The ESP32-S3 has
a CAN controller (TWAI) in silicon — it _is_ the dongle. Adding one would mean two microcontrollers
talking to each other for no reason, at worse latency, in a project whose entire point is the
embedded work.

### Why power does not come from the DLC

An earlier version of this plan took +12V from the diagnostic connector. **That was wrong.** The
DLC's power pin is intended for a handheld scanner, is likely on a shared low-amperage fuse, and
should not be feeding a ~1 A display — let alone a USB charger alongside it. Take power from the
battery or a fuse tap on its own fused circuit. The DLC gives you **two wires only: CAN-H and
CAN-L.**

If more accessories are planned (USB charger, etc.), add a small fused distribution block under the
seat. That's the standard motorcycle solution and it makes the install look deliberate.

---

## 4. Bill of materials

### On the bike

| #  | Part                                                                 | ~Cost  | Function                            | Notes                                                                                                                                                                            |
| -- | -------------------------------------------------------------------- | ------ | ----------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1  | **ESP32-S3 N16R8** devkit (16 MB flash, 8 MB octal PSRAM)           | owned  | MCU                                 | Native USB-JTAG for debugging. TWAI in silicon. **GPIO 35/36/37 are taken by octal PSRAM — do not use.** The user also owns an ESP32-WROOM-32; use it as a bench CAN *transmitter* to replay recorded frames. |
| 2  | **Waveshare 1.28" round IPS 240×240, GC9A01, SPI** (arduino.ua prod6121) | 809₴ | Display                             | 32 mm active — Beeline Moto size. Touch SKU (CST816S) — leave the `TP_*` wires unconnected; touch is not used. 115 KB @ 16bpp framebuffer fits internal SRAM. **~300 nit, unbonded: NOT sunlight-proof** — see §7. |
| 3  | SN65HVD230 CAN transceiver breakout                                  | $3     | CAN physical layer                  | **3.3 V only** — not MCP2551/TJA1050 (5 V). **Desolder the onboard 120 Ω terminator** — the bus is already terminated at both ends.                                              |
| 4  | Honda 6-pin DLC → OBD2 16-pin cable                                  | $15    | Physical tap into the bike          | Bought for its connector; the Honda plug isn't sold loose.                                                                                                                       |
| 5  | OBD2 splitter, 1 male → 2 female, full 16-pin pinout                 | ~$9    | Keeps the diagnostic port available | Braided version preferred for vibration/chafe resistance.                                                                                                                        |
| 6  | Solderable OBD2 male connector with PCB                              | ~$3    | Device pigtail                      | Solder to pins 6 and 14 only.                                                                                                                                                    |
| 7  | 12 V→5 V buck converter, 2 A, automotive-rated                       | $5     | Power                               | Bike voltage swings 12–14.5 V with charging and crank dips. A cheap USB buck browns out and resets the board mid-ride.                                                           |
| 8  | TVS diode SMBJ18A                                                    | $1     | Transient protection                | Motorcycles produce **load dump** spikes of 60–80 V, faster than any regulator reacts.                                                                                           |
| 9  | 2 A blade fuse + inline holder                                       | $2     | Fault current limit                 |                                                                                                                                                                                  |
| 10 | BH1750 ambient light sensor                                          | $2     | Auto-dims backlight                 | Even 300 nits at night is blinding and destroys night vision.                                                                                                                       |
| 11 | IP67 momentary push button, 16 mm metal, NO                          | ×2, $8 | Screen cycling (next / back)        | 16 mm is the smallest reliably hittable with winter gloves.                                                                                                                      |
| 12 | 10 kΩ resistor ×2, 100 nF ceramic ×2                                 | $0.40  | RC debounce                         | The handlebar-to-unit run is an unshielded antenna next to ignition leads. Software debounce alone will produce phantom presses from coil EMI.                                   |

### Bench only

| #  | Part                           | ~Cost | Function                                                                                         |
| -- | ------------------------------ | ----- | ------------------------------------------------------------------------------------------------ |
| 13 | **CANable 2.0** USB-CAN        | $35   | `candump` / `cansniffer` over SocketCAN. Finds the RPM and gear frames. Used once, then shelved. |
| 14 | ELM327 WiFi v1.5 (PIC18F25K80) | ~$9   | Sanity-check tool only. See below.                                                               |

**Total: ~$110** (MCU already owned), of which ~$65 is permanently on the bike.

**On #14:** its only job is de-risking. Three untested assumptions are currently stacked — that the
adapter fits, that pins 6/14 are populated, and that the bike speaks CAN. This answers all three in
five minutes. WiFi (not Bluetooth) because the user is on iPhone and iOS cannot do Bluetooth SPP.
**It does not replace the CANable** — a request/response translator cannot show broadcast frames,
which is where Honda's proprietary data lives.

### Sourcing (user is in Ukraine)

- Rozetka: Honda 6-pin adapter (269₴), SN65HVD230 module (64₴), ELM327 WiFi (380₴)
- carkit.com.ua (Kharkiv, Nova Poshta): OBD2 splitter braided (350₴, art. 111350), plain (270₴, art.
  111030), solderable male connector with PCB (120₴, art. 22)
- arduino.ua: Waveshare 1.28" GC9A01 LCD (809₴, prod6121)

---

## 5. Immediate next steps

### Step 0 — Verify the connector (before anything else)

Lift the seat. Confirm the red DLC has **6 pins**, not 4. If it's 4-pin, this entire plan is wrong
and the project pivots to K-Line — see §7.

### Step 1 — Confirm the cable and the bus

With ignition **off**, plug in the Honda adapter and measure resistance between OBD2 **pin 6** and
**pin 14** on the female socket. Expect **~60 Ω** (two 120 Ω terminators in parallel).

- ~60 Ω → CAN confirmed, proceed.
- ~120 Ω → possibly a stub without local termination; check with a scope.
- Open circuit → the adapter's CAN pins aren't populated. Get a different cable.

**Do this the day the parts arrive, not the week before the demo.**

Standard OBD2 pinout on the adapter's female end (the adapter has already done the Honda mapping):

| Pin   | Signal                        |
| ----- | ----------------------------- |
| 6     | CAN-H                         |
| 14    | CAN-L                         |
| 16    | +12 V _(do not use — see §3)_ |
| 4 / 5 | GND                           |

### Step 2 — Enumerate supported PIDs

With the ELM327 + Car Scanner (force protocol 7: ISO 15765-4, 29-bit, 500 kbps), run the supported-
PID scan. Or do it manually: PID `0x00` returns a 4-byte bitmask of support for `0x01–0x20`; `0x20`
covers `0x21–0x40`; `0x40` covers `0x41–0x60`.

**This is definitive for this specific ECU and replaces all guessing about what's available.** Do it
before designing any screens.

### Step 3 — Find the RPM frame

CANable + `candump can0` / `cansniffer`. Blip the throttle on the stand, watch which two bytes track
monotonically. Record the frame ID and byte offset. While there, shift through the gearbox and watch
for a byte stepping 1→2→3 — that's gear position, which is **not** a standard PID and only exists in
a proprietary broadcast frame.

Log a few minutes of real traffic to a file. That's the fixture for bench development in Step 4.

### Step 4 — Firmware on the bench

ESP32-S3 TWAI: 500 kbps, extended IDs, accept-all filter, **listen-only**. Render the gauge from
recorded frames. No bike needed for this stage. Suggested order: blink + USB serial → GC9A01 via
`esp_lcd` + LVGL arc widget → TWAI listen-only printing frame IDs (WROOM-32 as the transmitter, two
transceivers, 120 Ω at each end, short twisted pair) → decode PID `0x0C` into the arc.

### Step 5 — Integrate

Power chain (fuse → TVS → buck), transceiver, splitter + pigtail, light sensor and buttons, mount.
Then the **noon sun test** (§7) decides whether the 1.28" LCD stays or gets swapped.

---

## 6. Known data and derived values

Reported working on the CMX line via standard PIDs: coolant temp, engine load, ignition timing,
system voltage, throttle position.

| Data              | PID    | Rider value                                                                  |
| ----------------- | ------ | ---------------------------------------------------------------------------- |
| RPM               | `0x0C` | Primary gauge. Decode: `((A << 8) \| B) / 4`                                 |
| Coolant temp      | `0x05` | Liquid-cooled twin in summer traffic                                         |
| Battery voltage   | `0x42` | R/R failure is _the_ common Honda twin failure. Best early warning available |
| Vehicle speed     | `0x0D` | From ECU, not GPS — no dropout, no lag                                       |
| Intake air temp   | `0x0F` | Below ~3 °C = black ice warning                                              |
| Throttle position | `0x11` | Marginal live; good for logging                                              |
| Engine load       | `0x04` | Feeds fuel-consumption estimate                                              |

**Do not expect fuel level (`0x2F`)** — on bikes it's usually a direct analog sender to the dash, not
an ECU PID. Check the bitmask but plan for its absence.

### Derived values (this is the capstone-grade work)

Raw PIDs are a data dump. These are engineering:

- **Computed gear** — speed ÷ RPM falls into six distinct bands. Learn them once and you get a gear
  indicator without ever finding the proprietary frame. Good algorithmic content for the write-up
  and a solid fallback.
- **Smart shift light** — RPM threshold gated on throttle position, so it doesn't fire while
  coasting.
- **Instantaneous economy** — engine load × RPM as a fuel-flow proxy, ÷ speed. Approximate; calibrate
  against a few tank fills. Genuinely useful on an 11 L tank.
- **Overheat / low-voltage alarms** — full-screen red takeover. Safer than a number that has to be
  read and interpreted at 90 km/h.

---

## 7. Risks and fallbacks

| Risk                                     | Mitigation                                                                                                                                                                                                                                                                             |
| ---------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Connector turns out to be 4-pin K-Line   | Pivot to L9637D or SI9241A transceiver + UART, implement fast init and Honda's request/response loop. Reference: the **HondaECU** project (RyanHope on GitHub) has reverse-engineered the protocol fairly thoroughly. This is a multi-week detour — find out in Step 0, not month two. |
| Honda proprietary frames resist decoding | Standard emissions PIDs are mandated by Euro 5 and include RPM. Ship on those; treat gear position as a stretch goal.                                                                                                                                                                  |
| 1.28" LCD fails the noon sun test        | Upgrade path, same size class, display layer swaps behind LVGL: (a) high-brightness 1.28" GC9A01, 500–800 nit (AliExpress, same driver, zero code change); (b) Waveshare ESP32-S3-Touch-AMOLED-1.43 (CO5300, QSPI, ~600 nit, laminated glass, mostly-black UI helps; watch burn-in and heat); (c) Sharp LS012B7DD01 1.2" round memory LCD — reflective, guaranteed in sun, monochrome, needs a sidelight at night. |
| CAN path fails entirely                  | Inductive pickup on the plug lead → comparator → MCU input capture. RPM from pulse frequency × 60 ÷ pulses-per-rev. Crude, needs an extra sensor, but demoable.                                                                                                                        |
| GPIO exhaustion                          | No longer a risk with SPI — ~12 GPIOs total. See pin plan below.                                                                                                                                                                                                                       |

### Pin plan (ESP32-S3 N16R8)

| Function         | GPIO   | Notes                                         |
| ---------------- | ------ | --------------------------------------------- |
| TWAI RX          | 4      | from SN65HVD230 `R`                           |
| TWAI TX          | 5      | to SN65HVD230 `D` — assigned but never driven |
| LCD SCLK         | 12     |                                               |
| LCD MOSI         | 11     |                                               |
| LCD DC           | 10     |                                               |
| LCD CS           | 9      |                                               |
| LCD RST          | 14     |                                               |
| LCD BL           | 13     | LEDC PWM, driven from BH1750                  |
| I²C SDA / SCL    | 8 / 18 | BH1750                                        |
| Button next/back | 1 / 2  | 10 kΩ pull-up + 100 nF RC at the pin          |

**Avoid GPIO 0, 3, 45, 46 for buttons.** They're strapping pins; a button held at power-on boots the
chip into download mode instead of running firmware. GPIO0 is the classic mistake. **Avoid GPIO
35–37 entirely** — octal PSRAM on the N16R8.

### Display constraint worth remembering

**Size decision:** the user wants a Beeline-sized device. 1.28" = 32 mm active, which is Beeline
Moto's screen size; 2.1" (53 mm) was judged too big for a handlebar tachometer. Touch is not wanted.
The 1.28" GC9A01 was chosen as the **simplest possible display to learn on** (4-wire SPI, six wires,
best-documented round panel, LVGL built-in driver), accepting that it is ~300 nit and unbonded.

**Expected daylight behaviour of the 1.28" LCD:** excellent at night/overcast, readable in shade,
**washed out in direct sun** (glossy unbonded cover reflects more light than the panel emits). Two
cheap fixes that matter more than extra nits: a printed **sun hood** (15–20 mm lip, top and sides)
and a **matte AR film** on the cover glass. Then the noon sun test with the real UI decides whether
the panel is upgraded — see the risks table for the three upgrade options.

**Keep the display layer behind LVGL from day one** so any panel swap is one driver file.

**E-paper cannot render RPM.** It was considered and rejected — refresh is far too slow for a
fast-changing value. The Sharp memory LCD (LS012B7DD01 round) remains the guaranteed-in-sunlight
fallback: reflective, microamp static power, handles 20+ Hz. Monochrome, and needs a sidelight for night.

Brightness alone doesn't solve daylight readability. Outdoor light is 10,000–100,000 lux; on a
typical 300-nit panel, surface reflections push effective contrast below 1:1. The fix is bright
backlight **plus** killing reflections — optical bonding and AR coating. When ordering, ask for
"1000 nits **and optically bonded**."

Two free wins worth more than an extra 200 nits: a printed **sun hood** shading the top and sides,
and a UI of pure white on pure black with a fat needle and no gradients. Fine detail and mid-tones
are the first things to disappear in sunlight.

---

## 8. Firmware notes

- **Buttons:** GPIO interrupt → set flag → handle the screen change in the main task. Do **not** poll
  in the render loop. The CAN RX callback and panel refresh are the latency-critical paths and
  button handling should be nowhere near them.
- **Backlight:** LEDC PWM on the GC9A01 `BL` pin, driven from BH1750 readings. Hysteresis so it
  doesn't flicker under passing shadows.
- **Toolchain:** ESP-IDF 5.x (not Arduino) — `esp_lcd` GC9A01 panel driver + LVGL.
- **Bench development:** replay recorded CAN frames from Step 3 rather than needing the bike powered.
- **Protocol discovery, if any active querying is ever needed:** do it on the stand, never while
  riding.
