![TrafficSignals.ai](https://trafficsignals164508950.files.wordpress.com/2021/03/cropped-momenteo-profile.png)

# Artificial Intelligence Platform for Traffic Signals

Revolutionising transport networks utilising artificial intelligence. 

Thanks to rapid advances in cloud, big data and analytics, and the falling cost of these new technologies, our traffic signals platform, powered by artificial intelligence, is creating the next wave of disruption, growth and breakthrough innovation across the globe. Our platforms bring together communities of customers and partners, including developers and are enabling new levels of collaboration between companies from different industry sectors. This approach has historically resulted in the conception of entirely new ways of solving problems and delivering services.

Our solution is built on the very foundation that in 5 years’ time we may have a radically different way of gathering traffic flow information. With research projects we want to go further than just integrating with all data, but ask the question; why could a car not tell a signal as it passes what it was like where it has been and what it is like now? The car could also tell the signal where it is going, helping it make a decision about which stage to give priority to. Why could the signal not then tell the car what the traffic is like on its way and how going a different route may be more efficient? This technology is here today and our approach, architecture and technology will mean these advances are simple upgrades not complete changes to the very fundamentals of systems deployed.

TrafficSignals.ai vision is to challenge impossible positions and provide an innovative and deeply integrated approach to intelligence around big data, internet of things and other sources of data. The business strives to be first to market for product innovations, capability and business needs fulfilment. Our focus is on enabling clients to reduce revenue spends whilst increasing quality of service through machine learning and artificial intelligence.

For more informatio, visit: [TrafficSignals.ai](http://TrafficSignals.ai)

# Common Library for Open Source

This library contains source code which is derived directly from open source software. Note, these instructions are for Ubuntu 20.04 LTS, other platforms may vary. 

## Pre-requisites

Please install the following libraries. Note, all are required, even if only some or one part of the libraries are used. 

```Bash
sudo apt update
sudo apt upgrade

sudo apt install libboost-all-dev # Boost.Asio

```

## Compile

To download and compile to repository from source control, execute the following: 

```Bash
mkdir -p ~/src/trafficsignals-ai
cd ~/src/trafficsignals-ai
git clone https://github.com/TrafficSignals-ai/common-open.git
cd common-open
mkdir release && cd release
cmake ..
make -jXX # where XX is the number of available cores 
```

## Source Documentation 

To automatically create source documentation from the comments in the source files execute the following steps from the home directory of the project:

```Bash
sudo apt install doxygen # if not already installed...
cd ~/src/trafficsignals-ai/common-open
doxygen
```
Files will now have been created in the ```/html``` and ```/latex``` directories. ```/html/index.html``` is the home page for the documentation.

## Libraries Used

### Boost.Asio

Boost.Asio is a cross-platform C++ library for network and low-level I/O programming that provides developers with a consistent asynchronous model using a modern C++ approach. The source code can be found in the /BOOST directory and is derived from the examples provided. 

[Official Page](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html) - [Licence](BOOST/BOOST_LICENSE_1_0.txt) - [Licence FAQ](https://www.boost.org/users/license.html#FAQ) - [Documentation](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/reference.html)

## Technical Documentation

[Release Notes](ReleaseNotes.md) - [Source Documentation](html/index.html)
