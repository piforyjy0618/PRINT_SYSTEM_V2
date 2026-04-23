QT -= gui

TEMPLATE = lib
DEFINES += PRINTSYSTEM_LIBRARY

CONFIG += c++17

msvc{
    QMAKE_CFLAGS +=/utf-8
    QMAKE_CXXFLAGS +=/utf-8
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HeadBoard/head_board.cpp \
    MainBoard/main_board.cpp \
    PrintHead/HeadTypes/qs256_head.cpp \
    PrintHead/print_head.cpp \
    print_system.cpp

HEADERS += \
    HeadBoard/head_board.h \
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
