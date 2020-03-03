# Flags and env variables:
CPPFLAGS=-std=c++17 -Wall
PROJECT=adak

# Note: to build this project, GNU make and g++ must be installed.

# To build a target run 'make <target>' where
# <target> is one of the targets below:

# This is just a sketch-
# feel free to use another build system as you see fit:

# This will run the standard build cycle without debug symbols or ASan
# but with optimization:
build:
	g++ -O3 -o ./bin/$(PROJECT)_build src/*.cpp


# This will compile with debug symbols and ASan:
debug:
	g++ -g -fsanitize=address -o ./bin/$(PROJECT)_debug src/*.cpp

# This will compile the final release, minus any assertions:
release:
	g++ -DNDEBUG -o ./bin/$(PROJECT)_release src/*.cpp

# This will clean up the ./bin folder:
clean:
	rm -f ./bin/$(PROJECT)_{build,debug,release}
