# OpenToonz (eXperimental)

![](./toonz/sources/toonz/Resources/splash_OTX.svg)

OpenToonz (eXperimental), also known as OTX, is a portable version of OpenToonz that contain changes, fixes and new features that will either be in the next official release or somewhere down the road.

This repository IS NOT source code repository of OpenToonz and should not be used for continued development. Please see https://github.com/opentoonz/opentoonz for the source code.

### Program Requirements

- Windows 7 (64bit) or later
- macOS 10.13 (High Sierra) or later
- Ubuntu 14.04 (or equivalent) or later

For additional requirements, please refer to the OpenToonz site at <https://opentoonz.github.io/e/index.html>.

### Installation and Usage

OTX builds are created as portables so no installation is required.  You can keep existing installed OpenToonz versions on your system.

- Windows:

   - Download the file: OpenToonz-OTX-win.zip
   - Right-click the OpenToonz-OTX-win.zip file and select "Extract All..." to extract the contents.
      - This will extract a folder called "OpenToonzPortable".
   - Move the OpenToonzPortable folder to a desired location (i.e. C:\Program Files, Desktop, a USB key)
   - Double-click the 'OpenToonzPortable\OpenToonz.exe' to run

- macOS:

   - Download the file: OpenToonz-OTX-osx.dmg
   - Double-click the OpenToonz-OTX-osx.dmg file to open it
   - Drag the "OpenToonz" application to wherever you want to run it from (i.e Applications, Desktop, a USB key)
        - If you already have the official OpenToonz application installed, you should rename the OTX version (OpenToonz_OTX) if you want to keep it in the same directory as the official version.
   - Double-click the "OpenToonz" application to run.
        - The 1st time you do this, macOS will likely block the application from running. You will need to update your Security settings to allow it to run.

   - NOTE(s): 
      - Do NOT run the OTX application from inside the .dmg file. It will not work.
      - If you run this for the first time on a system that already has OpenToonz officially installed, it will detect the preferences and prompt you 2x to import them, once for preferences and another for room layouts.

        However, due to a Qt bug, not OT, the popups appear behind the splash screen and will make it seem like it's taking forever to load.

        If it seems like it is taking forever to initialize OTX, click away from the splash screen to reveal the popups. Click and drag it away from splash screen area before interacting with it.

- Linux:

   - Download the file: OpenToonz-OTX-linux.tar.gz
   - Right-click the OpenToonz-OTX-linux.tar.gz file and "Extract Here" to extract the contents.
        - This will create a folder as "OpenToonz-OTX-linux/OpenToonzPortable".
   - Move the entire OpenToonzPortable folder to a desired location (i.e. application folder, Desktop, a USB key)
   - Double-click the 'OpenToonzPortable/OpenToonz.AppImage' to run
