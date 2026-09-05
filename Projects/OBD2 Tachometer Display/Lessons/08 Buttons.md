# 08 — Buttons

Two buttons, NEXT and BACK, to cycle screens. Simple hardware, and the excuse to learn the one software pattern that every embedded program is built on.

## GPIO as input

In lesson 04 a GPIO was an output: you set it high or low. Now it's an input: you read whether something *else* has put it high or low.

A button is a gap in a wire. Pressed, the gap closes. So wire one side of the button to the GPIO and the other side to GND, and pressing it connects the pin to ground — reads 0.

But what does it read when *not* pressed? Nothing is connected. The pin floats, picking up whatever stray charge is nearby, and reads randomly. You need something gently holding it high when the button isn't pulling it low. That's a **pull-up resistor** — a weak connection to 3.3 V. The ESP32 has one built into every pin; you just enable it in software (`GPIO_PULLUP_ENABLE`). So: unpressed reads 1, pressed reads 0. "Active low." It's backwards from what you'd expect and it's the convention everywhere.

## Why not just poll

You could read the pin in a loop every few milliseconds. For a bench demo it'd work. But your notes are specific: the CAN receiver and the display refresh are the latency-critical paths, and button handling should be nowhere near them. Polling means a loop, and a loop is another thing competing for the CPU at the wrong moment.

Instead, use an **interrupt**. You tell the chip "when this pin goes from high to low, stop whatever you're doing and run this function." The function is called an ISR — interrupt service routine.

## The pattern: ISR → queue → task

ISRs run with the rest of the system frozen. They have to be tiny and fast, and they can't do anything that blocks or allocates — and they absolutely must not touch LVGL. So the ISR does one thing: puts a message on a queue and returns.

```
ISR:   "button 1 was pressed" → xQueueSendFromISR(queue, &btn_id, ...)
Task:  loops on xQueueReceive(queue, &btn_id, portMAX_DELAY)
       → takes the LVGL lock → switches screen → releases
```

The task blocks on the queue, costing nothing until a press arrives. Then it does the real work in a normal context where blocking and locking are fine. This shape — a tiny ISR posting to a queue drained by a task — is how you'll handle CAN frames in lesson 12, and how essentially all real-time firmware is organised. Learn it here where the stakes are low.

## Bounce

A mechanical button doesn't close once. The contacts chatter for a millisecond or two, producing a burst of high-low-high-low before settling. Your interrupt fires for every edge and you get five "presses" from one push.

On the bench, handle it in software: in the task, ignore any press that arrives within 50 ms of the last one. On the bike you'll add a hardware fix too (lesson 18), because the wires to the handlebar will pick up electrical noise from the ignition and the software lockout alone won't be enough. That's an "on the bike" problem. Don't solve it yet.

## What to do

1. Two tactile buttons on a breadboard. Each: one leg to a GPIO, the other to GND. Use **GPIO 1** for NEXT and **GPIO 2** for BACK.
2. Don't use GPIO 0, 3, 45 or 46. They're *strapping pins* — the chip reads them at reset to decide how to boot. A button held on GPIO 0 at power-on puts the chip into download mode instead of running your code, and you'll spend an hour thinking the firmware's corrupt.
3. Configure both pins: input, pull-up enabled, interrupt on falling edge. Install the GPIO ISR service, attach one handler for both pins.
4. Create a queue and a button task. ISR posts the pin number; task receives, debounces, switches screen.
5. Make a second screen — anything, a label saying "Screen 2" — so there's something to switch to.

## Done when

NEXT and BACK cycle between the gauge and the placeholder screen, a single push gives a single switch, and the gauge keeps sweeping smoothly while you mash buttons. Commit.

That's the end of the screen project. Next: [[09 What CAN Actually Is]].
