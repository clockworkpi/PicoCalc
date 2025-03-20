# Setting Up the Pico SDK on Linux for PicoCalc Development

This guide walks you through setting up the Raspberry Pi Pico SDK on a Linux system for **PicoCalc** development. By the end, you'll have a working environment to compile and flash firmware to your Raspberry Pi Pico on PicoCalc.

## Prerequisites
Before getting started, make sure you have the following:

- A Linux system (Ubuntu, Debian, etc.)
- Raspberry Pi Pico 
- USB cable for flashing firmware
- Basic familiarity with the terminal

## Step 1: Install Required Dependencies
Open a terminal and install the necessary tools:

```bash
sudo apt update && sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential git
```

## Step 2: Clone the Pico SDK
Navigate to a workspace directory and clone the official Raspberry Pi Pico SDK:

```bash
mkdir -p ~/pico && cd ~/pico
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git checkout tags/2.0.0 -b sdk2.0.0
git submodule update --init
```

## Step 3: Set Up Environment Variables
To ensure the build system finds the SDK, set an environment variable:

```bash
echo 'export PICO_SDK_PATH=~/pico/pico-sdk' >> ~/.bashrc
source ~/.bashrc
```

For Zsh users:

```bash
echo 'export PICO_SDK_PATH=~/pico/pico-sdk' >> ~/.zshrc
source ~/.zshrc
```

## Step 4: Create a New Project
Let's create a sample project using the Pico SDK:

```bash
cd ~/pico
git clone https://github.com/raspberrypi/pico-examples.git
git checkout tags/sdk-2.0.0 -b sdk2.0.0
mkdir -p pico-examples/build && cd pico-examples/build
```

Now, configure the project with CMake:

```bash
cmake ..
```

## Step 5: Compile the Code
Compile a sample program (e.g., Blink):

```bash
make -j$(nproc) blink
```

If successful, this will generate a `blink.uf2` file inside `build/blink/`.

## Step 6: Flash the Firmware
To flash the firmware to your Pico:

1. Hold down the **BOOTSEL** button on the Pico.
2. Plug it into your computer via USB then release **BOOTSEL**.
3. The Pico should appear as a mass storage device (`RPI-RP2`).
4. Copy the `blink.uf2` file to the Pico:

   ```bash
   cp blink/blink.uf2 /media/$USER/RPI-RP2/
   ```

The Pico will automatically reboot and start running the Blink program.

## Step 7: Verify Everything Works
To check if your Pico is working correctly, you can use `minicom` or `screen` to monitor serial output:

```bash
sudo apt install -y minicom
minicom -b 115200 -o -D /dev/ttyACM0
```

Press **Ctrl+A, then Z**, and select **Quit** when finished.

## Conclusion
You now have the Raspberry Pi Pico SDK set up on Linux and successfully flashed firmware onto your Pico. 
You can start developing and customizing firmware for **PicoCalc** projects!

For more details, refer to the official [Pico SDK documentation](https://github.com/raspberrypi/pico-sdk).

