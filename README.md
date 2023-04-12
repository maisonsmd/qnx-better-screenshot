# Better screenshot (`betterss`)

This project works as a replacement for QNX's `screenshot`

The [`screenshot`](https://www.qnx.com/developers/docs/7.0.0/com.qnx.doc.screen/topic/manual/screenshot.html) command from QNX is slow & lacks of some features

## Feature

| Feature                              | [`screenshot`](https://www.qnx.com/developers/docs/7.0.0/com.qnx.doc.screen/topic/manual/screenshot.html) | `betterss` |
|--------------------------------------|------------|----------|
| Capture any display                  |  ✅   |  ✅ |
| Capture any area of the screen       |  ❌   |  ✅ |
| Support PNG                          |  ❌   |  ✅ |

## Performance
|                                      | [`screenshot`](https://www.qnx.com/developers/docs/7.0.0/com.qnx.doc.screen/topic/manual/screenshot.html) | `betterss` |
|--------------------------------------|------------|----------|
| Full screen capture time (1920x1080) | ~1.47s     |  **~0.10s** (14.7 times faster)  |

## Usage

Full screen capture:
```shell
./betterss
```

### Capture any area:

```shell
./betterss x=100 y=100 w=200 h=200
```
`x y w h` are optional

The images are saved under `/var/tmp`

### Specify output path

```shell
./betterss file=/var/screenshot/image_name
```

The file extension `.bmp` will be concatenated

Note: for custom locations, the path must exists (I didn't do the `mkdir`)

### Export as PNG

```shell
./betterss type=png
```
Encoder: [`fpng`](https://github.com/richgel999/fpng)

### Capture another display

`betterss` allow capturing any display attached (like Cluster virtual display, lower blade,...)

```shell
./betterss display=index
```
- `index`: the index of the display, might be `0`, `1`, `2`,...

You have to try each index to find the desired display

## Build

Firstly, setup your build enviroment (QNX compiler,...)

Then `cd` to project directory:
```shell
cmake .
make
```
