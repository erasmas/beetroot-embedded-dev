# 01 — Count the Pins

Five minutes, no tools, and it's the most important step in the whole project.

## Why

Everything in your plan assumes the bike speaks **CAN** — the standard modern vehicle bus — through a **6-pin** diagnostic connector. That's true for a 2022 Rebel 500, because Euro 5 regulations forced Honda to add standard OBD access.

But the Rebel was sold for years before that with a **4-pin** connector running a completely different system called K-Line: a single wire, a slow serial protocol, and Honda's own proprietary commands. If your bike turns out to have that connector, the transceiver you've bought is the wrong part, the CAN code you're about to write is the wrong code, and the project becomes a reverse-engineering job that takes weeks longer.

You don't want to discover that in month two. You want to discover it today, by looking.

## What to do

1. Lift the seat.
2. Find the diagnostic connector. It's **red**, with a rubber dust cap, usually tucked along the frame near the battery or the rear of the tank.
3. Pull the cap and count the pin holes.
4. Take a photo.

## What you'll see

- **6 holes** → CAN bike. The plan is right. Write "**6-pin, verified <date>**" at the top of [[OBD2 Tachometer Display]] and move on.
- **4 holes** → K-Line bike. Stop. The plan needs a different transceiver (an L9637D or similar), a UART instead of CAN, and the HondaECU project on GitHub as your protocol reference. Come back and we'll re-plan — it's not the end of the project, but it's a different project.

## One more thing while you're in there

Notice where the connector sits and how much room there is around it. Your adapter cable, a splitter, and eventually a small power circuit all need to live under this seat. It's worth a second photo of the whole space.

Done when: the photo is saved and the note says which connector you have.

Next: [[02 The Five Ideas You Need]].
