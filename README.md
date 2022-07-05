# Better screenshot (`betterss`)

This project works as a replacement for QNX's `screenshot`

The `screenshot` command from QNX is slow & lacks of some features

## Feature

| Feature                              | screenshot | betterss |
|--------------------------------------|------------|----------|
| Capture any area of the screen       |  &check;   |  &cross; |
| Full screen capture time (1920x1080) | ~1.47s     |  ~0.16s  |

## Usage

Full screen capture:
```shell
./betterss
```

Capture any area:
```shell
./betterss x=100 y=100 w=200 h=200
```
`x y w h` are optional

The images are saved under `/var/tmp`

If you want to save to another location:
```shell
./betterss file=/var/screenshot/image001
```

The file extension `.bmp` will be concatenated

Note: for custom locations, the path must exists (I didn't do the `mkdir`)

# Build

First, run the `proteus_build_config.sh` script to setup enviroment (compiler,...)

Then `cd` to source code directory:
```shell
cmake .
make
```