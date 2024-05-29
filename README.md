# ecm

## Overview
This is a simple command-line tool that implements Neill Corlett's Error Code Modeler (ECM) codec. The main use for this is decoding `.ecm` files you may find on the internet, or, more rarely, creating them yourself. Such files arise in the context of CD-ROM files to be used with legacy system emulator software. A `.bin` file is a direct binary sector-by-sector copy of the raw data from a CD-ROM, which includes the error correction code (ECC). As such, there is significant useless redundant data that is both unneeded outside of the context of an optical disk as well as very hard to compress (as it is essentially random). The ECM codec removes this ECC data (as well as a few other bits of overhead), which not only shrinks the data a bit on its own, but more importantly makes it much more amenable to compression by other means. In the spirit of UNIX tools, this simple utility only handles the ECM algorithm, and is designed to be piped to another program that will handle the compression.

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
An ISO can lose information relevent to the CD-ROM, especially when the ECC has been tampered with for various reasons, or if the CD-ROM is multi-session. Here’s a detailed comparison of why one might use BIN/CUE over ISO:
#### ISO Files:
- **Efficient and Simple**: ISO files provide an efficient and straightforward way to image data discs, capturing the entire filesystem and user data in a single file.
- **Widespread Compatibility**: ISO files are widely supported by operating systems and software, making them easy to mount, extract, or burn.
- **Use Case**: Ideal for standard data CDs, DVDs, and Blu-ray discs where the primary need is to replicate the filesystem and data content.
#### BIN/CUE Files:
- **Raw Sector Data**: BIN files capture a raw sector-by-sector copy of the disc, including all data from each sector. This can include additional information like subchannel data and possibly ECC, which is important for certain types of discs.
- **Multiple Tracks and Sessions**: The CUE file provides a detailed description of the disc’s layout, including track boundaries, session information, and other structural details. This is particularly important for audio CDs, mixed-mode discs, and discs with special formats that ISO cannot fully encapsulate.
- **Precision**: BIN/CUE files can precisely replicate the original disc’s structure, making them suitable for archival purposes and for creating exact duplicates of discs that have complex layouts or non-standard formats.
- **Use Case**: Essential for audio CDs, mixed-mode CDs, discs with multiple sessions, or other special formats where capturing all low-level details of the disc is necessary.
#### Key Reasons to Use BIN/CUE over ISO:
1. **Multiple Tracks and Mixed Media**: Audio CDs and mixed-mode CDs often contain multiple tracks and require the detailed structural information that BIN/CUE provides.
2. **Subchannel Data**: Certain types of discs, especially older formats, may rely on subchannel data for proper playback or functionality. BIN/CUE can capture this data, while ISO cannot.
3. **Exact Duplication**: For archival purposes or when creating an exact duplicate of a disc with non-standard formatting, BIN/CUE is the preferred format due to its ability to replicate all aspects of the original disc.
4. **Special Formats**: Some software, games, or multimedia discs use non-standard formats that require the detailed sector-level information captured by BIN/CUE.
#### Summary:
- **ISO**: Best for straightforward data disc imaging, providing simplicity, efficiency, and broad compatibility.
- **BIN/CUE**: Best for complex disc formats requiring detailed replication of the disc’s structure and data, including multiple tracks, subchannel information, and special formats.

The choice between the two formats depends on the specific requirements of the disc being imaged and the intended use of the image file.
