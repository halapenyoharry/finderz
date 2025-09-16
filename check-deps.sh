#!/bin/bash

# Check dependencies for Finderz build

echo "=== Checking Finderz Dependencies ==="
echo ""

# Function to check if package is installed
check_package() {
    if dpkg -l | grep -q "^ii  $1"; then
        echo "✓ $1 installed"
        return 0
    else
        echo "✗ $1 NOT installed"
        return 1
    fi
}

# Required packages
echo "Core build tools:"
check_package meson
check_package ninja-build

echo ""
echo "GTK and GLib development:"
check_package libgtk-3-dev
check_package libglib2.0-dev
check_package gobject-introspection
check_package libgirepository1.0-dev

echo ""
echo "Nemo/Cinnamon specific:"
check_package libcinnamon-desktop-dev
check_package libxapp-dev

echo ""
echo "Other libraries:"
check_package libxml2-dev
check_package libexif-dev
check_package libexempi-dev
check_package libnotify-dev

echo ""
echo "Python (for tools):"
which python3 > /dev/null && echo "✓ python3 found" || echo "✗ python3 NOT found"
python3 -c "import gi" 2>/dev/null && echo "✓ python3-gi installed" || echo "✗ python3-gi NOT installed"

echo ""
echo "To install missing packages, run:"
echo "  bash install-deps.sh"