# 15 — Finding the RPM Frame

Reverse-engineering, in the most pleasant possible form: change one thing on the bike, watch which bytes change on the laptop. You'll find the RPM frame in a few minutes and maybe the gear position as a bonus.

## Optional first: the five-minute sanity check

If you bought the ELM327 WiFi adapter, now's its moment. Plug it into the OBD2 socket, connect your phone to its WiFi, open Car Scanner, and — this is the part that trips people up — **set the protocol manually** to "ISO 15765-4 CAN 29-bit 500 kbps" (protocol 7). Auto-detect is known to fail on this bike, and a failed auto-detect looks exactly like a dead bus. Start the engine. You should see live RPM.

While it's connected, run the *supported PIDs* scan and paste the list into [[OBD2 Tachometer Display]] §6. That list is the definitive answer to "what data can this ECU give me" — coolant temperature, voltage, speed, and so on. Design screens from it, not from guesses.

Then unplug it. It's done its job. The ELM327 can only ask questions; it can't show you the broadcast traffic, and the broadcast traffic is what your device lives on.

## The real tool: CANable + candump

The CANable is a USB-to-CAN adapter that Linux (and macOS via a small daemon) sees as a network interface. You plug it into the bike, and the laptop becomes a node on the bus that can dump every frame.

Plug it in through the OBD2 splitter so the socket's still free. Then on the laptop:

```
sudo ip link set can0 up type can bitrate 500000 listen-only on
candump -l can0          # log everything to a file
```

in one terminal, and

```
cansniffer can0
```

in another. `cansniffer` is the magic one: it shows one line per frame ID, and **highlights bytes that are changing**. Static bytes fade; live ones jump out.

(On macOS, SocketCAN isn't native. Either use a Linux VM with USB passthrough, or `python-can` with the `slcan` backend and a short script that does the same job.)

## What to do

1. Bike on its stand, ignition on, engine running at idle.
2. Watch `cansniffer`. You'll see a handful of IDs updating constantly. Ignore them for a moment.
3. **Blip the throttle.** Watch for two adjacent bytes that rise and fall together with the engine note. That's RPM. Note the ID and the byte offset. Check the scale: at idle (~1,300 rpm) the raw 16-bit value should be around 5,200 if it's the standard ×4 encoding, or 1,300 if it's unscaled. Write it down either way.
4. **Pull the clutch and click through the gears** (stand, rear wheel free — be careful). Watch for a single byte stepping 1, 2, 3, 4, 5, 6. That's gear position, which is *not* a standard OBD value and only exists in Honda's own broadcast frames. If you find it, you've saved yourself the computed-gear algorithm later. If you don't, no loss — it's a stretch goal.
5. Let it idle for two or three minutes with `candump -l` running, rev it a few times, shift a few times. That log file is your fixture.

## What you'll see

Probably somewhere between five and twenty distinct IDs, most of them updating every 10–50 ms. The IDs will be 29-bit (long hex numbers). Exactly which one carries RPM is the thing you can't know until you look — Honda doesn't publish it, and it's the reason this step exists.

## Done when

You have **(frame ID, byte offset, scale)** for RPM written in [[OBD2 Tachometer Display]] §6, the same for gear if you found it, and a `candump` log copied into the repo for the replay tool. Run the lesson 13 script on it and confirm the WROOM can play it.

Next: [[16 First Contact]].
