# Building

## Setup

### Linux

The code supports both `g++` as well as `clang`. Others may work.

Be sure to install the following packages. Below is for Debian and derived.

```bash
sudo apt-get update

# tools
sudo apt install g++ git cmake

# dev packages
sudo apt install xorg-dev libasound2-dev libgl1-mesa-dev  \
    libcurl4-openssl-dev libwebkit2gtk-4.0-dev
```

### MacOS

Install `xcode` from the app store and CLI tool (`xcode-select --install`).

You will also need to install `cmake` and `git` via homebrew.

### Windows

Install the [Visual Studio Community edition] (https://visualstudio.microsoft.com/vs/community/)

This bundles a version of `cmake`

You will need to [install git from the website](https://git-scm.com/download/win)

## Clone, configure, build

```bash
git clone https://github.com/SolidFuel/Arp.git
cd Arp
git submodule init
git submodule update

cmake -S. -Bbuild -DCMAKE_BUILD_TYPE:string=Release
cmake --build build --config Release
```

The plugin can be found under `build/Source/Starp_artefacts/Release/VST3`