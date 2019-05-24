#!/bin/bash
export TOONZDIR=toonz/build/toonz
echo ">>> Creating folder: $TOONZDIR/OTX/OpenToonzPortable"
mkdir -p $TOONZDIR/OTX/OpenToonzPortable folder
echo ">>> Copying $TOONZDIR/OpenToonz.app to $TOONZDIR/OTX/OpenToonzPortable"
cp -r $TOONZDIR/OpenToonz.app $TOONZDIR/OTX/OpenToonzPortable 
echo ">>> Copying stuff $TOONZDIR/OTX/OpenToonzPortable/portablestuff"
cp -r stuff $TOONZDIR/OTX/OpenToonzPortable/portablestuff
echo ">>> Creating $TOONZDIR/OpenToonz-OTX-osx.dmg"
hdiutil create $TOONZDIR/OpenToonz-OTX-osx.dmg -ov -volname OpenToonzPortable -fs HFS+ -srcfolder $TOONZDIR/OTX -format UDZO
