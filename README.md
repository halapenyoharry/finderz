# Finderz

[![Version](https://img.shields.io/badge/version-0.0.1--alpha-orange)](https://github.com/halapenyoharry/finderz/releases)
[![License](https://img.shields.io/badge/license-GPL%20v2-blue)](LICENSE)
[![Status](https://img.shields.io/badge/status-early%20alpha-red)](https://github.com/halapenyoharry/finderz)

⚠️ **EARLY ALPHA**: This is experimental software. Expect bugs, incomplete features, and breaking changes.

A better file explorer inspired by macOS Finder, with enhanced sorting, grouping, and network volume support.

## Why Finderz?

The 'z' means we're not trying to be perfect - we're trying to be *useful*. This is a filesystem manager built for the reality of mixed Mac/Linux studios where you need both platforms to play nice together. 

### The Philosophy

The ultimate creative studio runs both Mac and Linux boxes on the same LAN:
- **Macs** for creative tools, audio/video production, design work
- **Linux** boxes for development, servers, compute, and actual file management
- **Shared storage** that somehow needs to work well for everyone

Finderz bridges this gap by actually reading and respecting the `.DS_Store` files Macs leave everywhere, turning what's usually considered "pollution" into useful metadata. Your Mac colleagues set up a careful folder view? Finderz will respect it. But unlike Finder, you can also override it with better sorting rules when needed.

## Vision

Finderz aims to be a superior file management experience that combines the visual elegance of macOS Finder with powerful customization options and cross-platform compatibility. Built for Linux (Pop!_OS 22.04) with the ability to leverage macOS metadata when available. A little janky? Maybe. More useful than Finder? Definitely.

## Key Features

### 🗂️ Smart .DS_Store Integration
- Read and utilize .DS_Store metadata from macOS volumes
- Show Mac metadata as columns in list view:
  - Icon positions from Finder
  - View preferences (Icon/List/Column/Gallery)
  - Color labels
  - Sort preferences
- Preserve macOS folder view settings across network shares
- Seamless integration with mixed macOS/Linux environments

### 🔄 Advanced Sorting Profiles
- Create multiple sorting presets (e.g., "Always by date, then name")
- Quick-switch between sorting configurations
- Per-directory sorting memory

### 📊 Flexible Grouping System
- Customizable grouping ranges (not limited to Finder's presets)
- Multi-level grouping criteria
- User-defined grouping templates

### ⚙️ Customization Sets
- 3-4 different view/behavior profiles
- Quick profile switching based on context
- Per-user and per-directory settings persistence

## Technical Stack

- **Target OS**: Pop!_OS 22.04 (Ubuntu-based)
- **Base**: Fork of existing Linux file manager (TBD)
- **Language**: (Based on forked project)
- **UI Framework**: (Based on forked project)

## Project Structure

```
finderz/
├── README.md           # This file
├── docs/              # Documentation
├── src/               # Source code
├── config/            # Configuration templates
└── scripts/           # Build and utility scripts
```

## Use Cases

### Mixed Studio Environment
- **SMB/AFP shares** from Mac servers that are full of .DS_Store files
- **NAS devices** accessed by both Mac and Linux machines  
- **External drives** formatted as exFAT that move between systems
- **Git repositories** where Mac devs accidentally committed .DS_Store files (now they're features!)

### The "Mac Handoff"
When your Mac colleague sends you a carefully organized project folder, Finderz will:
- Respect their icon positions and view settings
- But still let you sort by date modified when you need to find the latest files
- Show you their color labels and tags
- Let you add your own Linux-side organization on top

## Development Roadmap

### Phase 1: Foundation
- [x] Fork Nemo as base
- [x] Set up development environment
- [ ] Basic .DS_Store parser implementation

### Phase 2: Core Features
- [ ] Implement sorting profiles
- [ ] Create grouping system
- [ ] Add settings management

### Phase 3: Enhancement
- [ ] Network volume support
- [ ] Performance optimization
- [ ] UI/UX refinements

### Phase 4: Polish
- [ ] User testing
- [ ] Documentation
- [ ] Package for distribution

## Getting Started

### Prerequisites
- Pop!_OS 22.04 or compatible Ubuntu-based distribution
- Development tools (gcc, make, etc.)
- Git

### Installation
```bash
# Clone the repository
git clone https://github.com/halapenyoharry/finderz.git
cd finderz

# Build instructions will depend on the forked base
# TBD after selection
```

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting PRs.

## License

GPL v2 - Same as Nemo, which Finderz is forked from. See [LICENSE](LICENSE) file for details.

## Acknowledgments

- macOS Finder for design inspiration
- The open-source file manager community
- .DS_Store reverse engineering efforts

---

*Finderz - Where better file management begins*