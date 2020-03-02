#!/bin/bash
export QTDIR=/usr/local/opt/qt
export TOONZDIR=toonz/build/toonz

echo ">>> Copying stuff to $TOONZDIR/OpenToonz.app/portablestuff"
cp -R stuff $TOONZDIR/OpenToonz.app/portablestuff

echo ">>> Creating OpenToonz-OTX-osx.dmg"

$QTDIR/bin/macdeployqt $TOONZDIR/OpenToonz.app -dmg -verbose=0 -always-overwrite \
   -executable=$TOONZDIR/OpenToonz.app/Contents/MacOS/lzocompress \
   -executable=$TOONZDIR/OpenToonz.app/Contents/MacOS/lzodecompress \
   -executable=$TOONZDIR/OpenToonz.app/Contents/MacOS/tcleanup \
   -executable=$TOONZDIR/OpenToonz.app/Contents/MacOS/tcomposer \
   -executable=$TOONZDIR/OpenToonz.app/Contents/MacOS/tconverter \
   -executable=$TOONZDIR/OpenToonz.app/Contents/MacOS/tfarmcontroller \
   -executable=$TOONZDIR/OpenToonz.app/Contents/MacOS/tfarmserver 
   
mv $TOONZDIR/OpenToonz.dmg $TOONZDIR/../OpenToonz-OTX-osx.dmg
