echo "-----------------------------------------------"
echo "Compiling TrafficSignals.ai Common Open library"
echo "-----------------------------------------------"

phycores=$(echo $sudoPW|cat - /proc/cpuinfo|grep -m 1 "cpu cores"|awk '{print $ 4;}')
echo "-- Cores: $phycores."

rm -rf      ~/src/TrafficSignals-ai/common-open/release
mkdir -p    ~/src/TrafficSignals-ai/common-open/release/include
cp -r       ~/src/TrafficSignals-ai/common-open/include/*                       ~/src/TrafficSignals-ai/common-open/release/include
mkdir -p    ~/src/TrafficSignals-ai/common-open/release/log
echo "-- Refreshed /release"

rm -rf      ~/src/TrafficSignals-ai/common-open/debug
mkdir -p    ~/src/TrafficSignals-ai/common-open/debug/include
cp -r       ~/src/TrafficSignals-ai/common-open/include/*                       ~/src/TrafficSignals-ai/common-open/debug/include
mkdir -p    ~/src/TrafficSignals-ai/common-open/debug/log
echo "-- Refreshed /release"


echo --------------------------------
echo "-- Creating MakeFile"
cd ~/src/TrafficSignals-ai/common-open/release
cmake ..
echo "-- MakeFile created"

echo --------------------------------
echo "-- Making with $phycores cores."
make -j$num_cpus
echo "-- Compiling TrafficSignals.ai Vision library complete."
