# method9
Not for public use. 
Barely works, not tested for stability or functionality.
For educational purposes only. **Do not use the scanner on public IPs.** Only use the scanner on your local machine.

## Minecraft server scanner
Given an IP, find minecraft servers from neighboring octets. 

## Minecraft mass server list adder (server.dat)
Within the same repo I've also included a server.dat creator, which takes a .txt list of IPS (one IP per line) and turns it into a valid NBT file that minecraft can read.
You can then replace your servers.dat file with the generated .dat file in your .minecraft folder. It's fast. You can add tens of thousands of servers to your minecraft server list if you wanted. 

## Server Crawler Usage
### Arguments
```
  --help                                        produce help message
  --s [ip]                                      the IP address that we start scanning from. required.
  --threads [thread_count]                      number of threads. optional. default = 50
  --timeout-read [milliseconds]                 read timeout in milliseconds. optional. default = 1000ms (1s)
  --timeout-connection [milliseconds]           connection timeout in milliseconds. optional. default = 1000ms (1s)
  --ports [port1,port2,...]                     comma-separated list of ports. optional. default is 25565
  --distance [number]                           the amount of adjacent neighbors to visit. higher = further ips scanned. optional. default is 3
  --crawl-direction [forward,backward,both]     the crawl mode. options for this are: forward, backward, both. optional. default is forward.
  --verbose-results                             display all the valid servers with their MOTD and player count after scanning.
```

### Examples
You can scan using default settings by supplying the starting ip address with the `--s` flag.
This scans forward neighbors within 3 blocks on 50 threads.
```shell
server-crawler --s 1.2.3.4
```

If you want to scan on multiple ports, you can do so by providing a comma separated list of ports after the `--ports` flag.
```shell
server-crawler --s 1.2.3.4 --ports=25565,25566,25567,25568
```

If the scan is going too slow, you can allocate more threads to the scanner using the ``--threads`` flag.
A good thread count I would recommend to start with is `100`.
NOTE: Choose this value wisely. I am not responsible for any damage caused to your computer.
```shell
server-crawler --s 1.2.3.4 --threads 100
```

By default, the server crawler will only visit neighbors in one direction (ascending). You can change this by using the `--crawl-direction` flag
Crawl direction is `forward` by default. If you want to traverse IP neighbors behind you, you can use the `backward` or `both` options. 
```shell
server-crawler --s 1.2.3.4 --crawl-direction backward
```

### 
## Build instructions
This project uses `cmake` to build. Before running cmake, make sure you've installed the required dependencies for this project:
- boost (v1.83.0) components: system, regex, program_options
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

## Credit
Thanks to ToKiNoBug's [NBT writer utility](https://github.com/ToKiNoBug/NBTWriter-of-Toki), we could easily implement the server.dat creator tool. Seriously, all the other C++ NBT libraries I tried were not as straightforward to use as this one. I'm not kidding.
