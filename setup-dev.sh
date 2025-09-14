#!/bin/bash

# Finderz Development Environment Setup Script

echo "Setting up Finderz development environment..."

# Check for required dependencies
echo "Checking dependencies..."

# Required packages for building Nemo/Finderz
DEPS="meson ninja-build libgtk-3-dev libglib2.0-dev libxml2-dev \
      libexif-dev libexempi-dev libnotify-dev libcinnamon-desktop-dev \
      libxapp-dev python3-dev python3-setuptools gir1.2-gtk-3.0 \
      gobject-introspection libgirepository1.0-dev"

# Check if running on Pop!_OS/Ubuntu
if [ -f /etc/os-release ]; then
    . /etc/os-release
    if [[ "$ID" == "ubuntu" ]] || [[ "$ID" == "pop" ]]; then
        echo "Detected $NAME"
    else
        echo "Warning: This script is designed for Pop!_OS/Ubuntu"
    fi
fi

# Install dependencies
echo "Installing build dependencies..."
echo "This will require sudo access:"
sudo apt update
sudo apt install -y $DEPS

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    meson setup build --prefix=/usr/local
else
    echo "Build directory already exists"
fi

# Create initial configuration
echo "Creating default Finderz configuration..."
mkdir -p ~/.config/finderz

cat > ~/.config/finderz/profiles.json << 'EOF'
{
  "version": "1.0",
  "profiles": [
    {
      "name": "Default",
      "description": "Standard sorting by name",
      "is_default": true,
      "sort_levels": [
        {"criteria": "name", "direction": "ascending"}
      ],
      "grouping_type": "none",
      "folders_first": true,
      "show_hidden_files": false,
      "case_sensitive": false,
      "natural_sorting": true
    },
    {
      "name": "By Date",
      "description": "Always sort by date modified, then by name",
      "is_default": false,
      "sort_levels": [
        {"criteria": "date_modified", "direction": "descending"},
        {"criteria": "name", "direction": "ascending"}
      ],
      "grouping_type": "date_range",
      "folders_first": false,
      "show_hidden_files": false,
      "case_sensitive": false,
      "natural_sorting": true
    },
    {
      "name": "Developer",
      "description": "Show hidden files, group by extension",
      "is_default": false,
      "sort_levels": [
        {"criteria": "extension", "direction": "ascending"},
        {"criteria": "name", "direction": "ascending"}
      ],
      "grouping_type": "extension",
      "folders_first": true,
      "show_hidden_files": true,
      "case_sensitive": true,
      "natural_sorting": true
    }
  ],
  "active_profile": "Default",
  "directory_profiles": {}
}
EOF

echo "Configuration created at ~/.config/finderz/profiles.json"

# Create a simple build script
cat > build.sh << 'EOF'
#!/bin/bash
cd build && ninja
EOF
chmod +x build.sh

# Create a run script for testing
cat > run-finderz.sh << 'EOF'
#!/bin/bash
# Run Finderz from build directory for testing
export G_MESSAGES_DEBUG=all
export FINDERZ_DEBUG=1
./build/src/nemo --new-window
EOF
chmod +x run-finderz.sh

echo ""
echo "Setup complete! Next steps:"
echo "1. Build Finderz:     ./build.sh"
echo "2. Test Finderz:      ./run-finderz.sh"
echo "3. Install (optional): cd build && sudo ninja install"
echo ""
echo "For development, you can run Finderz from the build directory"
echo "without installing system-wide."
EOF