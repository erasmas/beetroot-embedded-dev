# 07 — Drawing the Gauge

By the end of this lesson you'll have a tachometer. It will be lying — the number comes from a loop, not an engine — but it'll look like the product, and that changes how the rest of the project feels.

## What LVGL is

LVGL is a UI toolkit for microcontrollers. You create widgets — an arc, a label, a button — set their properties, and LVGL renders them into a pixel buffer and hands that to the display driver from the last lesson. It has layouts, styles, animations, and fonts. If you've used any retained-mode UI framework (think Qt, or the DOM), it'll feel familiar.

The important architectural rule from your notes: **everything on screen goes through LVGL.** No drawing pixels directly. This is what makes the display swappable — if the 1.28" panel fails the sun test, you replace the panel driver underneath LVGL and every screen you've built carries over unchanged.

## The threading rule

LVGL has one rule that will bite a software engineer who doesn't know it: **it's not thread-safe.** One task runs LVGL's internal timer (which handles rendering, animations, and input). If any *other* task touches a widget at the same time, you get corruption or a crash.

The `esp_lvgl_port` component you're about to use handles this by running LVGL in its own task and giving you a lock. Any time you touch a widget from elsewhere — and the CAN receiver will, in lesson 12 — you wrap it:

```c
if (lvgl_port_lock(0)) {
    lv_arc_set_value(arc, rpm);
    lvgl_port_unlock();
}
```

Same discipline as a UI thread in a desktop app. Learn it now with fake data; it won't surprise you later.

## What to do

1. Add the LVGL port component: `idf.py add-dependency "espressif/esp_lvgl_port"`. This pulls in LVGL itself and the glue to `esp_lcd`. The example you started from in lesson 06 already uses it — read how it creates the display handle and registers it with the port. That's the boundary between "display" and "UI."

2. Strip the example's demo and build your own screen:
   - Black background. `lv_obj_set_style_bg_color(scr, lv_color_black(), 0)`.
   - An `lv_arc` — the round gauge. Set its range to 0–9000, its angles to sweep about 270° with the gap at the bottom, a thick white indicator, a thin grey background track.
   - An `lv_label` in the centre with a big font (LVGL ships Montserrat up to 48pt; enable the size you want in the SDK Configuration Editor under LVGL → Font usage). This shows the number.

3. Fake the engine. Create a FreeRTOS task that sweeps a variable from 800 to 8000 and back over a few seconds, and every 20 ms takes the lock, updates the arc and the label, releases it.

## Design rules, and why

Your notes set these, and they're not aesthetic preferences — they're sunlight physics:

- **Pure white on pure black.** Mid-tones and greys vanish first in direct sun. Maximum contrast is the only thing that survives.
- **One piece of information per screen.** The original Beeline works because a rider absorbs one thing per glance. RPM is *the* thing on this screen; everything else is a different screen.
- **Fat shapes.** A thick arc and a big number. Thin needles and small text disappear.

Resist the six-gauge dashboard. You can always add; you can't un-clutter at 90 km/h.

## Done when

The arc sweeps, the number follows it, it looks like a tachometer. Commit. Show someone — this is the first moment the project looks real, and you should enjoy it.

Next: [[08 Buttons]].
