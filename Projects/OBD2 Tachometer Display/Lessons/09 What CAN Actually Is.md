# 09 — What CAN Actually Is

No wiring in this lesson. Ten minutes of reading so that the next four lessons make sense, explained for someone who already knows networking.

## The frame

A CAN message — a *frame* — is tiny: an **ID** and up to **8 bytes** of data. That's the whole thing. No source address, no destination, no length prefix beyond a 4-bit count, no checksum you have to care about (the hardware does it).

The ID is either 11 bits ("standard") or 29 bits ("extended"). Your bike uses 29-bit IDs. It's a setting; it has to match or you hear nothing.

## There are no addresses

This is the part that's unlike anything in IP networking. Every frame is broadcast. Every node on the bus sees every frame. The ID isn't "who it's for" — it's "what it is." A node that cares about engine RPM listens for the ID that means "engine RPM." A node that doesn't, ignores it. Think pub/sub with topics, no broker.

It means nobody has to ask the ECU for RPM. The ECU publishes it, every 10–20 ms, forever, whether anyone's listening or not. Your device just subscribes. This is why there's no ELM327 dongle in your design — a dongle is for *asking*, and you don't need to ask.

## Arbitration

Any node can transmit whenever the bus is idle. If two start at once, the one with the lower ID wins and the other backs off — automatically, in hardware, without corrupting either frame. Low IDs are high priority. You'll never transmit, so this is just background.

## Two wires, one signal

CAN is *differential*. The two wires are called CAN-H and CAN-L. To send a 0, the transceiver pulls CAN-H up and CAN-L down; to send a 1, it lets them both rest at the middle. The receiver doesn't look at either wire's voltage — it looks at the *difference* between them.

Why bother? Because electrical noise — from the ignition coil, the alternator, a passing truck — hits both wires equally, and equal noise on both sides cancels out of the difference. That's why CAN won in vehicles, and it's why the pair has to be **twisted**: twisting ensures both wires pick up the same noise.

## The two chips you need

Your ESP32 has a CAN controller in silicon. Espressif calls it **TWAI** (for trademark reasons; it's CAN). It handles frames, IDs, arbitration, checksums — the whole protocol. But it speaks in ordinary 3.3 V logic on two pins, `TX` and `RX`.

Turning that into the differential pair is the **transceiver**'s job — your SN65HVD230. It's a dumb physical-layer part: logic in, differential out, and the reverse. You always need both: controller for the protocol, transceiver for the wire.

Controller `TX` → transceiver `D`. Transceiver `R` → controller `RX`. Four wires total with power and ground.

## Termination, again

Idea 4 from lesson 02. The bus needs a 120 Ω resistor at each physical end, and only there. Your transceiver modules have one soldered on. On the bench you'll build a two-node bus, so both modules keep theirs. On the bike you'll be a third node in the middle of a bus that already has its two, so that module loses its resistor. Silence is the symptom of getting this wrong in either direction.

## Listen-only

The TWAI controller has a mode where it never drives the bus — not to send, not even to send the acknowledgement bit that every receiver normally sends. It's electrically invisible. Your device runs in this mode permanently. The ECU can't tell it's there, so there's no way to upset it, set a fault code, or interfere with a diagnostic scanner plugged in alongside. It's what makes the device safe to use while riding.

The one wrinkle: a transmitter that never gets acknowledged assumes something's wrong and retries. On the real bike other nodes acknowledge, so it's fine. On your bench, where the only other node is your listen-only S3, the fake-bike transmitter needs to be told not to expect ACKs. You'll see that setting in lesson 11.

## The three things that must match

Bitrate (500 kbit/s), ID length (29-bit), and termination (exactly two). If any of them is wrong, nothing arrives and nothing complains. When the bus is silent, check those three, in that order, before anything else.

Next: [[10 Build a Fake Bike]].
