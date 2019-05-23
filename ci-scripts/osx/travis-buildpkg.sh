#!/bin/bash
# We assume we're in opentoonz/toonz/sources/toonz
mkdir ./OTX/OpenToonzPortable folder
cp -r ./OpenToonz.app ./OTX/OpenToonzPortable 
cp -r ../../../thirdparty/stuff to ./OTX/OpenToonzPortable/portablestuff
hdiutil create ./OpenToonz-OTX-osx.dmg -ov -volname OpenToonzPortable -fs HFS+ -scrfolder ./OTX -format UDZO
