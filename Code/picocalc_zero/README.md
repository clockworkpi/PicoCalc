# PicoCalc Zero

## 32bit PicoCalc_zero2w_v1.0.img.bz2 
Kernel git url: `https://github.com/raspberrypi/linux.git`  
Branch: `rpi-6.1.y`  
Kernel commit hash:`0afb5e98488aed7017b9bf321b575d0177feb7ed`    
Pi-gen: https://github.com/cuu/pi-gen/commit/1d60f6d528402bda43a75a375b243d9ebee3d8fa

### How to compile
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


### /boot/config.txt
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


## 64bit os image
Kernel git url: `https://github.com/raspberrypi/linux.git`
Branch: `rpi-6.18.y`


### How to compile
```
git clone --depth=1 --branch rpi-6.18.y https://github.com/raspberrypi/linux

cd linux

git reset --hard b3495ea401d709af0701a85babcba5e3d788a602

git apply 1.patch

KERNEL=kernel8
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig


./scripts/config -m CONFIG_KEYBOARD_PICOCALC
./scripts/config -m TINYDRM_PICOCALC_ILI9488
make ARCH=arm64 KERNEL=kernel8 CROSS_COMPILE=aarch64-linux-gnu- olddefconfig

make ARCH=arm64 KERNEL=kernel8 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs
```


