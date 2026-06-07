# Survivor AI — JavidKhashayarZombie plugin

This Game Feature plugin is the brain of the survivor. The game spawns the world,
houses, items and zombies; we can't touch the survivor pawn directly, so everything
here is bolted on at runtime and driven by a behavior tree. Here's the short story of
what it does and how it grew.

## How it hooks in
The survivor pawn is read-only, so I **inject** my components onto it at runtime
(`SurvivorComponentInjector`, kicked off by the first BT node `BTTask_InjectComponents`).
Those components find each other with `FindComponentByClass` — no hard wiring.

## The behavior tree (priorities, highest first)
1. **Escape Purge** — if standing in (or near) a purge zone, drop everything and run out.
2. **Handle Threat** — a zombie is close, deal with it.
3. **Pickup Item** — grab something worth grabbing.
4. **Explore House** — walk through a house and loot it.
5. **Spiral** — nothing else to do, wander and look for houses.

Each branch can interrupt the lower ones the instant its condition flips (the decorators
re-poll every tick), so the agent always reacts immediately instead of finishing what it
was doing.

## Exploring the world (`SpiralMovement`, `HouseTracker`)
- I scan the whole map for houses up front, so even far-away villages are known.
- The agent heads to the **nearest unvisited house**, sweeps it with a small grid of
  points, and remembers it. Visited houses go on a **15-house cooldown** so it doesn't
  keep re-entering the same ones — it spreads out and comes back much later for respawned loot.
- When no house is known it falls back to a real **expanding spiral** 

## Handling items (`ItemHandler`)
- Aims for a tidy loadout: **pistol + shotgun + medkit + 2 food**.
- Keeps the better copy (more ammo / higher value), and **throws out garbage and spent
  items** so the world's item pool keeps respawning fresh stuff.
- **Uses a medkit when hurt and food when tired**, automatically.
- Grabs items *while* exploring a house (not after), and never chases something it has no room for.

## Fighting zombies (`ThreatHandler`)
- Detects zombies in every direction (not just its view cone), so something sneaking up
  behind still gets dealt with.
- **Turns to face**, then shoots only when it has a clear line of sight and the target is in
  range — **pistol for distance, shotgun up close** where its spread actually lands.
- **Kites backwards** when a zombie gets too close, and only **sprints** when it really needs
  to (to save stamina). If it has no gun, it **runs for a weapon or a house** instead of
  just fleeing, then turns around and kills.

## Escaping purge zones (`PurgeEscaper`)
- Reads each zone's size and bolts for a point that's **guaranteed outside every zone**,
  reacting early (before it's deep inside) and not stopping until it's well clear.

## Debug
Lots of on-screen messages and debug draws (spiral path, exploration points, escape lines,
shooting state) to make the decision-making visible while testing.
