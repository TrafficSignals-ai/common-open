![TrafficSignals.ai](https://trafficsignals164508950.files.wordpress.com/2021/03/cropped-momenteo-profile.png)

# Artificial Intelligence Platform for Traffic Signals

Revolutionising transport networks utilising artificial intelligence. 

Our TrafficSignals.ai platform is for intelligent transport system integrators, who need advanced traffic signal detection and control, the TrafficSignals.ai platform is a technology framework that significantly reduces the time taken to deliver products and solutions. Unlike building or buying an off the shelf solution, the TrafficSignals.ai allows tight technical integration, full data gathering and control and the necessary regulatory documentation for rapid product and solution realisation.

For more information, visit: [TrafficSignals.ai](http://TrafficSignals.ai)

# Common Library for Open Source

This library contains source code which is derived directly from or uses open source software. 

Note, the instruction for use below are for Ubuntu 20.04 LTS, other platforms may vary.  

## Pre-requisites 

Please install the following libraries. Note, all are required, even if only some or one part of the libraries are used. 

```Bash
sudo apt update
sudo apt upgrade

sudo apt install gcc build-essential cmake pkg-config git

sudo apt install libboost-all-dev # Boost.Asio
```

## Compile

To download and compile to repository from source control, execute the following: 

```Bash
mkdir -p ~/src/TrafficSignals-ai
cd ~/src/TrafficSignals-ai
git clone https://github.com/TrafficSignals-ai/common-open.git
cd common-open
./compile.sh
```

## Source Documentation 

To automatically create source documentation from the comments in the source files execute the following steps from the home directory of the project:

```Bash
sudo apt install doxygen # if not already installed...
cd ~/src/trafficsignals-ai/common-open
doxygen
```
Files will now have been created in the ```/html``` and ```/latex``` directories. ```/html/index.html``` is the home page for the documentation.

Execute the following to bring up the index: 

```Bash
x-www-browser index.html
```

## Libraries Used

### Boost.Asio

Boost.Asio is a cross-platform C++ library for network and low-level I/O programming that provides developers with a consistent asynchronous model using a modern C++ approach. The source code can be found in the /BOOST directory and is derived from the examples provided. 

[Official Page](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html) - [Licence](BOOST/BOOST_LICENSE_1_0.txt) - [Licence FAQ](https://www.boost.org/users/license.html#FAQ) - [Documentation](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/reference.html)

### RapidXml

RapidXml is an attempt to create the fastest XML parser possible, while retaining useability, portability and reasonable W3C compatibility. It is an in-situ parser written in modern C++, with parsing speed approaching that of strlen function executed on the same data.

This implementation uses the licence option 2, the "2. The MIT License" permitted under the [licence agreement options](http://rapidxml.sourceforge.net/license.txt) .

[Official Page](http://rapidxml.sourceforge.net/) - [Licence](rapidxml/licence.txt) - [Documentation](http://rapidxml.sourceforge.net/manual.html)


## Technical Documentation

[Release Notes](ReleaseNotes.md) - [Source Documentation](html/index.html)
