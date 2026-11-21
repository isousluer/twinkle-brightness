# Changelog

All notable changes to this project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/).

## [1.1.0] - 2026-11-21

### Added

- **Link displays** button in the panel, like Twinkle Tray's. While linked
  (the default), one slider sets all displays to the same level, and the
  hotkeys and the mouse wheel change all of them. While unlinked, each display
  has its own slider, the hotkeys change the display with the active window,
  and the mouse wheel changes the display under the cursor. The choice is
  remembered.

### Changed

- The on-screen indicator shows one row per display (name, bar and level) when
  a hotkey changes displays that are at different levels. Previously it showed
  only the first display's level.
- The schedule and idle dimming no longer poll. The schedule sets a timer for
  the next entry and also reacts to clock changes. Idle dimming checks for
  input every second only while the displays are dimmed.
- The mouse wheel hook runs on its own thread, so a busy panel or a slow
  display can't delay mouse input.

### Fixed

- If the session ended while the displays were dimmed for inactivity, they
  stayed dimmed. The previous levels are now restored at sign-out and
  shutdown, and at the next start if the mod was stopped in another way.
- After sleep, the latest schedule entry was applied even when "apply on
  startup and after sleep" was off.
- Scrolling over a tray icon flyout could change brightness when it
  overlapped the icon's position.
- A display that keeps failing no longer triggers a re-detection more than
  once every 10 seconds.

### Removed

- The "Show an All displays slider" and mouse wheel "Displays to change"
  settings. The link button replaces both.

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

[1.1.0]: https://github.com/isousluer/twinkle-brightness/releases/tag/v1.1.0
[1.0.0]: https://github.com/isousluer/twinkle-brightness/releases/tag/v1.0.0
