# inodeyou-c
inodeyou-c is a userspace C program adapted from [inodeyou](https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py) that uses a simple cross-based check to detect inodes that are hidden by Linux rootkit(s). 

inodeyou-c uses The Sleuth Kit (TSK) C library to compare the view of the filesystem inodes by two methods:
1) Finding inodes visible to the disk's view of the file system (with read()) via TSK library
2) Finding inodes visible from the user's view of the mounted file system (with getdent(), stat(), etc.).

With the results from both methods, we can cross-check to see if there are any inodes detected by one method and not by the other. If so, those inodes might be hidden by a rootkit/malware. 

Adapted from inodeyou:
- http://www.unixist.com/security/detecting-hidden-files/index.html (inodeyou documentation)
- https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py (inodeyou repo)

## Features
- Detects regular inodes hidden by working rootkits such as Diamorphine, Nurupo, LilyOfTheValley, Nuk3gh0st, and Reptile. (Rootkits that work on kernel 4.x)
- Works well on directories such as /home and /etc
- Tested on Ubuntu 16.04 LTS (64-bit)

### Limitations
- Buggy on some directories like /run and /lib64
- False positive of recently deleted inodes show due to tsk memory leaks
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
The example command scans for hidden inodes on /home/user1, whereby the /home/user1 is on the root directory mounted on /dev/sda1
```
Usage: sudo ./inodeyou-c volume mountpoint [directory] 
Example: ./inodeyou-c /dev/sda1 / /home/user1
```
