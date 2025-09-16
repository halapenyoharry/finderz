# Finderz Development TODO

## 🎯 Current Goal
Transform Finderz into the ultimate metadata-aware file manager that makes ALL metadata sortable, viewable, and indelible.

## Phase 1: Foundation ✅ In Progress
- [x] Fork Nemo base
- [x] Basic DS_Store detection
- [x] Column provider architecture
- [ ] **Complete DS_Store binary parser**
  - [ ] Parse B-tree structure
  - [ ] Extract icon positions
  - [ ] Read view preferences
  - [ ] Parse color labels
- [ ] **EXIF/XMP extraction**
  - [ ] Integrate libexiv2
  - [ ] Extract camera metadata
  - [ ] Read AI generation data
  - [ ] Parse XMP sidecars
- [ ] **Extended attributes handler**
  - [ ] Read Linux xattrs
  - [ ] Write metadata to user.finderz.*
  - [ ] Sync with file operations
- [ ] **Dynamic column system**
  - [ ] Auto-discover metadata fields
  - [ ] Create columns on-the-fly
  - [ ] Save column preferences

## Phase 2: Core Features
- [ ] **AI Metadata Extraction**
  - [ ] PNG tEXt/iTXt chunks
  - [ ] WebP metadata
  - [ ] ComfyUI workflow extraction
  - [ ] Stable Diffusion parameters
- [ ] **Multi-level sorting**
  - [ ] Primary/secondary/tertiary sort
  - [ ] Save sorting profiles
  - [ ] Quick-switch UI
- [ ] **Rating system**
  - [ ] 1-5 star ratings
  - [ ] Instant write to EXIF
  - [ ] Keyboard shortcuts (1-5 keys)
- [ ] **Metadata filtering**
  - [ ] Query language parser
  - [ ] Live filter results
  - [ ] Save filter presets

## Phase 3: System Integration
- [ ] **Tracker3 integration**
  - [ ] Export to SPARQL
  - [ ] Custom ontologies
  - [ ] System-wide search
- [ ] **XDG compliance**
  - [ ] Proper cache locations
  - [ ] Thumbnail spec compliance
  - [ ] D-Bus service
- [ ] **Sidecar support**
  - [ ] .xmp file monitoring
  - [ ] .metadata JSON files
  - [ ] Custom sidecar formats
- [ ] **Batch operations**
  - [ ] Multi-select metadata edit
  - [ ] Bulk rating
  - [ ] Metadata copy/paste

## Phase 4: Polish
- [ ] **Smart grouping**
  - [ ] Group by metadata patterns
  - [ ] Collapsible groups
  - [ ] Custom grouping rules
- [ ] **Column presets**
  - [ ] Photo preset
  - [ ] AI image preset
  - [ ] Document preset
  - [ ] Auto-detect best preset
- [ ] **Performance**
  - [ ] Background indexing
  - [ ] Lazy metadata loading
  - [ ] Cache optimization
- [ ] **Mac compatibility**
  - [ ] Complete DS_Store support
  - [ ] Resource fork handling
  - [ ] Bidirectional label sync

## 🐛 Known Issues
- DS_Store parser only validates headers, doesn't parse data
- Column provider registered but not integrated with views
- No actual metadata extraction yet

## 📝 Notes
- Following zero-friction philosophy from AI image metadata project
- All metadata should be indelible (stored in file + xattr + index)
- Must maintain compatibility with Tracker3 for system search
- Performance is critical - use lazy loading everywhere

## 🔗 Related Projects
- `/home/harold/Projects/ai-image-metadata-manager/` - AI metadata standards
- `/home/harold/ai/frameworks/AI_IMAGE_METADATA_STANDARDS.md` - Standards doc

## 🎉 Milestones
- [ ] First successful EXIF column display
- [ ] First AI prompt shown in list view
- [ ] First rating saved to file
- [ ] First system-wide search hit
- [ ] First Mac user says "wow, this is better than Finder"