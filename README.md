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

* split\_blocks: Program to de-interleave image file into separate files block-wise.
Used for example to de-interleave main (user) and spare (out-of-band) area of NAND
image, on page-per-page basis, or to reorder pages/blocks by rejoining with different
block size later.

* join\_blocks: Program to interleave images block-wise, the opposite of the earlier.
Together with split\_blocks can be used to reorder pages based on physical-to-logical
mapping of a controller, for example to group paired pages of MLC flash together, 
perform block rotation by splitting and rejoining in different order, or joining images
from multiple Flash chips.

* filter\_blocks: As the name implies, this program filters input blocks based on
conditions specified on command line. This is used to selectively output only those blocks,
which were mapped to a filesystem and actually containing user data, or to remove known
bad blocks from the image.

* rotate\_blocks: Some Flash controllers do not start filling an eraseblock on logical boundary,
but start with an offset, starting writing with logical page number other than zero.
Rotation operation restores logical ordering in erase block by moving pages with logical page number 0
to the beginning of the block, by the amount of pages in Logical Page Number, modulo page count per
erase block. Afterwards pages are arranged with Logical Page Number equal to Physical Page Number. 

## License

Code is licensed under GNU General Public License 2.0
