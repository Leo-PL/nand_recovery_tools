# NAND recovery tools

## Introduction
This repository contains a few simple utilities that help in reconstruction
of block device image from images of individual NAND Flash memory chips.

They are implemented as separate programs with UNIX philosophy in mind,
mainly for easy scriptability and automation of recovery of particular
storage device.

## Implemented tools
* majority\_edac: program to reconstuct correct NAND image from multiple
dumps containing bitflips, by means of
[majority-vote error correction](https://en.wikipedia.org/wiki/Majority_logic_decoding)

* invert: program to invert the image bitwise, replacing ones with zeros
and vice versa. Useful to undo inversion performed by certain Flash controllers.

## License

Code is licensed under GNU General Public License 2.0
