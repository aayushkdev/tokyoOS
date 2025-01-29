#!/bin/bash

# Run make clean to clean previous builds
echo "Cleaning previous builds..."
make clean

# Run make to build the project
echo "Building the project..."
make

# Run QEMU to boot the kernel
echo "Booting the kernel with QEMU..."
qemu-system-x86_64 -cdrom dist/tokyo.iso -m 512M


