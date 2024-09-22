# LED Matrix Project

## Objective
This project aims to create a user-space application to control an 8x8 LED matrix using the MAX7219 chip. The application will blink an LED in a smiley face pattern, utilizing the SPI communication protocol for interaction between the Raspberry Pi 3B and the MAX7219.

## Hardware Requirements
- **Raspberry Pi 3B**
- **MAX7219 LED Matrix (8x8)**
- Jumper wires for connections

## Software Requirements
- Raspberry Pi OS
- WiringPi or another SPI library for C programming
- Cross-compilation tools (for building the kernel module on a host machine)
- Linux kernel headers (matching the kernel version on Raspberry Pi)

## Project Overview
Due to the limited memory space on the Raspberry Pi, the necessary kernel module is cross-compiled on a host machine (laptop) and transferred to the Raspberry Pi. The user-space application will then interact with the MAX7219 to display the smiley face.

### Tasks
1. **Cross-Compile Kernel Module**: The kernel module for SPI communication is developed and compiled on the host machine.
2. **Transfer Module**: The compiled module is transferred to the Raspberry Pi.
3. **Develop User-Space Application**: A user-space application is created to send commands to the MAX7219 over SPI, enabling it to blink the LED in a smiley face pattern.

## Setup Instructions
1. **Wiring the MAX7219 to the Raspberry Pi**:
   - Connect the MAX7219 to the Raspberry Pi using appropriate GPIO pins.
   - Ensure that power and ground connections are made correctly.

2. **Compiling the Kernel Module**:
   - Use the following commands to compile the kernel module on the host machine:
     ```bash
     make -C /path/to/kernel/source M=$PWD modules
     ```

3. **Transferring the Module**:
   - Use SCP or another method to transfer the compiled `.ko` file to the Raspberry Pi:
     ```bash
     scp my_module.ko pi@raspberrypi:/path/to/destination
     ```

4. **Loading the Module**:
   - On the Raspberry Pi, load the module using:
     ```bash
     sudo insmod my_module.ko
     ```

5. **Running the User-Space Application**:
   - Compile and run the user-space application that communicates with the MAX7219 to display the smiley face:
     ```bash
     gcc -o led_matrix_app led_matrix_app.c -lwiringPi
     sudo ./led_matrix_app
     ```

6. **Testing the LED Matrix**:
   - After running the application, the LED matrix should start displaying the smiley face pattern.

## Documentation
For detailed explanations, source code, and additional resources, refer to the attached PDF document.
