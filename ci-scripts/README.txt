OTX (OpenToonz eXperimental) is based on the current official OpenToonz version 
but has changes, fixes and new features that will either be in the next official 
release or somewhere down the road.

OTX builds are created as portables so no installation is required.  You can 
keep existing installed OpenToonz versions on your system.

Installation and Usage
======================

Windows:

   - Download the file: OpenToonz-OTX-win.zip
   - Right-click the OpenToonz-OTX-win.zip file and select "Extract All..." to
     extract the contents.
        - This will extract a folder called "OpenToonzPortable".
   - Move the OpenToonzPortable folder to a desired location (i.e. C:\Program 
     Files, Desktop, a USB key)
   - Double-click the 'OpenToonzPortable\OpenToonz.exe' to run

macOS:

   - Download the file: OpenToonz-OTX-osx.dmg
   - Double-click the OpenToonz-OTX-osx.dmg file to open it
   - Drag the "OpenToonz" application to wherever you want to run it from (i.e 
     Applications, Desktop, a USB key)
        - If you already have the official OpenToonz application installed, you 
          should rename the OTX version (OpenToonz_OTX) if you want to keep it in
          the same directory as the official version.
   - Double-click the "OpenToonz" application to run.
        - The 1st time you do this, macOS will likely block the application from
          running. You will need to update your Security settings to allow it to
          run.

   NOTE(s): 
      - Do NOT run the OTX application from inside the .dmg file. It will not 
        work.
      - If you run this for the first time on a system that already has 
        OpenToonz officially installed, it will detect the preferences and 
        prompt you 2x to import them, once for preferences and another for room 
        layouts.

        However, due to a Qt bug, not OT, the popups appear behind the splash 
        screen and will make it seem like it's taking forever to load.

        If it seems like it is taking forever to initialize OTX, click away from
        the splash screen to reveal the popups. Click and drag it away from 
        splash screen area before interacting with it.

Linux:

   - Download the file: OpenToonz-OTX-linux.tar.gz
   - Right-click the OpenToonz-OTX-linux.tar.gz file and "Extract Here" to 
     extract the contents.
        - This will create a folder as "OpenToonz-OTX-linux/OpenToonzPortable".
   - Move the entire OpenToonzPortable folder to a desired location (i.e.
     application folder, Desktop, a USB key)
   - Double-click the 'OpenToonzPortable/OpenToonz.AppImage' to run

Upgrading
=========

- Back up your original OTX installation, just in case!
- Copy the OpenToonzPortable (or OpenToonz.app for macOS users) to the same 
  directory as the original installation
- When prompted to replace already existing files, always REPLACE.

Application configuration files that have changed as part of the release will be
updated. Projects/scenes in the portablestuff folder and personal preferences 
and settings will remain unchanged.

CAVEAT: 
   For Windows users, you will not see any changes to Menu or Room configurations
   unless you reset to Default rooms.

Suggestions
===========

In order to always make your projects and scenes available to all 
versions/variants of OpenToonz, I suggest the following:

- Configure all OpenToonz versions to have the same Project Root folder on your 
  machine under My Documents, Desktop or some custom location.
- Always use the common Project Root folder to save all your scenes.  Don't use 
  sandbox or portablesandbox folders.
- If you are on a USB key and want to transfer files from one machine to another
  machine, select a scene from the common projects folder and export it to the 
  portablesandbox folder
   - Conversely, you can do the same to move it from the portablesandbox to a 
     common folder.
