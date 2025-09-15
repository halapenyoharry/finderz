# Building Finderz

## Quick Start

1. **Install dependencies** (requires sudo):
   ```bash
   bash install-deps.sh
   ```

2. **Build Finderz**:
   ```bash
   meson setup build
   cd build && ninja
   ```

3. **Run Finderz** (without installing):
   ```bash
   G_MESSAGES_DEBUG=all ./build/src/nemo
   ```

## What's Working Now

After building, Finderz can:
- ✅ Detect .DS_Store files in directories
- ✅ Validate DS_Store file headers
- ✅ Log when Mac metadata is found (visible with G_MESSAGES_DEBUG=all)

## Testing DS_Store Support

1. **Test with Python tool**:
   ```bash
   # Install Python ds_store library
   pip3 install --user ds-store
   
   # Run test tool
   python3 test-ds-store.py
   ```

2. **Create a test DS_Store**:
   ```bash
   python3 test-ds-store.py
   # Choose option 3 to create test.DS_Store
   ```

3. **Navigate to folder with DS_Store**:
   - Run Finderz with debug output
   - Navigate to a folder containing .DS_Store
   - Check terminal for "FINDERZ:" messages

## Build Troubleshooting

### Missing meson/ninja

If meson or ninja aren't installed:
```bash
sudo apt update
sudo apt install meson ninja-build
```

### Missing GTK/GLib headers

```bash
sudo apt install libgtk-3-dev libglib2.0-dev
```

### Missing Cinnamon libraries

```bash
sudo apt install libcinnamon-desktop-dev libxapp-dev
```

## Current Status

- **DS_Store Detection**: ✅ Working
- **Header Validation**: ✅ Working  
- **Full Parsing**: 🚧 In progress
- **UI Integration**: 🚧 Next step

## Development

To see Finderz-specific debug output:
```bash
G_MESSAGES_DEBUG=all ./build/src/nemo 2>&1 | grep FINDERZ
```

This will show:
- When DS_Store files are detected
- File validation results
- Parsing attempts

## Next Steps

The foundation is in place. Next priorities:
1. Full DS_Store record parsing
2. Status bar indicator for Mac metadata
3. Respect Mac view preferences