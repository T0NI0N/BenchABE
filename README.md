# Benchabe

Simple program for benchmarking the post-quantum CP-ABE scheme under given parameters.

Project made for computer science degree thesis.

## Requirements
- First, you need a C++ compiler with OMP support, cmake, make, and autoconf. 
On Ubuntu (os where this project has been developed and successfully tested) you can install them with the following commands: 
```
sudo apt-get install build-essential 
sudo apt-get install cmake
sudo apt-get install autoconf 
```
- [Palisade Development](https://gitlab.com/palisade/palisade-development)
- [Palisade ABE](https://gitlab.com/palisade/palisade-abe)
- [Cxxopts](https://github.com/jarro2783/cxxopts)

## Installation
First, be sure to have installed all the required dependencies.

1. Download or clone the repository (`git clone <link-to-this-repository>`, requires git) on your computer.
2. Navigate to this project directory and then create the build directory: `mkdir build`, `cd build`.
3. Run the following command: `cmake ..` and `make`.

Now you can use the program, for example: `./benchabe` will start using the default values.

## Usage
It is possible to modify the following parameters:
- Size of the ring (default: 1024)
- Number of attributes to use (default: 6) 
- Base of the lattice (default: 64) 
- Number of iterations to use for testing (default: 100) 
- Keygen phase (default: single phase) 

Furthermore, it is also possible to activate the verbose output (it will show the results for all the iterations), when it is disabled (default) only the average time will be displayed.

Running `./benchabe -h` or  `./benchabe --help` will show all the possible options.

Running only `./benchabe` will start the program using all the default values.

For example, `./benchabe -a 15 -i 40 -v` will start the program using 15 attributes, 40 iterations and enabling the verbose output.

For example, `./benchabe -r 2048 -b 128 -o` will start the tests using 2048 for the size of the ring, 128 for the lattice's base and enabling the keygen offline / online phase.
