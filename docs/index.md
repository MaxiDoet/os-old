A simple 32bit os written in C

## Drivers
 - VBE 800x600
 - Serial Port
 - PS/2 (Keyboard, Mouse)
 - PCI
 - PIT
 - RTC

## Building
 1. Clone this repository ``` git clone https://github.com/TheHeroCraft1579/os``` 
 2. Install dependencys ``` sudo apt-get install gcc nasm build-essential gcc-multilib xorriso```
 3. Adjust config ``` nano config.h ```
 4. Build ``` ./build.sh ```
