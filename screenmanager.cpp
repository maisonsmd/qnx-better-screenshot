#include "screenmanager.h"

#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>

#include "config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

class Rectangle {
public:
    Rectangle() : m_x(0), m_y(0), m_w(0), m_h(0) {}
    Rectangle(int _x, int _y, int _w, int _h) : m_x(_x), m_y(_y), m_w(_w), m_h(_h) { }

    int left() const { return m_x; }
    int right() const { return m_x + m_w; }

    int top() const { return m_y; }
    int bottom() const { return m_y + m_h; }

    int width() const { return m_w; }
    int height() const { return m_h; }

    bool isValid() const {
        return left() >= 0 && top() >= 0 && width() > 0 && height() > 0;
    }

    Rectangle getIntersection(const Rectangle &_rhs) {
        Rectangle result {};

        // not intersect horizontally
        if (right() < _rhs.left() || left() > _rhs.right())
            return result;

        // not intersect vertically
        if (bottom() < _rhs.top() || top() > _rhs.bottom())
            return result;

        result.m_x = std::max(left(), _rhs.left());
        result.m_w = std::min(right(), _rhs.right()) - result.left();

        result.m_y = std::max(top(), _rhs.top());
        result.m_h = std::min(bottom(), _rhs.bottom()) - result.top();

        return result;
    }

    std::string toString() const {
        std::stringstream ss;

        ss << "("
           << top() << ","
           << left() << " "
           << width() << "*"
           << height() << ")";

        return ss.str();
    }

private:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
};

Screen::Screen()
{
    if (!createScreenContext()) {
        return;
    }

    if (!findDisplays()) {
        return;
    }
}

Screen::~Screen() {
    screen_destroy_context(m_context);
}

std::vector<screen_display_t> Screen::displays() const
{
    return m_displayList;
}

int Screen::displayCount() const
{
    return m_displayList.size();
}

Screen::Size Screen::displayResolution(screen_display_t _display) const
{
    int size[2] {0};
    int rotation = displayRotation(_display);

    Size result {};

    if (screen_get_display_property_iv(_display, SCREEN_PROPERTY_NATIVE_RESOLUTION, size) != 0) {
        printf("Failed to get display rotation, errno: %d(%s)\n", errno, strerror(errno));
        return result;
    }

    if (rotation == Rotation::SCREEN_ROTATION_90
            || rotation == Rotation::SCREEN_ROTATION_270) {
        result.w = size[1];
        result.h = size[0];
    } else {
        result.w = size[0];
        result.h = size[1];
    }

    return result;
}

Screen::Rotation Screen::displayRotation(screen_display_t _display) const
{
    int rotation {SCREEN_ROTATION_NONE};
    if (screen_get_display_property_iv(_display, SCREEN_PROPERTY_ROTATION, &rotation) != 0) {
        printf("Failed to get display rotation, errno: %d(%s)\n", errno, strerror(errno));
        return SCREEN_ROTATION_NONE;
    }
    return (Rotation)rotation;
}

void Screen::captureScreen(int _displayIndex, int _x, int _y, int _w, int _h)
{
    if (displayCount() == 0) {
        printf("No display found!\n");
        return;
    }

    if (_displayIndex < 0 || _displayIndex >= displayCount()) {
        printf("displayIndex out of range\n");
        return;
    }

    auto display = m_displayList[_displayIndex];
    auto size = displayResolution(display);

    if (_w <= 0 || _w > size.w)
        _w = size.w;

    if (_h <= 0 || _h > size.h)
        _h = size.h;

    // get the intersect rectangle between requested rectangle and the screen
    Rectangle requestRect {_x, _y, _w, _h};
    Rectangle screen {0, 0, size.w, size.h};
    Rectangle intersection = requestRect.getIntersection(screen);

    printf("%s\n", intersection.toString().c_str());

    if (!intersection.isValid()) {
        printf("Requested rectangle out side the screen\n");
        return;
    }

    screen_pixmap_t pixmap;
    if (screen_create_pixmap(&pixmap, m_context) != 0) {
        printf("Failed creating pixmap\n");
        return;
    }

    screen_buffer_t buffer;

    void *pointer;
    int stride; // bytes per line
    const int rect[4] = {intersection.left(), intersection.top(), intersection.width(), intersection.height()};

    int val = SCREEN_USAGE_READ | SCREEN_USAGE_NATIVE;
    screen_set_pixmap_property_iv(pixmap, SCREEN_PROPERTY_USAGE, &val);

    val = SCREEN_FORMAT_RGB888;
    screen_set_pixmap_property_iv(pixmap, SCREEN_PROPERTY_FORMAT, &val);

    int err =    screen_set_pixmap_property_iv(pixmap, SCREEN_PROPERTY_BUFFER_SIZE, rect + 2);
    err = err || screen_create_pixmap_buffer(pixmap);
    err = err || screen_get_pixmap_property_pv(pixmap, SCREEN_PROPERTY_RENDER_BUFFERS,
                                               reinterpret_cast<void**>(&buffer));
    err = err || screen_get_buffer_property_pv(buffer, SCREEN_PROPERTY_POINTER, &pointer);
    err = err || screen_get_buffer_property_iv(buffer, SCREEN_PROPERTY_STRIDE, &stride);
    err = err || screen_read_display(display, buffer, 1, rect, 0);

    if (err == 0) {
        saveBmp((char *)pointer, intersection.width(), intersection.height(), stride);
    } else {
        printf("Capture error, errno: %d(%s)\n", errno, strerror(errno));
    }

    screen_destroy_pixmap(pixmap);

    return;
}

