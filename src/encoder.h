#ifndef ENCODER_H
#define ENCODER_H

#include <cstring>
#include <cstdlib>

namespace Encoder {
    /**
     * @brief  Encode the raw data into BMP format
     *         Requires the caller to free the buffer
     */
    char *encodeBmp(size_t &_file_size, char *_data, int _w, int _h, int _stride)
    {
        // https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries

        // the length of each row must be a multiplier of 4, so add padding if necessary
        const int padding = (4 - (_w * 3) % 4) % 4;
        int filesize = 54 + (3 * _w + padding) * _h;

        char *img = (char *)malloc(filesize);

        size_t written_size = 0;
        const auto append = [&](const char *data, int size)
        {
            memcpy(img + written_size, data, size);
            written_size += size;
        };

        char bmpFileHeader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
        char bmpInfoHeader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};
        char bmpPad[3] = {0, 0, 0};

        bmpFileHeader[2] = (char)(filesize);
        bmpFileHeader[3] = (char)(filesize >> 8);
        bmpFileHeader[4] = (char)(filesize >> 16);
        bmpFileHeader[5] = (char)(filesize >> 24);

        bmpInfoHeader[4] = (char)(_w);
        bmpInfoHeader[5] = (char)(_w >> 8);
        bmpInfoHeader[6] = (char)(_w >> 16);
        bmpInfoHeader[7] = (char)(_w >> 24);
        bmpInfoHeader[8] = (char)(_h);
        bmpInfoHeader[9] = (char)(_h >> 8);
        bmpInfoHeader[10] = (char)(_h >> 16);
        bmpInfoHeader[11] = (char)(_h >> 24);

        append(bmpFileHeader, 14);
        append(bmpInfoHeader, 40);

        for (int i = _h - 1; i >= 0; --i)
        {
            append(_data + _stride * i, 3 * _w);
            append(bmpPad, padding);
        }

        _file_size = written_size;
        return img;
    }
}

#endif // ENCODER_H
