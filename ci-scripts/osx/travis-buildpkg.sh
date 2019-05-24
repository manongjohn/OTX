#!/bin/bash
export TOONZDIR=toonz/build/toonz
mkdir -p $TOONZDIR/OTX/OpenToonzPortable folder
cp -r $TOONZDIR/OpenToonz.app $TOONZDIR/OTX/OpenToonzPortable 
cp -r thirdparty/stuff to $TOONZDIR/OTX/OpenToonzPortable/portablestuff
hdiutil create $TOONZDIR/OpenToonz-OTX-osx.dmg -ov -volname OpenToonzPortable -fs HFS+ -srcfolder $TOONZDIR/OTX -format UDZO
