# Changelog

All notable changes to this project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/).

## [Unreleased] - 1.3.0

### Added

- "Cycle through levels" hotkey action: each press goes to the next level of
  a list such as `20, 50, 100`, and after the last one back to the first.
  It starts from the displays' current level, so it continues sensibly after
  the level was changed elsewhere. Combine it with the hotkey's display
  setting for one hotkey per monitor, for example on Stream Deck buttons. (#14)

### Fixed

- A hotkey step that waits for fresh monitor levels no longer runs early when
  a monitor that didn't answer at startup is found, before the other
  monitors were read again.

## [1.2.0] - 2026-11-25

### Added

- Type an exact level: double-click a display's level in the panel,
  or press F2 or Enter on the focused row. Enter applies, Esc cancels. (#10)
- The tray icon shows the brightness level: the sun's rays light up like a
  gauge. It can be turned off in the settings. (#9)
- Setting for the position of the on-screen indicator: the bottom or the top
  of the screen, in the center or a corner. (#11)
- Tooltips for the buttons at the bottom of the panel. (#2)
- Turkish translation of the panel, the menus and the settings. The mod
  follows the Windows display language. (#3)
- Brightness changes made outside the mod are picked up. Laptop screens
  report them right away. Monitors are read again before a hotkey step if
  their levels are more than a few seconds old. (#5)

### Fixed

- Some monitors (for example ViewSonic VX2476) showed "Brightness control is
  not available" until the displays were refreshed by hand. DDC/CI reads are
  now retried with longer waits, and a monitor that doesn't answer is
  detected again up to three times in the first minute. (#13)
- Scrolling with a touchpad in the panel changed the brightness much too
  fast. It now steps once per mouse wheel notch. (#12)

## [1.1.0] - 2026-11-25

### Added

- **Link displays** button in the panel, like Twinkle Tray's. While linked
  (the default), one slider sets all displays to the same level, and the
  hotkeys change all of them. While unlinked, each display has its own slider
  and the hotkeys change the display with the active window. The choice is
  remembered.

### Changed

- The on-screen indicator shows one row per display (name, bar and level) when
  a hotkey changes displays that are at different levels. Previously it showed
  only the first display's level.
- The on-screen indicator shows display names in bold, like the panel.
- The schedule and idle dimming no longer poll. The schedule sets a timer for
  the next entry and also reacts to clock changes. Idle dimming checks for
  input every second only while the displays are dimmed.

### Fixed

- If the session ended while the displays were dimmed for inactivity, they
  stayed dimmed. The previous levels are now restored at sign-out and
  shutdown, and at the next start if the mod was stopped in another way.
- After sleep, the latest schedule entry was applied even when "apply on
  startup and after sleep" was off.
- A display that keeps failing no longer triggers a re-detection more than
  once every 10 seconds.

### Removed

- Mouse wheel control over the tray icon or the taskbar. It always reached the
  display with the taskbar, and removing it also removes the system-wide
  mouse hook. Scrolling over the panel's sliders still works.
- The "Show an All displays slider" setting. The link button replaces it.

## [1.0.0] - 2026-11-21

### Added

- Tray icon with a brightness panel: one slider per display and an optional
  "All displays" slider. The panel follows the Windows light/dark theme and
  accent color.
- External monitor support through DDC/CI, and laptop/tablet screen support
  through WMI.
- Configurable hotkeys: increase, decrease or set brightness, turn the displays
  off, and open the panel.
- On-screen brightness indicator.
- Mouse wheel control over the tray icon or the whole taskbar, with an optional
  modifier key.
- Time-of-day brightness schedule with smooth transitions.
- Idle dimming, with an option to skip it while full-screen apps run.
- Per-display options: custom name, min/max brightness (normalization) and
  hiding.
- Tray menu: refresh displays, turn displays off, pause automatic brightness and
  copy display info.
- Automatic re-detection of displays after sleep and display changes.

[Unreleased]: https://github.com/isousluer/twinkle-brightness/compare/v1.2.0...HEAD
[1.2.0]: https://github.com/isousluer/twinkle-brightness/releases/tag/v1.2.0
[1.1.0]: https://github.com/isousluer/twinkle-brightness/releases/tag/v1.1.0
[1.0.0]: https://github.com/isousluer/twinkle-brightness/releases/tag/v1.0.0
