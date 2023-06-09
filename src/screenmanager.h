#ifndef SCREEN_H
#define SCREEN_H

#include <cstdio>
#include <vector>
#include <screen/screen.h>
#include <memory>
#include <functional>

class Screen
{
public:
    //                                           raw   x,y,w,h  stride
    using OnCaptureCallback = std::function<void(void *, int *, int)>;

    Screen();
    ~Screen();

    enum Rotation {
        SCREEN_ROTATION_NONE = 0,
        SCREEN_ROTATION_90 = 90,
        SCREEN_ROTATION_180 = 180,
        SCREEN_ROTATION_270 = 270
    };

    struct Size {
        int w;
        int h;
    };

    /**
     * @brief  Get list of displays
     */
    std::vector<screen_display_t> displays() const;

    /**
     * @brief  Get number of displays
     */
    int displayCount() const;

    /**
     * @brief  Get display resolution
     * @return  The width & heigh might get swapped if the screen is rotated
     */
    Size displayResolution(screen_display_t _display) const;
    Rotation displayRotation(screen_display_t _display) const;

    /**
     * @brief  Take the screenshot
     * @param displayIndex  The index of the display
     */
    void captureScreen(int _displayIndex = 0, int _x = 0, int _y = 0, int _w = -1, int _h = -1);

    /**
     * @brief  Set the callback to be called after capturing
     */
    void setCaptureCallback(const OnCaptureCallback &_callback);

private:
    bool createScreenContext();
    bool findDisplays();

private:
    screen_context_t m_context {};
    std::vector<screen_display_t> m_displayList {};

    OnCaptureCallback m_callback {};
};

#endif // SCREEN_H
