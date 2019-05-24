#!/bin/bash
# We assume we're in opentoonz/toonz/sources/toonz
export TOONZDIR=toonz/build/toonz
mkdir -p $TOONZDIR/OTX/OpenToonzPortable folder
cp -r $TOONZDIR/OpenToonz.app $T9ONZDIR/OTX/OpenToonzPortable 
cp -r thirdparty/stuff to $TOONZDIR/OTX/OpenToonzPortable/portablestuff
hdiutil create $TOONZDIR/OpenToonz-OTX-osx.dmg -ov -volname OpenToonzPortable -fs HFS+ -srcfolder $TOONZ/OTX -format UDZO
