# PicoCalc Zero

## Kernel
Git url: `https://github.com/raspberrypi/linux.git`  
Branch: `rpi-6.1.y`  
Patch commit hash:`0afb5e98488aed7017b9bf321b575d0177feb7ed`    

## How to compile
```
git clone  https://github.com/raspberrypi/linux

cd linux

git reset --hard 0afb5e98488aed7017b9bf321b575d0177feb7ed

wget https://raw.githubusercontent.com/clockworkpi/PicoCalc/refs/heads/master/Code/picocalc_zero/kernel.patch

git apply  kernel.patch

make ARCH=arm KERNEL=kernel7 CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig

./scripts/config -m CONFIG_KEYBOARD_PICOCALC
make ARCH=arm KERNEL=kernel7 CROSS_COMPILE=arm-linux-gnueabihf-  olddefconfig

make ARCH=arm KERNEL=kernel7 CROSS_COMPILE=arm-linux-gnueabihf- -j2 zImage modules dtbs


```


## /boot/config.txt
```
dtparam=i2c_arm=on
dtparam=i2c_arm_baudrate=10000
dtoverlay=picocalc_kbd
dtoverlay=i2c-rtc,pcf8563
dtoverlay=dwc2
dtparam=spi=on
dtoverlay=spi0-0cs
dtoverlay=spi1-1cs
dtoverlay=mmc-spi1
camera_auto_detect=1
display_auto_detect=1
#max_framebuffers=2 (line commented for TFT ILI9488 installation on 06/27/2025)
disable_overscan=1
[cm4]
otg_mode=1
[all]
#[pi4]
arm_boost=1
#Modifications for ILI9488 installation implemented by the script on 06/27/2025
hdmi_force_hotplug=1
hdmi_cvt=320 320 60 1 0 0 0
hdmi_group=2
hdmi_mode=87
gpu_mem=128
# Utilized for TFT ILI9488 setup script by AdamoMD
# https://github.com/adamomd/4inchILI9488RpiScript/
# Feel free to send feedback and suggestions.
dtparam=audio=on
dtoverlay=audremap,pins_13_12
enable_uart=1
```



