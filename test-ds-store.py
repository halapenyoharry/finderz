#!/usr/bin/env python3
"""
Test script for DS_Store parsing
This helps verify what Finderz should be reading from .DS_Store files
"""

import sys
import os
from pathlib import Path

try:
    import ds_store
except ImportError:
    print("Error: ds_store module not found")
    print("Install with: pip3 install --user ds-store")
    sys.exit(1)

def analyze_ds_store(ds_store_path):
    """Analyze a .DS_Store file and print its contents"""
    
    if not os.path.exists(ds_store_path):
        print(f"Error: {ds_store_path} not found")
        return False
    
    print(f"\n=== Analyzing {ds_store_path} ===")
    print(f"File size: {os.path.getsize(ds_store_path)} bytes")
    
    try:
        with ds_store.DSStore.open(ds_store_path, 'r') as d:
            print("\nDS_Store contents:")
            
            # Group records by filename
            files_data = {}
            
            for record in d:
                filename = record.filename
                if filename not in files_data:
                    files_data[filename] = {}
                
                code = record.code.decode('utf-8') if isinstance(record.code, bytes) else record.code
                files_data[filename][code] = record.value
            
            # Print organized data
            for filename, data in sorted(files_data.items()):
                print(f"\n  File: {filename}")
                for code, value in sorted(data.items()):
                    # Format output based on code type
                    if code == 'Iloc':
                        print(f"    Icon location (Iloc): x={value[0]}, y={value[1]}")
                    elif code in ['bwsp', 'lsvp', 'lsvP', 'icvp']:
                        print(f"    View settings ({code}): {len(value)} bytes of plist data")
                    elif code == 'vmod':
                        print(f"    View mode (vmod): {value}")
                    else:
                        print(f"    {code}: {value}")
            
            # Look for view settings
            if '.' in files_data:
                folder_settings = files_data['.']
                print("\n=== Folder View Settings ===")
                
                if 'vmod' in folder_settings:
                    print(f"  View mode: {folder_settings['vmod']}")
                
                if 'icvp' in folder_settings:
                    print("  Has icon view settings (icvp)")
                
                if 'lsvp' in folder_settings or 'lsvP' in folder_settings:
                    print("  Has list view settings (lsvp/lsvP)")
                
                if 'bwsp' in folder_settings:
                    print("  Has browser window settings (bwsp)")
        
        return True
        
    except Exception as e:
        print(f"Error reading DS_Store: {e}")
        return False

def find_ds_store_files(directory):
    """Find all .DS_Store files in a directory tree"""
    
    ds_stores = []
    for root, dirs, files in os.walk(directory):
        if '.DS_Store' in files:
            ds_stores.append(os.path.join(root, '.DS_Store'))
    
    return ds_stores

def create_test_ds_store():
    """Create a simple test .DS_Store file"""
    
    test_file = 'test.DS_Store'
    print(f"\nCreating test DS_Store file: {test_file}")
    
    try:
        with ds_store.DSStore.open(test_file, 'w+') as d:
            # Add some test records
            d.insert(ds_store.DSStoreEntry('.', b'vmod', 1))  # Icon view
            d.insert(ds_store.DSStoreEntry('test.txt', b'Iloc', (100, 200)))
            d.insert(ds_store.DSStoreEntry('folder', b'Iloc', (300, 400)))
        
        print(f"Created {test_file}")
        return test_file
    except Exception as e:
        print(f"Error creating test DS_Store: {e}")
        return None

def main():
    if len(sys.argv) > 1:
        # Analyze specific DS_Store file
        ds_store_path = sys.argv[1]
        analyze_ds_store(ds_store_path)
    else:
        # Interactive mode
        print("DS_Store Test Tool for Finderz")
        print("================================")
        print("\nOptions:")
        print("1. Analyze a specific .DS_Store file")
        print("2. Find all .DS_Store files in current directory")
        print("3. Create a test .DS_Store file")
        print("4. Exit")
        
        while True:
            choice = input("\nChoice (1-4): ").strip()
            
            if choice == '1':
                path = input("Enter path to .DS_Store file: ").strip()
                analyze_ds_store(path)
            
            elif choice == '2':
                print("\nSearching for .DS_Store files...")
                found = find_ds_store_files('.')
                if found:
                    print(f"Found {len(found)} .DS_Store files:")
                    for f in found:
                        print(f"  - {f}")
                    
                    analyze = input("\nAnalyze all? (y/n): ").strip().lower()
                    if analyze == 'y':
                        for f in found:
                            analyze_ds_store(f)
                else:
                    print("No .DS_Store files found")
            
            elif choice == '3':
                test_file = create_test_ds_store()
                if test_file:
                    analyze_ds_store(test_file)
            
            elif choice == '4':
                break
            
            else:
                print("Invalid choice")

if __name__ == '__main__':
    main()