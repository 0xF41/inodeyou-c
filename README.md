# inodeyou-c
inodeyou-c is a userspace C program adapted from [inodeyou](https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py) that uses a simple cross-based check to detect inodes that are hidden by Linux rootkit(s). 

inodeyou-c uses The Sleuth Kit (TSK) C library to compare the view of the filesystem inodes by two methods:
1) Finding inodes visible to the disk's view of the file system (with read()) 
2) Finding inodes visible from the user's view of the mounted file system (with getdent(), stat(), etc.).

With the results from both methods, we can cross-check to see if there are any inodes detected by one method and not by the other. If so, those inodes are highly likely to be hidden by a rootkit that is currently hooking the syscalls used in the 'ls' command, ommiting entries of files the rootkit is configured to hide. 

Adapted from inodeyou:
- http://www.unixist.com/security/detecting-hidden-files/index.html (inodeyou documentation)
- https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py (inodeyou repo)

## Features
- Detects regular inodes hidden by working rootkits such as Diamorphine, Nurupo, LilyOfTheValley, Nuk3gh0st, and Reptile. (Rootkits that work on kernel 4.x)
- Tested on Ubuntu 16.04 LTS (64-bit)

### Limitations
- Does not have a recursive functionality in sub-directories (//TODO in future updates)
- Does not detect special files such as file sockets, symbolic links, etc. 
- Only works on ext file system types, not compatible with xfs and other types. 


## Installation
For debian-based distros: 
```
sudo apt update
sudo apt install gcc
sudo apt install libtsk-dev
sudo git clone https://github.com/fy-fy-fy/inodeyou-c.git
cd ./inodeyou-c/
sudo make
```

## Usage
```
Usage: sudo ./inodeyou-c volume mountpoint [directory] (./inodeyou-c /dev/sda1 / /)
```
