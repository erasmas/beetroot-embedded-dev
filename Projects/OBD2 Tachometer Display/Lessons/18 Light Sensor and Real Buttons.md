# 18 — Light Sensor and Real Buttons

Two finishing touches that both turn out to teach something: an I²C sensor for the backlight, and the reason the bench buttons won't survive the handlebar.

## The light sensor

Your display is bright enough to be painful at night — 300 nits in the dark wrecks your night vision and turns the road in front of you black. So it needs to dim automatically. That's the BH1750: a light sensor that reports lux over I²C.

**I²C** is the third bus (idea 3). Two wires — `SDA` (data) and `SCL` (clock) — shared by every I²C device, each of which has an address. The ESP32 sends "address 0x23, measure" and reads back two bytes. Request/response, like HTTP. Slow (100–400 kHz) and that's fine for a sensor you read ten times a second.

Wiring: `VCC` → 3V3, `GND` → the single ground node, `SDA` → **GPIO 8**, `SCL` → **GPIO 18**. The module almost certainly has pull-up resistors on SDA and SCL already (I²C needs them; the wires rest high and devices pull them low). If it doesn't, enable the ESP32's internal ones in the I²C config.

Code: ESP-IDF's `i2c_master` driver, and either a BH1750 component from the registry or a dozen lines of your own — send the "continuous high-res mode" command (`0x10`), then read two bytes, divide by 1.2, that's lux.

**The backlight** is the `BL` wire from lesson 05, which you've been holding high. Now drive it with **LEDC**, the ESP32's PWM peripheral: a hardware timer that switches the pin on and off at, say, 5 kHz with a duty cycle you control. 100% duty = full brightness, 10% = dim. The eye can't see 5 kHz flicker.

Map lux to duty. Something like: under 10 lux → 10%, over 1000 lux → 100%, logarithmic between. And add **hysteresis** — don't change brightness until the lux has moved by a good margin and stayed there for half a second. Otherwise a passing shadow from a tree flickers the screen, which is maddening.

## The real buttons

The bench buttons on a breadboard had a few centimetres of wire. The real ones are on the handlebar, a metre of wire away from the box under the seat, running alongside the ignition leads.

A metre of unshielded wire is an antenna. The ignition coil fires thousands of volts a few times per revolution, and a little of that couples into your button wires as brief spikes. The ESP32 sees a spike as a press. Your software lockout from lesson 08 doesn't help — these aren't bounces, they're real-looking pulses at random times. The gauge will switch screens on its own at 6,000 rpm.

The fix is hardware, and it's two parts per button, right at the GPIO pin: a **10 kΩ resistor** from the pin to 3V3 (a proper external pull-up, stronger than the internal one), and a **100 nF capacitor** from the pin to ground. The capacitor takes about a millisecond to charge or discharge through the resistor, so any pulse shorter than that never moves the pin enough to register. A real press, which lasts tens of milliseconds, goes through fine. This is called an RC filter, and the R times C — 10k × 100n = 1 ms — is its time constant.

Wire the two IP67 buttons: one side to the GPIO (1 for NEXT, 2 for BACK), the other to the ground node. RC parts at the ESP32 end, not the button end. Keep the software lockout too; belt and braces.

## Done when

Cover the sensor with your hand: the screen dims, smoothly, after a beat. Shine a phone torch on it: it brightens. Start the engine, rev it, and press nothing — the screen must not switch on its own. Then press NEXT at 5,000 rpm and it switches once. That's the EMI test passed.

Next: [[19 Enclosure and the Sun Test]].
