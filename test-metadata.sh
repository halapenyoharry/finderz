#!/bin/bash

# Test script to verify Finderz metadata extraction

echo "=== Finderz Metadata Test ==="
echo ""

# Create a test image with metadata
TEST_DIR="/tmp/finderz-test"
mkdir -p "$TEST_DIR"

echo "1. Creating test image with AI metadata..."
cat > "$TEST_DIR/create_test.py" << 'EOF'
#!/usr/bin/env python3
from PIL import Image
from PIL.PngImagePlugin import PngInfo
import json

# Create a simple test image
img = Image.new('RGB', (512, 512), color='blue')

# Add AI metadata in PNG chunks
metadata = PngInfo()
metadata.add_text("prompt", "a beautiful sunset over mountains, highly detailed")
metadata.add_text("parameters", """a beautiful sunset over mountains, highly detailed
Negative prompt: ugly, blurry
Steps: 20, Sampler: Euler a, CFG scale: 7.5, Seed: 123456, Model: SD 1.5""")

# ComfyUI workflow (simplified)
workflow = {
    "model": "stable-diffusion-v1-5",
    "prompt": "a beautiful sunset over mountains",
    "seed": 123456
}
metadata.add_text("workflow", json.dumps(workflow))

# Save with metadata
img.save('/tmp/finderz-test/test_ai_image.png', pnginfo=metadata)
print("Created test_ai_image.png with AI metadata")
EOF

python3 "$TEST_DIR/create_test.py" 2>/dev/null || echo "Need PIL: pip install pillow"

echo ""
echo "2. Setting extended attributes..."
setfattr -n user.finderz.rating -v "5" "$TEST_DIR/test_ai_image.png" 2>/dev/null || \
    xattr -w user.finderz.rating "5" "$TEST_DIR/test_ai_image.png" 2>/dev/null || \
    echo "xattr not available on this filesystem"

setfattr -n user.finderz.ai.model -v "Stable Diffusion 1.5" "$TEST_DIR/test_ai_image.png" 2>/dev/null || \
    xattr -w user.finderz.ai.model "Stable Diffusion 1.5" "$TEST_DIR/test_ai_image.png" 2>/dev/null

echo ""
echo "3. Reading metadata back..."
echo "Extended attributes:"
getfattr -d "$TEST_DIR/test_ai_image.png" 2>/dev/null | grep finderz || \
    xattr -l "$TEST_DIR/test_ai_image.png" 2>/dev/null | grep finderz || \
    echo "No finderz xattrs found"

echo ""
echo "4. Testing with Python ds_store library..."
python3 "$TEST_DIR/../test-ds-store.py" "$TEST_DIR" 2>/dev/null || echo "ds_store module not installed"

echo ""
echo "=== Test Complete ==="
echo ""
echo "To test in Finderz:"
echo "1. Build: meson setup build && cd build && ninja"
echo "2. Run: G_MESSAGES_DEBUG=all ./src/nemo $TEST_DIR"
echo "3. Switch to list view"
echo "4. Right-click column header → Select columns"
echo "5. Enable: AI Prompt, AI Model, Rating"
echo "6. You should see the metadata!"
echo ""
echo "Test files created in: $TEST_DIR"