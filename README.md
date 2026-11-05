# Auto Decorator AI — Android / Geode

Android-targeted Geode port of the supplied Auto Decorator AI source. The original procedural decoration engine, geometry analysis, pattern detection, and preset definitions are retained.

## Build on GitHub

1. Create an empty GitHub repository (or use your existing repository).
2. Upload **the contents of this folder** to the repository root. Do not upload the outer ZIP as a file.
3. Open the repository's **Actions** tab.
4. Select **Build Auto Decorator AI Android** and press **Run workflow** (or push a commit to `main` / `master`).
5. When the workflow finishes, open its run and download the artifact named `AutoDecoratorAI-Android`.
6. Extract the artifact and install the produced `.geode` package using Geode on Android.

The workflow requests Android64 and Android32 builds. A successful workflow is required before an installable `.geode` package exists.

## Current port changes

- Replaced the Windows-only DLL CMake setup with Geode SDK project setup.
- Added GitHub Actions Android build workflow.
- Removed the duplicate `hideOverlay()` definition (the original source could not compile as written).
- Removed blocking `sleep_for` from the editor mutation loop; sleeping on the game thread can freeze the editor.
- Kept the original AI engine, analyzer, pattern detector, presets, and UI source.

## Important status / known limitations

This is an Android build-preparation port, **not a verified working Android binary**. It must still be compiled against the exact Geode SDK / Geometry Dash bindings and tested in-game. In particular:

- The current UI obtains objects through the editor selection (`getSelectedObjects()`). Full-layout mode has not yet been implemented or verified.
- The decoration pool contains numeric object IDs from the original project; these IDs need in-game validation to ensure they resolve to suitable decoration objects on the target GD version.
- The editor object insertion and undo behavior must be tested on-device.
- The original UI has not yet been fully redesigned for narrow mobile aspect ratios.

Do not treat a green build alone as proof that generated decorations are visually correct or that full-layout decoration works.

## Target

Geometry Dash 2.2081 / Geode Android. The repository uses the Geode SDK supplied by `geode-sdk/build-geode-mod`.
