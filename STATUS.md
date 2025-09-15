# Finderz Development Status

## Current Functionality: 0% Finderz, 100% Nemo

**Honest status**: Right now, Finderz is just Nemo with some aspirational header files. If you build and run it, you get Nemo. But that's the starting point!

## What We Have

### ✅ Foundation (Working)
- Forked Nemo codebase 
- Builds and runs on Pop!_OS 22.04
- All standard Nemo features work

### 🏗️ Architecture (Designed, Not Implemented)

#### DS_Store Parser (`src/finderz-ds-store.{h,c}`)
- **Status**: Headers and skeleton only
- **Needs**: Actual binary parsing implementation
- **Challenge**: DS_Store uses a proprietary B-tree "Buddy allocator" format
- **Options**: 
  - Port from Python `ds_store` library
  - Use `dsstore` C implementation if we can find one
  - Reverse engineer from scratch (hard!)

#### Sorting Profiles (`src/finderz-sorting-profiles.h`)  
- **Status**: Header file with data structures defined
- **Needs**: Implementation and UI integration
- **Challenge**: Hooking into Nemo's existing sort system

## Terminology Clarification

This type of software is called:
- **File Manager** (Linux term)
- **File Explorer** (Windows term)  
- **Finder** (macOS term)
- **File Browser** (generic term)

Finderz is technically a "file manager" in Linux parlance.

## Immediate Next Steps

1. **Get DS_Store parsing working at all**
   - Even just reading the view style would be a win
   - Test with real .DS_Store files from Mac

2. **Hook one feature into Nemo**
   - Maybe just respect icon view vs list view from DS_Store
   - Proves the integration can work

3. **Find collaborators** 
   - Someone who's fought with DS_Store before
   - GTK3 developers who know Nemo/Nautilus internals
   - People with Mac+Linux setups to test

## Build & Test

```bash
# Install dependencies (Pop!_OS/Ubuntu)
sudo apt install meson ninja-build libgtk-3-dev libglib2.0-dev \
                 libxml2-dev libexif-dev libexempi-dev libnotify-dev \
                 libcinnamon-desktop-dev libxapp-dev

# Build
meson setup build
cd build && ninja

# Run (you'll see regular Nemo)
./src/nemo

# To see debug output
G_MESSAGES_DEBUG=all ./src/nemo
```

## Known Issues

- It's literally just Nemo right now
- No DS_Store features work yet
- The dreams exceed the implementation by ~100%

## Help Wanted

If you've ever:
- Parsed DS_Store files
- Written a file manager
- Worked with GTK3/GLib
- Wanted Finder features on Linux
- Suffered through Mac/Linux file sharing

...please contribute! Even advice on approach would help.