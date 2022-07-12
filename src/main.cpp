#include "screenmanager.h"
#include "config.h"
#include "fpng.h"

void saveImage(void *_data, int *rect, int _stride);
void saveBmp(char *_data, int _w, int _h, int _stride);
void savePng(char *_data, int _w, int _h, int _stride);

void saveBmp(char *_data, int _w, int _h, int _stride)
{
    // https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries

    // the length of each row must be a multiplier of 4, so add padding if necessary
    const int padding = (4 - (_w * 3) % 4) % 4;
    int filesize = 54 + (3 * _w + padding) * _h;

    char *img = (char *)std::malloc(filesize);

    int fsize = 0;
    const auto append = [&](char *data, int size) {
        memcpy(img + fsize, data, size);
        fsize += size;
    };

    char bmpFileHeader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    char bmpInfoHeader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    char bmpPad[3] = {0,0,0};

    bmpFileHeader[ 2] = (char)(filesize      );
    bmpFileHeader[ 3] = (char)(filesize >>  8);
    bmpFileHeader[ 4] = (char)(filesize >> 16);
    bmpFileHeader[ 5] = (char)(filesize >> 24);

    bmpInfoHeader[ 4] = (char)(      _w      );
    bmpInfoHeader[ 5] = (char)(      _w >>  8);
    bmpInfoHeader[ 6] = (char)(      _w >> 16);
    bmpInfoHeader[ 7] = (char)(      _w >> 24);
    bmpInfoHeader[ 8] = (char)(      _h      );
    bmpInfoHeader[ 9] = (char)(      _h >>  8);
    bmpInfoHeader[10] = (char)(      _h >> 16);
    bmpInfoHeader[11] = (char)(      _h >> 24);

    append(bmpFileHeader, 14);
    append(bmpInfoHeader, 40);

    for(int i = _h - 1; i >= 0; --i) {
        append(_data + _stride * i, 3 * _w);
        append(bmpPad, padding);
    }

    auto path = Config::instance()->getFileName("bmp");  
    FILE *f = fopen(path.c_str(), "wb");
    if (!f) {
        printf("Failed opening file: %s\n", path.c_str());
        return;
    }
    fwrite(img, fsize, 1, f);
    fclose(f);

    std::free(img);
    
    return;
}

void savePng(char *_data, int _w, int _h, int _stride)
{
    char *img = (char *)std::malloc(3  * _w * _h);

    for (int y = 0; y < _h; ++y) {
        memcpy(img + y * _w * 3, _data + y * _stride, _w * 3);
    }

    auto path = Config::instance()->getFileName("png");  

    if (!fpng::fpng_encode_image_to_file(path.c_str(), img, _w, _h, 3)) {
        printf("Error encoding to file\n");
    }

    std::free(img);
}

void saveImage(void *_data, int *rect, int _stride)
{
    int x = rect[0];
    int y = rect[1];
    int w = rect[2];
    int h = rect[3];

    if (Config::instance()->getImageType() == Config::ImageType::PNG) {
        savePng((char *)_data, w, h, _stride);
    } else {
        saveBmp((char *)_data, w, h, _stride);
    }
}

int main(int argc, char *argv[])
{
    const auto config = Config::instance();
    config->parseParameters(argc, argv);

    fpng::fpng_init();
    Screen screen;

    screen.setCaptureCallback(saveImage);

    screen.captureScreen(
        config->getDisplayIndex(),
        config->getX(), config->getY(),
        config->getW(), config->getH());

    return 0;
}
