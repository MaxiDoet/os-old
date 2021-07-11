A simple 32bit os written in C

## Drivers
 - VBE 800x600
 - Serial Port
 - PS/2 (Keyboard, Mouse)
 - PCI
 - PIT
 - RTC
 - ATA PIO
 - Intel AC97
 - RTL8139

## Features
 - SMBIOS detection
 - Demo desktop
 - Working ext2 fs (read only)

## Building
 1. Clone this repository ``` git clone https://github.com/TheHeroCraft1579/os``` 
 2. Install dependencys ``` sudo apt-get install gcc nasm build-essential gcc-multilib xorriso```
 3. Adjust config ``` nano config.h ```
 4. Build ``` ./build.sh ```

## Progress
 <p>
  Audio System / Driver 
  <img src="https://progress-bar-theherocraft1579.herokuapp.com/30">
 </p>
 <p>
 <p>
  Network Driver / Stack
  <img src="https://progress-bar-theherocraft1579.herokuapp.com/40">
 </p>
 <p>
  Filesystem / ATA Driver
  <img src="https://progress-bar-theherocraft1579.herokuapp.com/50">
 </p>
 <p>
  Desktop / GUI
  <img src="https://progress-bar-theherocraft1579.herokuapp.com/30">
 </p>

 
