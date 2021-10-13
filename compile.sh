echo "-----------------------------------------------"
echo "Compiling TrafficSignals.ai Common Open library"
echo "-----------------------------------------------"
echo "-- Cores: $nproc."

rm -rf      release
mkdir -p    release/include
cp -r       include/*                       release/include
mkdir -p    release/log
echo "-- Refreshed /release"

rm -rf      debug
mkdir -p    debug/include
cp -r       include/*                       debug/include
mkdir -p    debug/log
echo "-- Refreshed /debug"


echo --------------------------------
echo "-- Creating MakeFile"
cd release
cmake ..
echo "-- MakeFile created"

echo --------------------------------
echo "-- Making with $nproc proceses."
make -j$nproc
echo "-- Compiling TrafficSignals.ai Vision library complete."
