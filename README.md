# FDC-SDS Serial Disk Server for the Altair FDC+

FDC-SDS is a Linux implentation of the [Serial Drive Communications Protocol](https://deramp.com/downloads/altair/hardware/fdc+/FDC%20Serial%20Server%20Protocol.txt) for the [FDC+ Enhanced Floppy Disk Controller for the Altair 8800](https://deramp.com/fdc_plus.html).

## Installation
```
./configure
make
make install
```

## Baud Rate
The FDC+ serial drive can operate at one of three baud rates: 403.2K, 460.8K, or 230.4K baud. FDC-SDS only supports 230.4K (default) and 460.8K.

If the baud rate is changed in the FDC-SDS server, the corresponding change must also be made on the FDC+. Baud rates are changed in the FDC+ using the monitor.

## Disk Images
Numerous disk image files of original Altair software are available via links on the FDC+ web page. Images files have a `.dsk` extension. You can also recognize disk images by their file size. Eight inch disk images are 330K, Minidisk images are 75K.

The FDC-SDS server is indifferent to whether an eight inch image or Minidisk image is loaded, however, the image type loaded must match drive type selected on the FDC+.

## Example
`fdcsds -p /dev/ttyUSB0 -0 cpm22.dsk`
