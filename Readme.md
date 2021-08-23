![TrafficSignals.ai](resources/TrafficSignals-ai.png)

# Common Library for Open Source

This library contains source code which is derived directly from open source software. 

No application logic should be applied to this module. 

## Libraries Used

### Boost.Asio

Boost.Asio is a cross-platform C++ library for network and low-level I/O programming that provides developers with a consistent asynchronous model using a modern C++ approach. The source code can be found in the /BOOST directory and is derived from the examples provided. 

[Official Page](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html) - [Licence](BOOST/BOOST_LICENSE_1_0.txt) - [Licence FAQ](https://www.boost.org/users/license.html#FAQ) - [Documentation](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/reference.html)

### Install Pre-requisites

Please install the following libraries. Note, all are required, even if only some or one part of the libraries are used. 

```Bash
sudo apt update
sudo apt upgrade

sudo apt install libboost-all-dev # Boost.Asio

```

### Install

Download the repository from source control and navigate to the directory. Then execute the following: 

```Bash
mkdir release && cd release
cmake ..
make -jXX # where XX is the number of available cores 
```

### Source Documentation 

To automatically create source documentation from the comments in the source files complete the following steps from the home directory of the project:

```Bash
sudo apt install doxygen # if not already installed...
doxygen
```

Files will now have been created in the ```/html``` and ```/latex``` directories. ```/html/index.html``` is the home page for the documentation.


## Road Map

### v0.1 Now
 
### v0.2 Next

### v0.3 Future

## Technical Documentation

[Release Notes](ReleaseNotes.md) - [Source Documentation](html/index.html)
