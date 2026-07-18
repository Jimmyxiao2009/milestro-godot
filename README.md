# Milestro for Godot

GPU-friendly **Skia text rendering** for [Godot 4](https://godotengine.org/), ported from [MorizeroDev/Milestro](https://github.com/MorizeroDev/Milestro) (Unity native stack).

This repository provides a thin **GDExtension** over the real Milestro C plugin API (not a reimplementation of Skia).

## Features (v0.1)

- Load prebuilt native `libMilestro` on **macOS (x86_64)**
- `Milestro` class from GDScript:
  - `get_version()` → real native `MilestroGetVersion` (matches `1.0.0`)
  - `register_font_from_file(path)`
  - `render_text` / `render_text_metrics` → CPU raster → Godot `Image`
- Mixed Latin + CJK sample verified headless on Godot **4.7.stable**

## Quick start

1. Download the **Release** asset `milestro-godot-macos-x86_64-vX.Y.Z.zip`, **or** clone this repo (prebuilt dylibs are included under `addons/milestro/bin/`).
2. Open this folder in **Godot 4.7+** (`project.godot`).
3. Run `scripts/run_verify.sh`, or open `verify_main.tscn` directly — the output should contain `GODOT_VERIFY_OK`.

Copy `addons/milestro` into your own project’s `addons/` to use the plugin elsewhere.

For ready-made Godot controls, add `addons/milestro/MilestroTextLabel.gd` or
`addons/milestro/MilestroInputBoxControl.gd` to a scene. The first renders rich
text into a `TextureRect`; the second connects the native InputBox editing model
to Godot keyboard and mouse events.

### GDScript

```gdscript
var m = Milestro.new()
print(m.get_version())  # { major, minor, patch, ok, code }

m.register_font_from_file("/path/to/SourceHanSans.otf")
var metrics = m.render_text_metrics("Hello 你好", "Source Han Sans VF", 36.0, 512, 256)
if metrics.ok:
    var img: Image = metrics.image
    # metrics.height, metrics.longest_line, metrics.non_zero_pixels
```

## Layout

```
addons/milestro/     # .gdextension + prebuilt binaries + ICU data
src/                 # GDExtension C++ (godot-cpp)
native_smoke/        # CLI smoke test against the same C API
scripts/             # verify helpers
project.godot        # demo / smoke project
```

## Build from source (macOS)

Requirements: CMake ≥ 3.26, Ninja, Xcode CLT, Godot 4.7, a built [Milestro](https://github.com/MorizeroDev/Milestro) `libMilestro.dylib`.

```bash
# 1) Build native Milestro (upstream tree)
cmake -S /path/to/Milestro -B /path/to/Milestro/cmake-build-relwithdebinfo -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build /path/to/Milestro/cmake-build-relwithdebinfo --target Milestro

# 2) Build this GDExtension. CMake fetches godot-cpp automatically unless
#    GODOTCPP_DIR points at a local checkout. Set MILESTRO_ROOT / MILESTRO_LIB_DIR.
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DMILESTRO_ROOT=/path/to/Milestro \
  -DMILESTRO_LIB_DIR=/path/to/Milestro/cmake-build-relwithdebinfo/lib
cmake --build build --target milestro_godot

# If GitHub is unavailable, download godot-cpp separately and pass:
#   -DGODOTCPP_DIR=/path/to/godot-cpp
```

## Platform status

| Platform | Status |
|----------|--------|
| macOS x86_64 | Local + CI (`bin/macos-x86_64/`) |
| macOS arm64 | CI (`bin/macos-arm64/`) |
| Windows x86_64 | CI (`bin/windows-x86_64/`) |
| Linux x86_64 | CI (`bin/linux-x86_64/`) |
| Mobile | Not yet |

Binaries are produced by GitHub Actions: **Actions → Build multiplatform GDExtension**.  
Merged zip: release tag `v0.2.0-multiplatform` (or workflow artifact `milestro-godot-addon-multiplatform`).

### Layout

```
addons/milestro/bin/
  windows-x86_64/   milestro.windows.template_debug.x86_64.dll + libMilestro.dll + icudtl.dat
  linux-x86_64/     libmilestro.linux.template_debug.x86_64.so + libMilestro.so + icudtl.dat
  macos-x86_64/     libmilestro.macos.template_debug.x86_64.dylib + libMilestro.dylib + icudtl.dat
  macos-arm64/      ...
```

## Credits

- Native engine: [MorizeroDev/Milestro](https://github.com/MorizeroDev/Milestro) (MIT)
- Bindings: [godotengine/godot-cpp](https://github.com/godotengine/godot-cpp)

## License

MIT — see [LICENSE](LICENSE).
