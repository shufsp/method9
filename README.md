# method9
Not for public use. Barely works, not tested

## Minecraft mass server crawler
Scan neighboring IP addresses from a given starting point with any given range. Fast and multi-threaded. Only scans port 25565 for now.

## Minecraft mass server list adder (server.dat)
Within the same repo I've also included a server.dat creator, which takes a .txt list of IPS (one IP per line) and turns it into a valid NBT file that minecraft can read.
You can then replace your servers.dat file with the generated .dat file in your .minecraft folder. It's fast. You can add tens of thousands of servers to your minecraft server list if you wanted. 

## Build instructions
This project uses `cmake` to build. Before running cmake, make sure you've installed the required dependencies for this project:
- boost (v1.83.0)
- fmt (v10.1.0)
  
**Linux:** Install cmake, boost, and fmt using your distro's package manager.
  
**Windows:** Load the cmake project with Visual Studio and use a package manager like vcpkg to install these dependencies. You can also bypass cmake or visual studio and compile directly with MinGW, although that is up to you to figure out since I don't use Windows.
<br><br/>

Clone this repo and then execute the following commands inside the directory:
```shell
mkdir build && cd build
cmake ..
make
```

You can also run the included `build.py` script with python to do the exact same as above, but automatically.
```shell
python build.py
```
## Credit
Thanks to ToKiNoBug's [NBT writer utility](https://github.com/ToKiNoBug/NBTWriter-of-Toki), we could easily implement the server.dat creator tool. Seriously, all of the other C++ NBT libraries I tried were not as straightforward to use as this one.
