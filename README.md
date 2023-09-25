# method9
Not for public use. Barely works, not tested

## Minecraft mass server crawler
Scan neighboring IP addresses from a given starting point with any given range. Fast and multi-threaded. Only scans port 25565 for now.

## Minecraft mass server list adder (server.dat)
Within the same repo I've also included a server.dat creator, which takes a .txt list of IPS (one IP per line) and turns it into a valid NBT file that minecraft can read.
You can then replace your servers.dat file with the generated .dat file in your .minecraft folder. It's fast. You can add tens of thousands of servers to your minecraft server list if you wanted. 

## Build instructions
This project uses `cmake` to build.
To build this project from source, clone this repo and then execute the following commands inside the directory:
```shell
mkdir build && cd build
cmake ..
make
```

You can also run the included `build.py` script with python to do the exact same as above, but automatically.
```shell
python build.py
```