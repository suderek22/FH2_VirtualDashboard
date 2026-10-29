# VirtualDashboard changelog

## v1.3.6 (changes since v1.2.0)

- New: **layout selector** in the Mods menu (needs FH2 Mod Platform 2.8.0 or newer): Default (unchanged), Alternative (red needles, nitro and boost as straight strips under the dials) and Revuelto (one central rev counter with a white filling line that turns red past the red zone, a thin red tick at the end of the line, tick marks, italic digits, a blue four-quarter nitro ring around the counter, a boost gauge on the left, gear, speed, indicators, headlights and clock). Setting: `Layout=0/1/2` in the ini; the JSON feed has a `layout` field.
- New: **turn signals and hazards are read from the game's car object** instead of emulating the D-pad (see "Turn-signal technical notes").
- Fixed: on a slow game start the dashboard stayed on "Waiting for a car" because the hook gave up waiting for the game code after 30 s. It now waits up to 120 s.
- The page footer shows the version.

### Turn-signal technical notes (for other modders)

Forza Horizon 2 has no turn signals of its own. The ones in this game come from the [ForzaHorizon2_ExtraOptions](https://github.com/RokkuDayo/ForzaHorizon2_ExtraOptions) mod (`winmm.dll`, `features/blinkers.c`), which switches the car's light flags. VirtualDashboard only reads that state, so it needs Extra Options installed.

- The three flags are bytes on the local player's car object (the same object the HUD hook receives at `hud + 0xE8`): `car + 0x51BB` = hazards, `car + 0x51BC` = left, `car + 0x51BD` = right. Each is 0 or 1. Nearby: `car + 0x51A1` / `0x51A2` = manual / forced headlights, `car + 0x51B4` = automatic headlights.
- Extra Options writes these bytes just before calling the game's car update (`game + 0x2A9400`) and the game consumes them during that update. At the time a HUD hook runs they are usually already cleared, so a read from the HUD hook sees nothing. Readings taken once per frame therefore miss the signals; this is why earlier versions showed no indicators.
- Solution used here: a small background thread polls the three bytes in a tight loop (`Sleep(0)`, a `Sleep(1)` every 64 passes) and stores the tick of the last time each byte was seen set. A signal counts as on while it was seen within the last 250 ms. The HUD hook (about 30 ms cadence) then publishes `ind` from that latch: `3` = hazards (or left and right together), `1` = left, `2` = right, `0` = off.
- The poller reads memory inside `__try/__except`. It uses the car pointer saved by the HUD hook, so it follows car swaps.
- The page animates the blinking itself (the flags are steady while a signal is on).
- The log prints `Indicators now N` whenever the state changes, which is handy for debugging.
- To find the player's car without the HUD hook, ExtraOptions uses `*(game + 0x34ABA60)` as the player manager, calls `game + 0x21E0C0(manager, 0)` for the player and `game + 0x2DA060(player, &car)` for the car (vtable `game + 0x2C5CB80`; release through vtable slot `+0x18`). VirtualDashboard logs both addresses once (`Player car ..., HUD car ...`); they were identical.
- Extra Options needs a hold of 0.5 s on D-pad left/right to start a signal (up for hazards); a tap keeps the vanilla radio action.
## v1.2.0

- Nitro gauge: pink, four even sections, left of the speedometer. It reads the tank from FnFNitro 1.4.0 (optional; the gauge stays hidden if FnFNitro is not installed).
- Empty tank: the gauge shows a red outline. It pulses for 2 seconds when the tank runs out (or until it refills), then stays solid while the tank is empty.
- Turbo gauge: small gauge (-1 to 3 bar) at the top right of the RPM gauge. It appears once a non-zero boost is read and stays until the next car change, game start or page refresh, so cars without a turbo never show it.
- Two new Mods menu settings, "Dashboard nitro gauge" and "Dashboard turbo gauge" (ini: `NitroGauge`, `TurboGauge`, default 1). They only show or hide the gauges; telemetry is always read.
- `/api` JSON has a new `nitro` field (0-100, -1 when FnFNitro is not loaded). The raw telemetry list shows "Nitro tank (%)".
- The speedometer and RPM dials are drawn about 12% smaller while their extra gauge is enabled.
- Automatic update check through the Mod Platform (GitHub: suderek22/FH2_VirtualDashboard).

## v1.1.0

- Dashboard keeps working in all game menus, including the in-game Telemetry screen. "Waiting for a car in the game..." shows only during loading, after a car swap or when the connection is lost.
- Tap the gear to show the current speed in its place; tap again to go back.
- Digital RPM is rounded to 50 by default (tap the gauge to switch).
- Raw telemetry list: brake, handbrake, turbo boost (bar), power (kW), torque (Nm), game time and weather values.
- Fixed: the old "Brake" value was actually the clutch; it is now labelled Clutch.

See `CHANGELOG.txt` in the release folder for older versions.