bool Screen::createScreenContext()
{
    if (screen_create_context(&m_context,
                              SCREEN_DISPLAY_MANAGER_CONTEXT
                              | SCREEN_WINDOW_MANAGER_CONTEXT) != 0) {
        printf("Failed to create screen context, errno: %d(%s)\n", errno, strerror(errno));
        return false;
    }

    return true;
}

bool Screen::findDisplays()
{
    m_displayList.clear();

    int count = 0;
    if (screen_get_context_property_iv(m_context, SCREEN_PROPERTY_DISPLAY_COUNT, &count) != 0) {
        printf("Failed to get screen context, errno: %d(%s)\n", errno, strerror(errno));
        return false;
    }

    if (count == 0) {
        printf("No display found\n");
        return false;
    }

    screen_display_t* screen_disps = (screen_display_t*)std::calloc(count, sizeof(screen_display_t));
    if (screen_get_context_property_pv(m_context, SCREEN_PROPERTY_DISPLAYS, (void **)screen_disps) != 0) {
        printf("Failed to get screen context, errno: %d(%s)\n", errno, strerror(errno));
        std::free(screen_disps);
        return false;
    }

    m_displayList.reserve(count);
    for (int i = 0; i < count; ++i) {
        m_displayList.emplace_back(screen_disps[i]);
    }

    std::free(screen_disps);

    return true;
}

void Screen::saveBmp(char *_data, int _w, int _h, int _stride)
{
    // https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
    FILE *f;
    int filesize = 54 + 3 * _w * _h;

    char *img = (char *)std::malloc(3 * _w * _h);

    for(int y = 0; y < _h; y++) {
        memcpy(img + y * _w * 3, _data + y * _stride, _w * 3);
    }

    unsigned char bmpFileHeader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpInfoHeader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmpPad[3] = {0,0,0};

    bmpFileHeader[ 2] = (unsigned char)(filesize      );
    bmpFileHeader[ 3] = (unsigned char)(filesize >>  8);
    bmpFileHeader[ 4] = (unsigned char)(filesize >> 16);
    bmpFileHeader[ 5] = (unsigned char)(filesize >> 24);

    bmpInfoHeader[ 4] = (unsigned char)(      _w      );
    bmpInfoHeader[ 5] = (unsigned char)(      _w >>  8);
    bmpInfoHeader[ 6] = (unsigned char)(      _w >> 16);
    bmpInfoHeader[ 7] = (unsigned char)(      _w >> 24);
    bmpInfoHeader[ 8] = (unsigned char)(      _h      );
    bmpInfoHeader[ 9] = (unsigned char)(      _h >>  8);
    bmpInfoHeader[10] = (unsigned char)(      _h >> 16);
    bmpInfoHeader[11] = (unsigned char)(      _h >> 24);

    auto path = Config::instance()->getFileName("bmp");  
    f = fopen(path.c_str(), "wb");

    if (!f) {
        printf("Failed opening file: %s\n", path.c_str());
        return;
    }

    fwrite(bmpFileHeader, 1, 14, f);
    fwrite(bmpInfoHeader, 1, 40, f);

    for(int i = 0; i < _h; i++) {
        fwrite(img + (_w * (_h - i - 1) * 3), 3, _w, f);
        fwrite(bmpPad, 1, (4 - (_w * 3) % 4) % 4, f);
    }

    std::free(img);
    fclose(f);

    return;
}
