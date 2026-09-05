# 00 — Start Here

You're a software engineer about to build a piece of hardware. The good news is that about 70% of this project is software — you'll feel at home quickly. The other 30% is the part nobody taught you, and it's why you feel lost right now. This lesson is about how to think so the 30% stops being frightening.

## Why this feels confusing

Your handoff document is excellent and complete. That's the problem. It describes the *finished* device — every part, every protection circuit, every failure mode — all at once. Reading it is like reading a production Kubernetes config before you've ever run a container. Everything in it is true and none of it tells you what to do on Tuesday.

So let's split it up.

## It's three projects

**Project 1 — a screen.** An ESP32 dev board and a small round display, connected with six jumper wires, powered by the USB cable from your laptop. You write code that draws a gauge. No motorcycle. No soldering. Nothing that can hurt you or anything you own.

**Project 2 — listening.** The same board, plus a small chip called a CAN transceiver, hearing messages from a second ESP32 that you program to *pretend* to be the bike. Still on your desk. Still USB. The most expensive thing you could break is a $3 module.

**Project 3 — the motorcycle.** Now you take the working device and put it on the bike: find the real messages, wire up power safely, build a case, ride in the sun.

Almost every scary word in the handoff doc — TVS diode, buck converter, load dump, relay, debounce — belongs to project 3. You are allowed to not understand any of it until you get there. By the time you do, you'll have a working gadget on your desk and a reason to care.

## The one habit that matters

In software, your feedback loop is *edit → run → look at the error*. Errors are descriptive, nothing is destroyed, and you can change five things at once and still figure out what happened.

Hardware is different in three ways, and internalising them now will save you weeks:

**1. There are no error messages.** A wrong wire doesn't throw. It just does nothing, or shows garbage. Silence is the most common failure mode, and it gives you no hint. That's why every lesson here tells you what "broken" is going to look like and what to check first.

**2. Change one thing at a time.** If you rewire two pins and change the code and it stops working, you've lost the ability to bisect. Move one wire. Test. Move the next.

**3. Power off before you touch wires.** Unplug the USB cable before rewiring. Not because the voltages are dangerous to you — they aren't, 3.3 V can't hurt a person — but because a wire that brushes the wrong pin while powered can kill a chip silently. You won't know until an hour later when nothing makes sense.

And a fourth, which is just practical: **photograph every working state.** When it worked yesterday and doesn't today, the photo tells you which wire moved.

## What you need to buy before anything else

A **multimeter**. Any $15 one. It's the embedded engineer's debugger — it answers "is there voltage here" and "are these two points connected", which are the two questions behind almost every hardware bug. You'll use it in lesson 14 for real, but get it now so you have it.

Also check that your USB-C cable actually carries data. A surprising number are charge-only, and "the board doesn't show up" after an hour of driver fiddling is a miserable way to find out.

## How to use these lessons

Each one has the same shape: *why this step exists*, *the idea behind it* (explained once, properly), *what to actually do*, *what you'll see*, and *how you know you're done*. Don't skip the "done when" — it's the commit point. Commit your code there, too.

Next: [[01 Count the Pins]].
