# inodeyou-c
inodeyou-c is a userspace C program adapted from [inodeyou](https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py) that uses a simple cross-based check to detect inodes that are hidden by Linux rootkit(s) or malware(s). 

inodeyou-c uses The Sleuth Kit (TSK) C library to compare the view of the filesystem inodes by two methods:
1) Finding inodes visible to the disk's view of the file system (with read()) via TSK library
2) Finding inodes visible from the user's view of the mounted file system (with getdent(), stat(), etc.).

With the results from both methods, we can cross-check to see if there are any inodes detected by one method and not by the other. If so, those inodes might be hidden by a rootkit/malware. 

Adapted from inodeyou:
- http://www.unixist.com/security/detecting-hidden-files/index.html (inodeyou documentation)
- https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py (inodeyou repo)

**Note: This program is made for experiemental purposes and may produce a large number of false positives. Results given by this program may NOT be 100% representative**

## Features
- Detects regular inodes hidden by rootkits ([Diamorphine](https://github.com/m0nad/Diamorphine), [Nurupo](https://github.com/nurupo/rootkit), [LilyOfTheValley](https://github.com/En14c/LilyOfTheValley), [Nuk3gh0st](https://github.com/ropch4ins/Nuk3Gh0st), [Reptile](https://github.com/f0rb1dd3n/Reptile))
- Works well on directories such as /home and /etc
- Tested on Ubuntu 16.04 LTS (64-bit)

### Limitations
- Buggy on some directories like /run and /lib64 which creates false positives
- Some TSK functions have memory leaks which may create unexpected results
- Does not detect special files such as file sockets, symbolic links, etc. 
- Only works on ext2/3/4 file system types, not compatible with xfs and other types. 


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
The example command scans for hidden inodes from /home/user1 recursively, whereby /home/user1 is on the root directory mountpoint on the /dev/sda1 volume
```
Usage: sudo ./inodeyou-c volume mountpoint [directory] 
Example: ./inodeyou-c /dev/sda1 / /home/user1
```
