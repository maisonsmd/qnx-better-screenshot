#ifndef ENCODER_H
#define ENCODER_H

#include <cstring>
#include <cstdlib>

namespace Encoder {
    /**
     * @brief  Encode the raw data into BMP format
     *          Requires the caller to free the buffer
     */
    char *encodeBmp(size_t &_file_size, const char *_data, int _w, int _h, int _stride);
}

#endif // ENCODER_H
