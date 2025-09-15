#!/bin/bash

# Finderz Build Dependencies Installation Script
# Run this with: bash install-deps.sh

echo "=== Finderz Build Dependencies Installer ==="
echo "This script will install the required packages to build Finderz"
echo ""
echo "Required packages:"
echo "- meson (build system)"
echo "- ninja-build (build tool)"
echo "- libgtk-3-dev (GTK3 development)"
echo "- libglib2.0-dev (GLib development)"
echo "- libxml2-dev (XML parsing)"
echo "- libexif-dev (EXIF data)"
echo "- libexempi-dev (XMP metadata)"
echo "- libnotify-dev (notifications)"
echo "- libcinnamon-desktop-dev (Cinnamon desktop integration)"
echo "- libxapp-dev (X app framework)"
echo "- python3-pip (for DS_Store testing tools)"
echo ""
echo "Press Enter to continue or Ctrl+C to cancel..."
read

echo "Updating package lists..."
sudo apt update

echo "Installing build dependencies..."
sudo apt install -y \
    meson \
    ninja-build \
    libgtk-3-dev \
    libglib2.0-dev \
    libxml2-dev \
    libexif-dev \
    libexempi-dev \
    libnotify-dev \
    libcinnamon-desktop-dev \
    libxapp-dev \
    python3-pip \
    python3-dev \
    gobject-introspection \
    libgirepository1.0-dev

echo ""
echo "Installing Python DS_Store library for testing..."
pip3 install --user ds-store

echo ""
echo "=== Installation Complete ==="
echo "You can now build Finderz with:"
echo "  meson setup build"
echo "  cd build && ninja"
echo ""
echo "To run Finderz:"
echo "  ./build/src/nemo"