#!/bin/bash
export TOONZDIR=toonz/build/toonz
echo ">>> Creating folder: $TOONZDIR/OTX"
mkdir $TOONZDIR/OTX folder
echo ">>> Copying $TOONZDIR/OpenToonz.app to $TOONZDIR/OTX"
cp -r $TOONZDIR/OpenToonz.app $TOONZDIR/OTX 
echo ">>> Copying stuff $TOONZDIR/OTX/OpenToonz.app/portablestuff"
cp -r stuff $TOONZDIR/OTX/OpenToonz.app/portablestuff
echo ">>> Creating $TOONZDIR/OpenToonz-OTX-osx.dmg"
hdiutil create $TOONZDIR/OpenToonz-OTX-osx.dmg -ov -volname OpenToonzPortable -fs HFS+ -srcfolder $TOONZDIR/OTX -format UDZO
