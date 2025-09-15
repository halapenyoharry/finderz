# Contributing to Finderz

## 🆘 Help Wanted!

Finderz is an early alpha project that needs contributors, especially if you:

### Are You One of These People?

- **Mac + Linux user** with both machines on the same network
- **NAS owner** who accesses it from both Mac and Linux
- **Developer** who's tired of .DS_Store files but sees their potential
- **File manager developer** with experience in:
  - GTK3/4 development
  - File system internals
  - Nemo/Nautilus/other file manager codebases
  - Building "file explorers" / "file browsers" / "file managers" (whatever you call them!)
- **DS_Store reverse engineer** who understands the binary format
- **Studio/Creative professional** managing mixed OS environments
- **Someone who's attempted this before** - please share your learnings!

### Current Status: VERY EARLY

**What works:**
- ✅ Forked from Nemo successfully
- ✅ Basic structure for DS_Store parser (headers/skeleton only)
- ✅ Architecture for sorting profiles system
- ✅ Can compile and run (it's just Nemo right now)

**What doesn't work yet:**
- ❌ DS_Store parsing (needs implementation)
- ❌ Custom sorting profiles (needs implementation) 
- ❌ Mac metadata integration (needs implementation)
- ❌ Pretty much all the "Finderz" features (it's basically still Nemo)

### Priority Help Needed

1. **DS_Store Binary Parser** 
   - The skeleton is in `src/finderz-ds-store.c`
   - Need someone who understands the buddy allocator format
   - Or experience with Python ds_store libraries we could port

2. **GTK3 UI Development**
   - Integrate DS_Store view preferences into Nemo's view system
   - Add profile switcher UI
   - Custom grouping range configurator

3. **Testing in Mixed Environments**
   - SMB/AFP shares between Mac and Linux
   - NAS access from both platforms
   - External drives that move between systems

4. **Build System**
   - Make it actually build easily on various distros
   - Package for different distributions
   - Maybe Flatpak?

### How to Contribute

1. **Test it and report issues** - Even "it doesn't build on [distro]" is helpful
2. **Pick a small piece** - Maybe implement one DS_Store record type
3. **Share knowledge** - Know how Finder actually works? Document it!
4. **Suggest better approaches** - Should we fork something else? Use FUSE?

### Development Setup

```bash
# You'll need GTK3 dev packages (varies by distro)
# On Ubuntu/Pop!_OS:
sudo apt install meson ninja-build libgtk-3-dev libglib2.0-dev

# Build
meson setup build
cd build && ninja

# Run without installing
./src/nemo --new-window
```

### Communication

- **Issues**: Use GitHub issues for bugs and feature discussions
- **Philosophy**: Keep the "z" spirit - useful > perfect
- **Goal**: Make Mac and Linux play nice in shared environments

### Code Style

- Follow Nemo's existing style (mostly)
- Prefix new Finderz-specific code with `finderz_`
- Document anything DS_Store related heavily (it's obscure)

### Legal

By contributing, you agree to license your contributions under GPL v2 (same as Nemo).

---

**Remember**: This is about solving a real problem - mixed Mac/Linux environments need better file management. If you're dealing with this problem too, your input is valuable even if you're not a C developer!