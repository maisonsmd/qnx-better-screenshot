CONFIG += c++11
TARGET = betterss

DESTDIR = $$(APP_INSTALL_IVI_BIN_DIR)

target.path = /opt/bin
INSTALLS += target

SOURCES += appmain.cpp \
            main.cpp \
            screenmanager.cpp \
            spng/spng.c

HEADERS += appmain.h \
    config.h \
    screenmanager.h \
    spng/spng.h

INCLUDEPATH+=E:/fw/framework/include
INCLUDEPATH+=E:/fw/QNX/qnx700/target/qnx7/usr/include
INCLUDEPATH+=E:/fw/QNX/qnx700/target/qnx7/usr/include/c++/v1

LIBS += -lscreen -lz
