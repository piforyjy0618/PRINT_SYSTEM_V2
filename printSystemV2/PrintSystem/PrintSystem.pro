QT -= gui

TEMPLATE = lib
DEFINES += PRINTSYSTEM_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HeadBoard/headboard.cpp \
    MainBoard/icommandsender.cpp \
    MainBoard/mainboard.cpp \
    PrintHead/HeadTypes/qs256_head.cpp \
    PrintHead/printhead.cpp \
    printsystem.cpp

HEADERS += \
    HeadBoard/headboard.h \
    MainBoard/icommandsender.h \
    MainBoard/mainboard.h \
    PrintHead/HeadTypes/qs256_head.h \
    PrintHead/printhead.h \
    PrintSystemSDK.h \
    PrintSystem_global.h \
    SdkExport.h \
    printsystem.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
