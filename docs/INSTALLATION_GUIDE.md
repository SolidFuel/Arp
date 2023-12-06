# solidArp Installation Guide

The latest released version of _solidArp_ can be found on github at:

https://github.com/SolidFuel/Arp/releases/latest

There will be a few files available to help with install.
Below are instructions for each plaform.

## Windows (64 bit)

### Installer
The easiest way to install on windows is to use the included installer.
Download the file `solidArp-VX.X.X-win64-install.exe`, where X.X.X will 
be the release number.

**Note** Chrome browser will block the download with a warning about it
not being "commonly downloaded". Click through
the warning to download anyway.

### VST3 zip

The zip file `solidArp-VX.X.X-win64-vst3.zip` contains the VST3 file. Unzip
the archive and move the `solidArp.vst3` file to the `C:\Program Files\Common Files\VST3`
directory.

**Note** Chrome browser will block the download with a warning about it
not being "commonly downloaded". Click through
the warning to download anyway.

## Linux

### .deb package file

The available `.deb` package is only for the `amd64` architecture.

Download the `solidArp_X.X.X_linux-amd64.deb` file. Install using

```
sudo apt install ./solidArp_X.X.X_linux-amd64.deb
```

This will place the vst into the `/usr/lib/vst3` directory.

### VST3 zip

_Note_ The release builds only work on x86_64 (amd64) architecture.

Unzip the file `solidArp-Vx.x.x-linux-amd64.zip`. Place the resulting 
`solidArp.vst3` directory into `~/.vst3`

This can also be placed in `/usr/local/lib/vst3` for system-wide use. You will
need superuser privileges to do so.

## MacOS

### Installer
A pkg style installer is provided that works for both intel and arm architectures.
All version from 10.15 to 14.1 are supported.

Download the file `solidArp_VX.X.X_macos-universal.pkg`. Double click on the file
and follow the directions given.
