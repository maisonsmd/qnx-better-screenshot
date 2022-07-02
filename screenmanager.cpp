#include "screenmanager.h"

#include <QDebug>
#include <QImage>

#include <errno.h>
#include <cstdlib>

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

QSize Screen::displayResolution(screen_display_t _display) const
{
    int size[2] {0};
    int rotation = displayRotation(_display);

    QSize result {};

    if (screen_get_display_property_iv(_display, SCREEN_PROPERTY_NATIVE_RESOLUTION, size) != 0) {
        qCritical("Failed to get display rotation, errno: %d(%s)", errno, strerror(errno));
        return result;
    }

    if (rotation == Rotation::SCREEN_ROTATION_90
            || rotation == Rotation::SCREEN_ROTATION_270) {
        result.setWidth(size[1]);
        result.setHeight(size[0]);
    } else {
        result.setWidth(size[0]);
        result.setHeight(size[1]);
    }

    return result;
}

Screen::Rotation Screen::displayRotation(screen_display_t _display) const
{
    int rotation {SCREEN_ROTATION_NONE};
    if (screen_get_display_property_iv(_display, SCREEN_PROPERTY_ROTATION, &rotation) != 0) {
        qCritical("Failed to get display rotation, errno: %d(%s)", errno, strerror(errno));
        return SCREEN_ROTATION_NONE;
    }
    return (Rotation)rotation;
}

QPixmap Screen::captureScreen(int _displayIndex, int _x, int _y, int _w, int _h)
{
    QPixmap result;

    if (displayCount() == 0) {
        qCritical("No display found!");
        return result;
    }

    if (_displayIndex < 0 || _displayIndex >= displayCount()) {
        qCritical("displayIndex out of range");
        return result;
    }

    auto display = m_displayList[_displayIndex];
    auto size = displayResolution(display);

    qDebug() << "display resolution: " << size;

    if (_w <= 0 || _w > size.width())
        _w = size.width();

    if (_h <= 0 || _h > size.height())
        _h = size.height();

    qDebug() << _x << _y << _w << _h;

    screen_pixmap_t pixmap;
    if (screen_create_pixmap(&pixmap, m_context) != 0) {
        qCritical("Failed creating pixmap");
        return QPixmap();
    }

    screen_buffer_t buffer;

    void *pointer;
    int stride;
    const int rect[4] = {_x, _y, _w, _h};

    int val = SCREEN_USAGE_READ | SCREEN_USAGE_NATIVE;
    screen_set_pixmap_property_iv(pixmap, SCREEN_PROPERTY_USAGE, &val);

    val = SCREEN_FORMAT_RGBA8888;
    screen_set_pixmap_property_iv(pixmap, SCREEN_PROPERTY_FORMAT, &val);

    int err =    screen_set_pixmap_property_iv(pixmap, SCREEN_PROPERTY_BUFFER_SIZE, rect + 2);
    err = err || screen_create_pixmap_buffer(pixmap);
    err = err || screen_get_pixmap_property_pv(pixmap, SCREEN_PROPERTY_RENDER_BUFFERS,
                                               reinterpret_cast<void**>(&buffer));
    err = err || screen_get_buffer_property_pv(buffer, SCREEN_PROPERTY_POINTER, &pointer);
    err = err || screen_get_buffer_property_iv(buffer, SCREEN_PROPERTY_STRIDE, &stride);
    err = err || screen_read_display(display, buffer, 1, rect, 0);

    if (err == 0) {
        const QImage image = QImage(static_cast<unsigned char*>(pointer),
                                    _w, _h, stride, QImage::Format_ARGB32);
        result = QPixmap::fromImage(image);
    } else {
        qCritical("Capture error, errno: %d(%s)", errno, strerror(errno));
    }

    screen_destroy_pixmap(pixmap);

    return result;
}

bool Screen::createScreenContext()
{
    if (screen_create_context(&m_context,
                              SCREEN_DISPLAY_MANAGER_CONTEXT
                              | SCREEN_WINDOW_MANAGER_CONTEXT) != 0) {
        qCritical("Failed to create screen context, errno: %d(%s)", errno, strerror(errno));
        return false;
    }

    return true;
}

bool Screen::findDisplays()
{
    m_displayList.clear();

    int count = 0;
    if (screen_get_context_property_iv(m_context, SCREEN_PROPERTY_DISPLAY_COUNT, &count) != 0) {
        qCritical("Failed to get screen context, errno: %d(%s)", errno, strerror(errno));
        return false;
    }

    if (count == 0) {
        qCritical("No display found");
        return false;
    }

    screen_display_t* screen_disps = (screen_display_t*)std::calloc(count, sizeof(screen_display_t));
    if (screen_get_context_property_pv(m_context, SCREEN_PROPERTY_DISPLAYS, (void **)screen_disps) != 0) {
        qCritical("Failed to get screen context, errno: %d(%s)", errno, strerror(errno));
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

