QT -= gui

TEMPLATE = lib
DEFINES += SDK_EXPORTS

CONFIG += c++17

msvc{
    QMAKE_CFLAGS +=/utf-8
    QMAKE_CXXFLAGS +=/utf-8
}

# 是否在测试环境下
DEFINES += TEST_ENV

CONFIG(debug, debug|release) {
    TARGET = PrintSystemd
} else {
    TARGET = PrintSystem
}

# 调用boost库
INCLUDEPATH += C:/boost_1_90_0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ConfigManager/config_manager.cpp \
    HeadBoard/head_board.cpp \
    MainBoard/BoardCommunicate/board_communicate.cpp \
    MainBoard/BoardCommunicate/tcp/tcp_async_client.cpp \
    MainBoard/BoardCommunicate/udp/udp_async_client.cpp \
    MainBoard/main_board.cpp \
    PrintHead/HeadTypes/qs256_head.cpp \
    PrintHead/print_head.cpp \
    print_system.cpp

HEADERS += \
    ConfigManager/config_manager.h \
    ConfigManager/system_config.h \
    HeadBoard/head_board.h \
    MainBoard/BoardCommunicate/board_communicate.h \
    MainBoard/BoardCommunicate/tcp/tcp_async_client.h \
    MainBoard/BoardCommunicate/udp/udp_async_client.h \
    MainBoard/i_command_sender.h \
    MainBoard/main_board.h \
    PrintHead/HeadTypes/qs256_head.h \
    PrintHead/print_head.h \
    PrintSystemSDK.h \
    SdkExport.h \
    print_system.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

# 调用 spdlog
win32-g++:CONFIG(release, debug|release): LIBS += -L$$PWD/3rdparty/spdlog/ -lspdlog
else:win32-g++:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rdparty/spdlog/ -lspdlog
else:win32:!win32-g++:CONFIG(release, debug|release): LIBS += -L$$PWD/3rdparty/spdlog/ -lspdlog
else:win32:!win32-g++:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rdparty/spdlog/ -lspdlogd
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/3rdparty/spdlog/libspdlog.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/3rdparty/spdlog/libspdlog.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/3rdparty/spdlog/spdlog.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/3rdparty/spdlog/spdlogd.lib
INCLUDEPATH += $$PWD/3rdparty/spdlog
DEPENDPATH += $$PWD/3rdparty/spdlog

# 调用 fmt
win32-g++:CONFIG(release, debug|release): LIBS += -L$$PWD/3rdparty/fmt/ -lfmt
else:win32-g++:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rdparty/fmt/ -lfmt
else:win32:!win32-g++:CONFIG(release, debug|release): LIBS += -L$$PWD/3rdparty/fmt/ -lfmt
else:win32:!win32-g++:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rdparty/fmt/ -lfmtd
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/src/3rdparty/libfmt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/3rdparty/fmt/libfmt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/3rdparty/fmt/fmt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/3rdparty/fmt/fmtd.lib
INCLUDEPATH += $$PWD/3rdparty/fmt
DEPENDPATH += $$PWD/3rdparty/fmt
