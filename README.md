# ecm

## Overview
This is a simple command-line tool adhering to GNU interface standards that implements Neill Corlett's Error Code Modeler (ECM) codec. The main use for this is decoding `.bin.ecm` files you may find on the internet, or, more rarely, creating them yourself. Such files arise usually in the context of CD-ROM files to be used with legacy system emulator software. A `.bin` file is a direct binary sector-by-sector copy of the raw data from a CD-ROM, which includes the error correction code (ECC). As such, there is significant useless redundant data that is both unneeded outside of the context of an optical disk as well as very hard to compress (as it is essentially random). The ECM codec removes this ECC data (as well as a few other bits of overhead), which not only shrinks the data a bit on its own, but more importantly makes it much more amenable to compression by other means. In the spirit of UNIX tools, this simple utility only handles the ECM algorithm, and is designed to be piped to another program that will handle the compression.

The command line tool from Neill Corlett is very hard to find and no longer packaged by most Linux distributions, so I figured I'd spend a couple hours as make a nice GNU-compliant package of it all, and perhaps make it available as a snap.

## Usage
This utility uses similar command line arguments as `gzip`. For example, to decode an ecm file use the following syntax:
```
ecm -d filename.bin.ecm > filename.bin
```

## Building
Use the standard autoconf procedure for compiling:
```sh
$ autoreconf -i
$ ./configure
$ make
```

## FAQ

### Is this useful for other files?
No. If the file isn't a sector-by-sector copy of an optical disk, it isn't a standard optical drive ECC scheme and will result in no change to the file. You'll never lose data using this utility, but you won't gain anything unless the data fits the CD-ROM standard.

### Can I use this on ISO files that come from a CD-ROM or DVD?
Nope. An ISO file already has the redundant ECC and other overhead removed and just represents the file system data itself, essentially achieving what this program provides on `.bin` files.

### If ISO files perform a similar function, what is the point of a BIN file?
All I know is some emulators use them. I can only surmise that some older systems dealt with the raw data from an optical drive, and didn't have a driver to handle the error correction and translate into a proper file system, and thus the emulator must do the same.
