#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QPixmap>

#include <vector>

#include <screen/screen.h>

class Screenshot
{
public:
    Screenshot();
    ~Screenshot();

    /**
     * @brief  Get list of displays
     */
    std::vector<screen_display_t> displays() const;

    /**
     * @brief  Get number of displays
     */
    int displayCount() const;

    /**
     * @brief Get display resolution
     */
    QSize displayResolution(screen_display_t &_display) const;

    /**
     * @brief  Take the screenshot
     * @param displayIndex  The index of the display
     */
    QPixmap captureScreen(int _displayIndex = 0, int _x = 0, int _y = 0, int _w = -1, int _h = -1);

private:
    bool createScreenContext();
    bool findDisplays();

private:
    screen_context_t m_context;
    std::vector<screen_display_t> m_displayList;
};

#endif // SCREENSHOT_H
