# 基础模块
QT       += core gui

# QT5/6兼容：引入widgets（按钮、滑块等）
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# 关键：多媒体模块（播放音频必须）
QT += multimedia multimediawidgets

# C++标准（适配QT6.8.3）
CONFIG += c++17

# 禁用废弃API
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# 源文件列表
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    songlibrarywindow.cpp

# 头文件列表
HEADERS += \
    mainwindow.h \
    songlibrarywindow.h

# UI文件列表（仅保留主窗口UI，歌库窗口纯代码创建）
FORMS += \
    mainwindow.ui

# Windows优化：隐藏控制台窗口
win32: CONFIG -= console
win32: CONFIG -= app_bundle

# 部署规则（默认即可）
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
