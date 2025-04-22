# WallCycle
WallCycle aims to be a simple wallpaper changer written in plain C. Currently it is only availiable for Windows, but I plan to port it to Linux and possibly MacOS in the future.
It works by changing the wallpaper twice a day, so you can have a different wallpaper for the day and the night. This can reduce eye strain and make your desktop look more appealing.

## Table of Contents
- [WallCycle](#wallcycle)
  - [Table of Contents](#table-of-contents)
  - [Installation](#installation)
  - [Building](#building)
  - [Configure](#configure)
    - [Wallpaper](#wallpaper)
    - [Times](#times)
  - [Customization](#customization)
  - [Contributing](#contributing)

## Installation
WallCycle uses `inno Setup` to create the installer. This is a free installer for Windows that is easy to use and has a lot of features.

A `installer.iss` configuration file is provided in the `install` folder.

To install WallCylcle you can just use the provided installer. It will install the program and create a shortcut in the start menu. (Currently there is no release, because i have to update the CI pipeline to build the installer.)
> The installer is not signed, so you might get a warning from Windows. This is normal, as I am not a big company and don't have the money to sign the installer  

You can also build it from source if you want to.

## Building
To build WallCycle you need `make`, `gcc` and `python` installed. You can use the provided Makefile to build the project.
At first you need to create a virtual environment and install the dependencies:
```bash
python -m venv .venv
pip install -r ./tooling/requirements.txt
```
After this you have to build the `.ico` files for the system tray animation. This is done by using the `build_icons.py` script from the makefile:
```bash
make animation
```
Then you can build the project by running:
```bash
make
```
Optionally you can build the installer by running:
```bash
make release
```
This will create a `WallCycle-setup.exe` file in the `release` folder. You can run this file to install WallCycle.

To remove the created files you can run:
```bash
make animation-clean
make clean
make release-clean
```
This will remove the `build` and `release` folders and the `.ico` files.

## Configure
### Wallpaper
To change the used wallpapers you can either change `day.jpg` and `night.jpg` in the `img` folder or you can edit the `config.ini` file.

### Times
To customize the times when the wallpaper changes you can use the `right-click` menu of the system tray icon.  
Also you can use the `config.ini` file to set the times. The format is `HH` in the 24h format.

> Please do not touch the `State` section in the `config.ini` file. This is used to store the current state of the program and is automatically updated by the program. If you change this section, the program might not work as expected.

## Customization
You can customize the animation of the system tray icon by providing your own `.png` files.  
Note that there have to be `34` frames, starting at `Animation00.png` for full night and ending at `Animation33.png` for full day.  
The files have to be in the `include/src/Animation` folder. The frames have to have a transparent or `#1E1E1E` background as specified in the makefile.

To change the icon of the program you can provide your own `.ico` file. The file has to be in the `include/src` folder and has to be named `Icon.ico`. The icon will be used for the program and the installer.

## Contributing
If you have any ideas or want to contribute to this project, feel free to open an issue or a pull request. I am happy about any help I can get.
