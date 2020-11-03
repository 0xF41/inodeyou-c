# inodeyou-c
inodeyou-c is a userspace C program adapted from [inodeyou](https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py) that uses a simple cross-based check to detect inodes that are hidden by User-space and Kernel-space Linux rootkit(s). 

inodeyou-c uses The Sleuth Kit (TSK) C library to compare the view of the filesystem inodes via the read() syscall, and from the getdent() and stat() syscall. 
If an inode is discovered by the read() syscall and not from getdent() and stat(), it is likely that either the read(), getdent(), or stat() syscall is hooked by the rootkit, so some entries are hidden by the "ls" command.

Adapted from inodeyou:
- http://www.unixist.com/security/detecting-hidden-files/index.html (inodeyou documentation)
- https://bitbucket.org/unixist/inodeyou/src/master/inodeyou.py (inodeyou repo)

### Features
- // TODO
- Detects inodes hidden by working rootkits such as Diamorphine, Nurupo, LilyOfTheValley, Nuk3gh0st, and Reptile. If a rootkit is buggy in hiding files this script may not work. 

Usage:
```
// TODO
```

### Dependencies
- // TODO

For debian-based distros: 
```
// TODO
```

### Assumptions
- // TODO
