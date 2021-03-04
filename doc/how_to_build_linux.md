# Setting up the development environment for GNU/Linux

## Required software

Building OpenToonz from source requires the following dependencies:
- Git
- GCC or Clang
- CMake (3.4.1 or newer).
- Qt5 (5.9 or newer)
- Boost (1.55 or newer)
- LibPNG
- SuperLU
- Lzo2
- FreeType
- LibMyPaint (1.3 or newer)
- Jpeg-Turbo (1.4 or newer)
- OpenCV 3.2 or newer

### Installing Dependencies on Debian / Ubuntu 16.04 (Xenial)

```
$sudo apt-get install build-essential git cmake pkg-config libboost-all-dev qt5-default qtbase5-dev libqt5svg5-dev qtscript5-dev qttools5-dev qttools5-dev-tools libqt5opengl5-dev qtmultimedia5-dev libqt5multimedia5-plugins libqt5serialport5-dev libsuperlu-dev 
liblz4-dev libusb-1.0-0-dev liblzo2-dev libpng-dev libjpeg-dev libglew-dev freeglut3-dev libfreetype6-dev libjson-c-dev qtwayland5 libmypaint-dev libopencv-dev libturbojpeg-dev
```

Find a PPA repository for MyPaint 1.3 and install the following:
```
$ sudo apt-get install -y libmypaint-dev
```

Find a PPA repository for OpenCV 3.2 or later and install the following:
```
$ sudo apt-get install -y libopencv-dev
```

Notes:
* It's possible we also need `libgsl2` (or maybe `libopenblas-dev`)
* We may also need `libegl1-mesa-dev libgles2-mesa-dev libglib2.0-dev liblzma-dev`
* For Qt, MyPaint and OpenCV, you can alternatively build and install from source.

### Installing Dependencies on Fedora
(it may include some useless packages)

```
$ sudo dnf install gcc gcc-c++ automake git cmake boost boost-devel SuperLU SuperLU-devel lz4-devel lzma libusb-devel lzo-devel libjpeg-turbo-devel libGLEW glew-devel freeglut-devel freeglut freetype-devel libpng-devel qt5-qtbase-devel qt5-qtsvg qt5-qtsvg-devel qt5-qtscript qt5-qtscript-devel qt5-qttools qt5-qttools-devel qt5-qtmultimedia-devel blas blas-devel json-c-devel libtool intltool make qt5-qtmultimedia
```

For newest versions of OS you may install libmypaint from repository and don't need to build it from source:

```
$ sudo dnf install libmypaint-devel
```


### Installing Dependencies on ArchLinux

```
$ sudo pacman -S base-devel git cmake boost boost-libs qt5-base qt5-svg qt5-script qt5-tools qt5-multimedia lz4 libusb lzo libjpeg-turbo glew freeglut freetype2
$ sudo pacman -S blas cblas
```
From AUR, using eg. yaourt:
```
$ yaourt -S superlu libmypaint
```

Notes:
* ArchLinux has `blas` split into `blas` and `cblas`.

### Installing Dependencies on openSUSE

```
$ zypper in boost-devel cmake freeglut-devel freetype2-devel gcc-c++ glew-devel libQt5OpenGL-devel libjpeg-devel liblz4-devel libpng16-compat-devel libqt5-linguist-devel libqt5-qtbase-devel libqt5-qtmultimedia-devel libqt5-qtscript-devel libqt5-qtsvg-devel libtiff-devel libusb-devel lzo-devel openblas-devel pkgconfig sed superlu-devel zlib-devel json-c-devel libqt5-qtmultimedia
```

For newest versions of OS you may install libmypaint from repository and don't need to build it from source:

```
$ zypper install libmypaint-devel
```

## Build libmypaint dependency

If your linux distributions does not have libmypaint package, then build it from the source:

```
$ git clone https://github.com/mypaint/libmypaint.git -b v1.3.0
$ cd libmypaint
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
$ sudo ldconfig
$ cd ..
```

## Build instructions

### Cloning the GIT Tree

```
$ git clone https://github.com/opentoonz/opentoonz
```

### Copying the 'stuff' Directory

TODO: some parts should really be installed in $prefix/ instead... and some other in various cache or user-local places.
cf. https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
Until then we just follow the Win32/OSX layout.

The `~/.config/OpenToonz/` directory contains your settings, work and other files.

Initialize this path with the folling commands:

```
$ mkdir -p $HOME/.config/OpenToonz
$ cp -r opentoonz/stuff $HOME/.config/OpenToonz/
```

*Currently this is required to run OpenToonz.*

### Creating SystemVar.ini

TODO: fix the code to discover it automatically

```
$ cat << EOF > $HOME/.config/OpenToonz/SystemVar.ini
[General]
OPENTOONZROOT="$HOME/.config/OpenToonz/stuff"
OpenToonzPROFILES="$HOME/.config/OpenToonz/stuff/profiles"
TOONZCACHEROOT="$HOME/.config/OpenToonz/stuff/cache"
TOONZCONFIG="$HOME/.config/OpenToonz/stuff/config"
TOONZFXPRESETS="$HOME/.config/OpenToonz/stuff/fxs"
TOONZLIBRARY="$HOME/.config/OpenToonz/stuff/library"
TOONZPROFILES="$HOME/.config/OpenToonz/stuff/profiles"
TOONZPROJECTS="$HOME/.config/OpenToonz/stuff/projects"
TOONZROOT="$HOME/.config/OpenToonz/stuff"
TOONZSTUDIOPALETTE="$HOME/.config/OpenToonz/stuff/studiopalette"
EOF
```
Note the generated file must not actually contain `$HOME`, this expands to an absolute path in the generated file.

### Building LibTIFF

TODO: make sure we can use the system libtiff instead and remove this section.
Features from the modified libtiff are needed currently, so this isn't a simple switch.

```
$ cd opentoonz/thirdparty/tiff-4.0.3
$ ./configure --with-pic --disable-jbig
$ make -j$(nproc)
$ cd ../../
```

### Building OpenToonz

```
$ cd toonz
$ mkdir build
$ cd build
$ cmake ../sources
$ make -j$(nproc)
```

The build takes a lot of time, be patient. CMake may not pick up all the required dependencies. On Fedora 30, it can be helpful to use 
```
$cmake ../sources/ -DSUPERLU_INCLUDE_DIR=/usr/include/SuperLU
```
instead of just
```
$cmake ../sources/
```

### Troubleshooting Build Errors

If something doesn't compile or link, please run `make` this way to help spot the problem:
```
$ LANG=C make VERBOSE=1
```

#### Debug Build
If you need to debug the application, you should be able to use `cmake -DCMAKE_BUILD_TYPE=Debug`.


### Running OpenToonz

You can now run the application:

```
$ LD_LIBRARY_PATH=./lib/opentoonz:$LD_LIBRARY_PATH
$ ./bin/OpenToonz
```

### Performing a System Installation

The steps above show how to run OpenToonz from the build directory,
however you may wish to install OpenToonz onto your system.

OpenToonz will install to `/opt/opentoonz` by default, to do this run:

```
$ sudo make install
```

Then you can launch OpenToonz by running `/opt/opentoonz/bin/opentoonz`.

You can change the installation path by modifying the `CMAKE_INSTALL_PREFIX` CMake variable.

----

# Linux Package Definitions

It may be helpful to use existing packages as a reference when creating a package for your own distribution.

- ArchLinux (AUR):
  https://aur.archlinux.org/packages/opentoonz-git/

- App-Image (Portable):
  https://github.com/morevnaproject/morevna-builds

