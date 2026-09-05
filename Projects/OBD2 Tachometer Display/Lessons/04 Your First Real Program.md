# 04 — Your First Real Program

Blink. Every embedded engineer's first program, and for a good reason: it's the smallest thing that proves your code can reach the physical world.

## Start a repo first

Make a folder for the firmware *outside* Obsidian — it'll be a real git repo with a build directory. Copy the `hello_world` project into it and `git init`. Add a `.gitignore` with `build/`, `sdkconfig.old`, and `managed_components/`. Commit.

From now on, commit at the end of every lesson. Later, when something that worked stops working, you'll want to know whether it was the code or the wiring that changed — and git can answer half of that question.

## What a GPIO is

GPIO stands for "general-purpose input/output." It's a pin on the chip that you, the programmer, can set to be either an output (you decide whether it's at 3.3 V or 0 V) or an input (you read whether someone else has put it at 3.3 V or 0 V). Everything you connect to the ESP32 — display, buttons, transceiver — ultimately goes through GPIOs.

An LED is the simplest possible output. Set the pin high, it lights. Set it low, it doesn't.

## What to do

Your devkit has an onboard LED. On most S3 devkits it's an addressable RGB LED on GPIO 48, driven by a one-wire protocol that the `led_strip` component handles. The ESP-IDF `blink` example supports it out of the box — create a new project from that example and check the LED GPIO in the SDK Configuration Editor matches your board (look at the board's silkscreen or the seller's page; 48 is the usual answer, sometimes 38).

If you'd rather have a plain LED you understand completely: any LED, a 330 Ω resistor in series, from a free GPIO (say, 15) to GND. The resistor limits current so you don't cook the LED or the pin. Set the example to "GPIO" mode and that pin.

Build, flash, monitor. It blinks.

Now change the delay. Rebuild. It blinks at a different rate. That's the whole loop.

## Read the code — there's an OS in there

Open `blink_example_main.c`. Notice two things.

First, `app_main()` is your entry point, but it's not `main()`. That's because **FreeRTOS is already running** before your code starts. `app_main` is itself a task — a thread — that the system creates for you. You can create more. Later, the display will have its own task, and CAN will have its own task, and they'll run concurrently.

Second, `vTaskDelay(...)` is not `sleep()`. It tells the scheduler "I have nothing to do for N ticks, run someone else." If you busy-loop instead, you starve every other task — including the one that feeds the watchdog timer, which will then reset the chip on you with a cryptic error. Use `vTaskDelay`. Think of it as `await` in an async runtime.

Also notice `gpio_reset_pin` / `gpio_set_direction` / `gpio_set_level`. That three-step shape — configure the pin, set its direction, then use it — is the same for every peripheral.

## Done when

It blinks, you've changed the rate, and it's committed.

Next: [[05 Wiring the Display]] — your first real wiring.
