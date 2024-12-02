# Qrlot

Qrlot (A lot of QR)

The cross-platform program for creating a large number of QR codes with the ability to export as images or Excel table.

![Alt text](../main/images/Screenshot_20241202_222522.png?raw=true)

![Alt text](../main/images/Screenshot_20241202_222805.png?raw=true)

## Build instructions

Open terminal and enter the following commands.

### Windows (MSYS2 MinGW64)

pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-tools mingw-w64-x86_64-toolchain</br>
git clone https://github.com/stanislav539261/qrlot --recurse-submodules</br>
mkdir -p qrlot/build</br>
cd qrlot/build</br>
qmake6 .. -spec win32-g++</br>
mingw32-make -j NUM_THREADS</br>

### GNU/Linux

Debian:</br>
apt-get install gcc make qt6-base-dev qt6-base-private-dev qt6-tools-dev

Arch Linux:</br>
pacman -S gcc make qt6-base qt6-tools

git clone https://github.com/stanislav539261/qrlot --recurse-submodules</br>
mkdir -p qrlot/build</br>
cd qrlot/build</br>
qmake6 .. -spec linux-g++</br>
make -j NUM_THREADS</br>
