#!/bin/bash

# Finderz build script

echo "=== Building Finderz ==="

# Clean previous build if it exists
if [ -d "build" ]; then
    echo "Removing old build directory..."
    # Using safer approach - move instead of rm
    mv build build.old.$(date +%s) 2>/dev/null
fi

echo "Setting up build with meson..."
if ! meson setup build; then
    echo ""
    echo "ERROR: Meson setup failed!"
    echo "Try installing missing dependencies:"
    echo "  bash install-deps.sh"
    exit 1
fi

echo "Building with ninja..."
cd build
if ! ninja; then
    echo ""
    echo "ERROR: Build failed!"
    echo "Check the error messages above for missing libraries."
    exit 1
fi

echo ""
echo "=== Build Complete! ==="
echo ""
echo "To run Finderz:"
echo "  G_MESSAGES_DEBUG=all ./build/src/nemo"
echo ""
echo "To test metadata:"
echo "  bash test-metadata.sh"