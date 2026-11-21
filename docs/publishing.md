# Publishing a release

This project is distributed in two places:

1. **This GitHub repository**: source, documentation and releases.
2. **The official Windhawk mods repository**
   ([ramensoftware/windhawk-mods](https://github.com/ramensoftware/windhawk-mods)),
   where Windhawk users install it from the **Explore** tab.

## 1. Prepare the release here

1. Update `@version` in the header of `twinkle-brightness.wh.cpp`.
2. Add an entry to `CHANGELOG.md`.
3. Run `.\tools\test-harness\build.ps1 -Run` and make sure there are no
   warnings.
4. Test the mod in Windhawk: paste it into a new mod, compile it and try the
   features you changed.
5. Commit, tag and push:

   ```powershell
   git commit -am "Release v1.0.0"
   ```

   ```powershell
   git tag v1.0.0
   ```

   ```powershell
   git push --follow-tags
   ```

6. Create a GitHub release from the tag and paste the changelog entry into it:

   ```powershell
   gh release create v1.0.0 --title "v1.0.0" --notes-file release-notes.md
   ```

## 2. Submit to the Windhawk mods repository

Windhawk mods are submitted as pull requests. Each pull request adds or
updates a single file in the `mods/` folder.

1. Fork [ramensoftware/windhawk-mods](https://github.com/ramensoftware/windhawk-mods).
2. Copy `twinkle-brightness.wh.cpp` to `mods/twinkle-brightness.wh.cpp` in your
   fork. The file name must match the `@id` in the header.
3. Open a pull request titled `Twinkle Brightness v1.0.0`. In the description:
   - Explain what the mod does and link this repository.
   - Explain how it differs from existing mods. For example,
     `per-monitor-brightness` adds sliders to Windows 11 Quick Settings, while
     this mod is a standalone tray panel that works on Windows 10 and 11 and
     adds hotkeys, a schedule and idle dimming.
   - Attach a screenshot.
4. Review happens in two stages, which you drive with comments on the pull
   request
   ([details](https://github.com/ramensoftware/windhawk/wiki/Mod-submission-review-process)):
   1. Comment `/ai-review` and wait for the AI review. This can take 20-30
      minutes or more. Push fixes for the findings that hold up, and comment
      `/ai-review` again if needed.
   2. Comment `/ready-for-reviewer` to hand the pull request to a human
      reviewer. For each AI finding you did not act on, add a sentence
      explaining why.
   3. Address the human reviewer's comments by pushing to the same branch.
5. Once merged, the mod appears in Windhawk's **Explore** tab within a short
   time.

For later updates, repeat the steps above with the new version: copy the file
over the existing one and open a new pull request titled
`Twinkle Brightness v1.1.0`.

### Checklist from the Windhawk mod guidelines

- [ ] `@id` matches the file name, and `@version` was bumped.
- [ ] `@github` points to the author's GitHub profile.
- [ ] The readme and settings are in English.
- [ ] The mod compiles without warnings.
- [ ] No external downloads or telemetry.
- [ ] The tool mod boilerplate section is unchanged.
