# Changelog

All notable changes to this project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/).

## [1.1.0] - 2026-11-21

### Added

- "All displays" slider behavior setting. The new default, *relative*, moves
  every display by the same amount and keeps their differences. *Absolute*
  sets every display to the same level, as in 1.0.0.

### Changed

- The on-screen indicator shows one row per display (name, bar and level) when
  a hotkey or the mouse wheel changes displays that are at different levels.
  Previously it showed only the first display's level.

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
