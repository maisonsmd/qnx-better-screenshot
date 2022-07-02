#include "screenshot.h"

#include <QDebug>
#include <QImage>

#include <errno.h>
#include <cstdlib>

Screenshot::Screenshot()
{
    if (!createScreenContext()) {
        return;
    }

    if (!findDisplays()) {
        return;
    }
}

Screenshot::~Screenshot() {
    screen_destroy_context(m_context);
}

std::vector<screen_display_t> Screenshot::displays() const
{
    return m_displayList;
}

int Screenshot::displayCount() const
{
    return m_displayList.size();
}

QSize Screenshot::displayResolution(screen_display_t &_display) const
{
    int size[2];
    QSize result {};

    if (screen_get_display_property_iv(_display, SCREEN_PROPERTY_NATIVE_RESOLUTION, size) != 0) {
        qCritical("Failed to get display size, errno: %d(%s)", errno, strerror(errno));
        return result;
    }

    result.setWidth(size[0]);
    result.setHeight(size[1]);

    return result;
}

QPixmap Screenshot::captureScreen(int _displayIndex, int _x, int _y, int _w, int _h)
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

    if (_w <= 0 || _w > size.width())
        _w = size.width();

    if (_h <= 0 || _h > size.height())
        _h = size.height();

#if 0
    // Find corresponding display in SCREEN_DISPLAY_MANAGER_CONTEXT
    auto m_display = m_displayList[0];
    int count = 0;
    screen_display_t display = 0;
    screen_get_context_property_iv(m_context, SCREEN_PROPERTY_DISPLAY_COUNT, &count);

    if (count > 0) {
        const size_t idLen = 30;
        char matchId[idLen];
        char id[idLen];
        bool found = false;

        screen_display_t *displays = static_cast<screen_display_t*>
                (calloc(count, sizeof(screen_display_t)));

        screen_get_context_property_pv(m_context, SCREEN_PROPERTY_DISPLAYS, (void **)displays);
        screen_get_display_property_cv(m_display,  SCREEN_PROPERTY_ID_STRING, idLen, matchId);

        while (count && !found) {
            --count;
            screen_get_display_property_cv(displays[count], SCREEN_PROPERTY_ID_STRING, idLen, id);
            qDebug() << id << matchId;
            found = !strncmp(id, matchId, idLen);
        }

        if (found) {
            display = displays[count];
            qDebug("index: %d", count);
        }

        free(displays);

        if (!found) {
            return QImage();
        }
    }
#endif

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

bool Screenshot::createScreenContext()
{
    if (screen_create_context(&m_context,
                              SCREEN_DISPLAY_MANAGER_CONTEXT
                              | SCREEN_WINDOW_MANAGER_CONTEXT) != 0) {
        qCritical("Failed to create screen context, errno: %d(%s)", errno, strerror(errno));
        return false;
    }

    return true;
}

bool Screenshot::findDisplays()
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

