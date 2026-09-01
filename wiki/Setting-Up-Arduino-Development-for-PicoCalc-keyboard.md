# Setting Up Arduino Development for PicoCalc keyboard

This guide walks you through setting up your Arduino environment to develop for the PicoCalc keyboard (STM32F103R8T6) microcontroller using the [STM32duino Arduino Core](https://github.com/stm32duino/Arduino_Core_STM32). It also includes instructions to manually install the [XPowersLib](https://github.com/cuu/XPowersLib.git) library.

## 1. Install Arduino IDE
Download and install the latest version of [Arduino IDE](https://www.arduino.cc/en/software) for your operating system.

## 2. Install STM32duino Core via Board Manager
To use STM32 microcontrollers with Arduino, you need to install the STM32duino core.

### **Steps:**
1. Open **Arduino IDE**.
2. Go to **File** → **Preferences**.
3. In the **Additional Board Manager URLs** field, add the following URL:
   ```
   https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
   ```
4. Click **OK**.
5. Open **Tools** → **Board** → **Boards Manager**.
6. Search for **STM32** and install **STM32 MCU based boards** by STMicroelectronics.

## 3. Select the Correct Board and Settings
Once the STM32 core is installed, configure the board settings:

1. Go to **Tools** → **Board** → **STM32 Boards** → **Generic STM32F1 series**.
2. In **Tools** menu, configure the following settings:
   - **Board part number**: `Generic F103R8Tx`
   - **Upload method**: `STM32CubeProgrammer(Serial)`

## 4. Install XPowersLib Manually
Instead of installing XPowersLib via the Arduino Library Manager, we will manually clone it into the correct location.

### **Steps:**
1. Open a terminal.
2. Navigate to your Arduino libraries folder:
   ```bash
   cd ~/Arduino/libraries
   ```
3. Clone the XPowersLib repository from the correct branch:
   ```bash
   git clone -b stm32f103r8t6 https://github.com/cuu/XPowersLib.git
   ```

This ensures the library is correctly placed for the Arduino IDE to detect it.

## 5. Get Code 
```bash
git clone https://github.com/clockworkpi/PicoCalc.git
```
the keyboard code is in `Code/picocalc_keyboard`

open Code/picocalc_keyboard/picocalc_keyboard.ino in Arduino IDE 


## 6. Upload Code to PicoCalc Keyboard
To upload your code:

1. Connect the picocalc board to your PC using a usb type-c cable.
2. Set the **DIP 1** to ON
3. Long-Press the **Power On** button to power on.
4. Click the **Upload** button in Arduino IDE.
5. After upload, set **DIP 1** to OFF

Here is a screenshot of working arduino ide:  

![screenshot](https://github.com/clockworkpi/PicoCalc/blob/master/wiki/picocalc_keyboard_arduino_ide_compile.png)

DIP switches are on the back of picocalc mainboard  
![dips](https://github.com/clockworkpi/PicoCalc/blob/master/wiki/picocalc_back_dip_switch.png)

### Libraries version info  

* STM32 MCU based boards 2.10.0
* XPowersLib 0.2.3 (cuu mod)


# Flash pre-compiled firmware 

- Put DIP 1 ON
- Connect PicoCalc with USB Type-C cable
- sudo stm32flash -w Bin/PicoCalc_BIOS_v1.6.bin -v -S 0x08000000 /dev/ttyUSB0
- Put DIP 1 OFF

assume we got **/dev/ttyUSB0** as port of PicoCalc   
here is the sample correct writing firmware logs from linux:   
```
$ sudo stm32flash -w PicoCalc_BIOS_v1.6.bin -v -S 0x08000000 /dev/ttyUSB0
stm32flash 0.7

http://stm32flash.sourceforge.net/

Using Parser : Raw BINARY
Location     : 0x8000000
Size         : 65536
Interface serial_posix: 57600 8E1
Version      : 0x22
Option 1     : 0x00
Option 2     : 0x00
Device ID    : 0x0410 (STM32F10xxx Medium-density)
- RAM        : Up to 20KiB  (512b reserved by bootloader)
- Flash      : Up to 128KiB (size first sector: 4x1024)
- Option RAM : 16b
- System RAM : 2KiB
Write to memory
Erasing memory
Wrote and verified address 0x08010000 (100.00%) Done.

```
## STM32CubeProgrammer
On other platform,like windows or Mac  

we can use the official tool [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) to do the flashing job  

- Put DIP 1 ON
- Connect PicoCalc with USB Type-C cable
- Open STM32CubeProgrammer with right permissions,on linux maybe need sudo,on windows maybe need the Administrator rights 
- Press **Connect**
- Open file ,load PicoCalc_BIOS_v1.6.bin
- Press **Download**
- Press Disconnect
- Put DIP 1 OFF
- Power On PicoCalc

here is the sample screenshot:  

![dips](https://github.com/clockworkpi/PicoCalc/blob/master/wiki/stm32cube_flash_kbd_firmware.png)


## Extracting firmware

Also if We want extract firmware from keyboard,here is the steps:  

- Put DIP 1 ON
- Connect PicoCalc with USB Type-C cable
- sudo stm32flash -r PicoCalc_BIOS_v1.6.bin -S 0x08000000:65536 /dev/ttyUSB0
- Put DIP 1 OFF

here is sample correct extracting logs from linux :  
```
$ sudo stm32flash -r PicoCalc_BIOS_v1.6.bin -S 0x08000000:65536 /dev/ttyUSB0
stm32flash 0.7

http://stm32flash.sourceforge.net/

Interface serial_posix: 57600 8E1
Version      : 0x22
Option 1     : 0x00
Option 2     : 0x00
Device ID    : 0x0410 (STM32F10xxx Medium-density)
- RAM        : Up to 20KiB  (512b reserved by bootloader)
- Flash      : Up to 128KiB (size first sector: 4x1024)
- Option RAM : 16b
- System RAM : 2KiB
Memory read
Read address 0x08010000 (100.00%) Done.
```

### Write firmware bin file back to the keyboard

```
sudo stm32flash -w PicoCalc_BIOS_v1.6.bin -v -S 0x08000000 /dev/ttyUSB0
```

sample writing back log: 
```
stm32flash 0.7

http://stm32flash.sourceforge.net/

Using Parser : Raw BINARY
Location     : 0x8000000
Size         : 65536
Interface serial_posix: 57600 8E1
Version      : 0x22
Option 1     : 0x00
Option 2     : 0x00
Device ID    : 0x0410 (STM32F10xxx Medium-density)
- RAM        : Up to 20KiB  (512b reserved by bootloader)
- Flash      : Up to 128KiB (size first sector: 4x1024)
- Option RAM : 16b
- System RAM : 2KiB
Write to memory
Erasing memory
Wrote and verified address 0x08010000 (100.00%) Done.
```

### Convert firmware bin to intel hex format
```
srec_cat PicoCalc_BIOS_v1.6.bin  -Binary -offset 0x08000000 -output PicoCalc_BIOS_factory_v1.6.hex -Intel
```





