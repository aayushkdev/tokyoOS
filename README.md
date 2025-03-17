# Tokyo OS

Tokyo OS is a hobby operating system written in C, aiming to explore and learn about operating system development. It currently supports basic features like framebuffer output, serial communication, Global Descriptor Table (GDT) and Interrupt Descriptor Table (IDT) setup, and simple printing functionality.

## Features

* **Framebuffer Output:** Basic display functionality to render text on the screen.
* **Serial Communication (COM1):** Initialization and basic communication through the COM1 serial port.
* **Global Descriptor Table (GDT):** Setup of the GDT for memory segmentation.
* **Interrupt Descriptor Table (IDT):** Setup of the IDT for interrupt handling.
* **Basic Printing:** `printf` and `writeSerial` function for outputting text to the framebuffer and serial port.

## Getting Started

### Prerequisites

* A cross-compiler toolchain for x86_64 (configured to `$(HOME)/opt/cross/bin/x86_64-elf-gcc` and `$(HOME)/opt/cross/bin/x86_64-elf-ld` in the Makefile).
* NASM assembler.
* xorriso for ISO image creation.
* Limine bootloader tools (installed and available in your PATH).
* QEMU or another emulator/virtual machine for testing.

### Building and Running

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/aayushkdev/tokyoOS.git
    cd tokyoOS
    ```

2.  **Build the ISO image:**

    ```bash
    make
    ```

    This will:

    * Compile the C and assembly source files.
    * Link the object files into a kernel binary.
    * Create an ISO image (`dist/tokyo.iso`) with the kernel and Limine bootloader.
    * Install the Limine bootloader onto the ISO.

3.  **Run in QEMU:**

    To run the generated ISO image in QEMU, use:

    ```bash
    make run
    ```

4.  **Debug in QEMU:**

    To run the generated ISO image in QEMU with debug flags enabled, use:

    ```bash
    make debug run
    ```

5. **Cleaning the build:**

    To remove generated files, use:
    
    ```bash
    make clean
    ```