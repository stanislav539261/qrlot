QT += core gui

CONFIG += c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(external/QXlsx/QXlsx/QXlsx.pri)
include(external/QZXing/src/QZXing.pri)

DEFINES += ENABLE_ENCODER_GENERIC

INCLUDEPATH += \
    external/QXlsx/QXlsx/header \
    external/qzxing/src

SOURCES += \
    dialogs.cpp \
    main.cpp \
    progressdialog.cpp \
    state.cpp \
    window.cpp

HEADERS += \
    dialogs.h \
    progressdialog.h \
    state.h \
    window.h

FORMS += \
    exportpicturesdialog.ui \
    exportspreadsheetdialog.ui \
    progressdialog.ui \
    window.ui

TRANSLATIONS += \
    qrlot_ru_RU.ts

TRANSLATIONS +=
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
