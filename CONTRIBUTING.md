# Contributing

Thanks for your interest in improving Twinkle Brightness! Bug reports, display
compatibility reports and pull requests are all welcome.

## Reporting a problem

Please use the [issue templates](https://github.com/isousluer/twinkle-brightness/issues/new/choose)
and include:

- Your Windows version, for example Windows 11 24H2.
- Your Windhawk version.
- The output of **Copy display info** from the tray menu.
- How each display is connected (HDMI, DisplayPort, USB-C, dock, adapter...).
- The mod's log. In Windhawk, open the mod, go to the **Advanced** tab and set
  logging to verbose, reproduce the problem, then copy the log.

## Project layout

```text
twinkle-brightness.wh.cpp   The mod. This single file is what Windhawk runs.
tools/test-harness/         Builds and runs the mod outside Windhawk.
assets/                     Screenshots used by the README and the mod page.
docs/                       Maintainer documentation.
```

The source file is organized in sections:

1. **Settings**: parsing Windhawk settings into a `Settings` struct.
2. **Hardware worker** (`namespace hw`): display enumeration, DDC/CI and WMI.
   It runs on its own thread, so slow hardware calls never block the UI.
3. **UI state**: the display list as the UI sees it, fades and normalization.
4. **Tray icon, panel and on-screen indicator**: Win32 + GDI+ drawing.
5. **Hotkeys, mouse wheel, schedule and idle dimming**.
6. **Tool mod boilerplate**: the standard Windhawk code that runs the mod in a
   dedicated process. Leave this section unchanged.

## Building and testing

You need [Windhawk](https://windhawk.net) installed. The scripts use its bundled
clang compiler.

```powershell
.\tools\test-harness\build.ps1          # compile like Windhawk does (-Wall)
.\tools\test-harness\build.ps1 -Run     # also run the test harness
```

The harness detects your displays, opens the panel, renders the on-screen
indicator to `tools\test-harness\out\*.png` and exits. To try the real thing,
paste the file into a new mod in Windhawk's editor and click **Compile Mod**.

Before opening a pull request:

- Make sure the build has no warnings.
- Test on Windows 10 and/or 11, with at least one external monitor if your
  change touches the hardware code.
- Update `CHANGELOG.md`, and bump `@version` in the mod header for
  user-visible changes.
- Keep the code style: 4-space indentation, 80 columns, and
  [Chromium-style](https://chromium.googlesource.com/chromium/src/+/main/styleguide/c++/c++.md)
  formatting (`clang-format --style=Chromium`).
