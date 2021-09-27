echo "-----------------------------------------------"
echo "Compiling TrafficSignals.ai Common Open library"
echo "-----------------------------------------------"

phycores=$(echo $sudoPW|cat - /proc/cpuinfo|grep -m 1 "cpu cores"|awk '{print $ 4;}')
echo "-- Cores: $phycores."

rm -rf      /release
mkdir -p    /release/include
cp -r       /include/*                       /release/include
mkdir -p    /release/log
echo "-- Refreshed /release"

rm -rf      /debug
mkdir -p    /debug/include
cp -r       /include/*                       /debug/include
mkdir -p    /debug/log
echo "-- Refreshed /release"


echo --------------------------------
echo "-- Creating MakeFile"
cd /release
cmake ..
echo "-- MakeFile created"

echo --------------------------------
echo "-- Making with $phycores cores."
make -j$num_cpus
echo "-- Compiling TrafficSignals.ai Vision library complete."
