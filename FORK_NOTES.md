# Finderz Fork Notes

## Base: Nemo File Manager

Finderz is forked from Nemo (Linux Mint's file manager), which itself is a fork of Nautilus (GNOME Files).

### Why Nemo?
- Already has features that Nautilus removed (dual pane, tree view)
- Excellent plugin/extension system (Actions)
- Clean GTK3 codebase
- Active development and community
- Good starting point for macOS-like features

## Key Modifications from Nemo

### 1. DS_Store Support
- **Files**: `src/finderz-ds-store.{h,c}`
- **Purpose**: Parse and utilize macOS .DS_Store files from network volumes
- **Features**:
  - Read view preferences (icon/list/column/gallery)
  - Import icon positions and sizes
  - Preserve sort preferences
  - Cache parsed data for performance

### 2. Advanced Sorting Profiles
- **Files**: `src/finderz-sorting-profiles.{h,c}`
- **Purpose**: Multiple customizable sorting configurations
- **Features**:
  - Multi-level sorting (primary, secondary, tertiary)
  - Custom grouping ranges (not limited to Finder's presets)
  - Per-directory profile memory
  - Quick profile switching

### 3. Enhanced Grouping System
- **Location**: Integrated into sorting profiles
- **Features**:
  - Custom date ranges (beyond Today/Yesterday/This Week)
  - Custom size ranges (user-defined thresholds)
  - Group by tags, color labels, extensions
  - Collapsible groups

### 4. View Profiles
- **Purpose**: 3-4 different view/behavior profiles
- **Examples**:
  - "Work Mode": Sort by date modified, group by project
  - "Media Mode": Sort by type, show large thumbnails
  - "Clean Mode": Hide all dotfiles, sort alphabetically
  - "Developer Mode": Show hidden files, sort by extension

## Build System Changes

- Using Meson build system (inherited from Nemo)
- Added Finderz-specific configuration options
- Custom branding and icons

## Upstream Tracking

- Upstream remote: https://github.com/linuxmint/nemo.git
- Strategy: Periodic merges from upstream master
- Custom changes isolated in finderz-* files where possible

## Development Guidelines

1. **Preserve Nemo compatibility**: Keep core Nemo functionality intact
2. **Modular additions**: New features in separate files when possible
3. **Configuration first**: Make features optional/configurable
4. **Performance conscious**: Cache DS_Store data, lazy loading
5. **Cross-platform aware**: Handle missing DS_Store gracefully

## Testing Priorities

1. Network volume DS_Store reading
2. Profile switching performance
3. Large directory handling
4. Memory usage with cached DS_Store data
5. Compatibility with standard Nemo extensions

## Future Considerations

- Consider contributing useful features back to Nemo
- Evaluate switching to GTK4 when Nemo migrates
- Possible Flatpak packaging for easy distribution
- Integration with cloud storage providers