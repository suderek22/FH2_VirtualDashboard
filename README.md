# VirtualDashboard v1.4.0

A mod for Forza Horizon 2 (Azimuth 1.0.0.64) that serves a live dashboard on your local network. Open it on your phone or tablet to see speed, RPM, gear and more while you drive.

## Features

- Speedometer (tap to switch km/h and mph) with the scale adjusting to your speed.
- RPM gauge with the redline read from the current car. The digital RPM is rounded to 50 by default; tap the gauge to toggle.
- Gear (tap it to swap with the speed), headlights icon, and turn-signal and hazard arrows (blinking like the game).
- Car name, plus a gauge sweep animation on every car change.
- A hidden list of raw telemetry values (arrow button): speed, rpm, gear, throttle, brake, clutch, handbrake, turbo boost, approximate power and torque, game time and weather, with a footer under it.
- Keeps showing live data in game menus (including the in-game Telemetry screen).
- Optional nitro gauge (pink, four segments, left of the speedometer) fed by FnFNitro 1.4.0. A red outline marks an empty tank and pulses for 2 s after the tank runs out.
- Optional small turbo gauge (-1 to 3 bar) at the top right of the RPM gauge. It appears once a non-zero boost is read and stays until the next car change, game start or page refresh, so it never shows on cars without a turbo.
- Three layouts, chosen in the game: **Default** (two dials), **Alternative** (red needles, nitro and boost as straight strips under the dials) and **Revuelto** (one central rev counter with a white filling line, tick marks, italic digits, gear, speed, indicators, headlights, clock and boost gauge; the nitro tank wraps the rev counter as a blue ring in four quarters).
- Turn signals and hazards are read from the game itself.
- ABS, TCS and STM state are read from the game (the same getters the HUD uses).
- Works in portrait and landscape.

## Requirements

- Forza Horizon 2 running through Azimuth, version 1.0.0.64.
- [FH2 Mod Platform](https://github.com/Trinculo54/FH2-Mod-Platform) v2.8.0 or newer (needed for the layout selector).
- [FnFNitro](https://fh2mods.com/) 1.4.0 is optional, only for the nitro gauge.
- Extra Options (`winmm.dll`) is needed only for the turn-signal and hazard controls.
- PC and phone on the same Wi-Fi or LAN.

## Installation

1. Close the game.
2. Copy the `fh2-mods` folder from this package into the folder that contains your FH2 Mod Platform. Allow merging. This places `VirtualDashboard.dll` and `VirtualDashboard.ini` in your existing `fh2-mods` folder.
3. Start the game. The log (`fh2-mods\platform.log`) should show `[VirtualDashboard] 1.4.0 registered`.
4. Allow `azimuth` through Windows Firewall for **private** networks when prompted. If the prompt didn't appear, allow it manually in Windows Defender Firewall.
5. Find your PC's IPv4 address (`ipconfig`) and open `http://<PC-IP>:8080/` on your phone, for example `http://192.168.178.85:8080/`.

## Settings

- **In game:** Mods page, "Phone dashboard", turns the server on or off.
- **In game:** Mods page, "Dashboard layout" (Default / Alternative / Revuelto), "Dashboard nitro gauge" and "Dashboard turbo gauge", show or hide the gauges. Telemetry is always read.
- **`fh2-mods\VirtualDashboard.ini`:**
  ```
  [VirtualDashboard]
  Enabled=1
  Port=8080
  NitroGauge=1
  TurboGauge=1
  Layout=0
  ```
  Change `Port` if 8080 is taken. Restart the game after changing it.

## Notes

- The dashboard keeps working in menus. It shows "Waiting for a car in the game..." during loading screens, after a car swap, or when the connection is lost.
- Power and torque are approximations and may differ from the in-game Telemetry page. Boost is shown in bar.
- Turn signals and hazards: the game has none of its own; Extra Options adds them (hold D-pad left/right 0.5 s, up for hazards). The dashboard reads the resulting state, see "How the turn signals are read" below. The blinking itself is animated on the page.
- A JSON feed is available at `http://<PC-IP>:8080/api`. It now includes `nitro` (percent, -1 when FnFNitro is not loaded) and `abs`, `tcs`, `stm` (0/1).
- The server has no password and is meant for a trusted home network. Don't expose the port to the internet.

## How the turn signals are read (for modders)

- Flags: three bytes on the local player's car object, `car + 0x51BB` hazards, `+0x51BC` left, `+0x51BD` right (0 or 1). The car pointer is `hud + 0xE8` in the HUD hook at `game + 0x783E67`; it equals the player's car from the player manager (`*(game + 0x34ABA60)` -> `game + 0x21E0C0(mgr, 0)` -> `game + 0x2DA060(player, &car)`).
- Extra Options sets the bytes right before the game's car update (`game + 0x2A9400`) and the game clears them during it, so a once-per-frame read from a HUD hook sees zeros.
- VirtualDashboard runs a tight polling thread over the three bytes and keeps the time each was last seen set; a signal is on if seen within the last 250 ms. The published `ind` is 0 off, 1 left, 2 right, 3 hazards (or both sides).
- Reads are wrapped in `__try/__except`; `Indicators now N` is logged on every change.

## Updates

The FH2 Mod Platform checks https://github.com/suderek22/FH2_VirtualDashboard for new releases (needs Platform 2.7.1 or newer).

## Uninstall

Delete `VirtualDashboard.dll` and `VirtualDashboard.ini` from `fh2-mods`.

Created by suderek. [Visit FH2mods](https://fh2mods.com/)







