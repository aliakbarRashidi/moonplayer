#-------------------------------------------------
#
# Project created by QtCreator 2012-09-08T16:18:28
#
#-------------------------------------------------

QT += core gui network xml widgets

macx:  TARGET = MoonPlayer
!macx: TARGET = moonplayer
TEMPLATE = app


SOURCES += main.cpp\
    player.cpp \
    playlist.cpp \
    webvideo.cpp \
    skin.cpp \
    httpget.cpp \
    downloader.cpp \
    pyapi.cpp \
    settingsdialog.cpp \
    reslibrary.cpp \
    resplugin.cpp \
    mybuttongroup.cpp \
    detailview.cpp \
    utils.cpp \
    mylistwidget.cpp \
    cutterbar.cpp \
    videocombiner.cpp \
    searcher.cpp \
    classicplayer.cpp \
    plugin.cpp \
    updatechecker.cpp \
    selectiondialog.cpp \
    videoqualities.cpp \
    playercore_mpv.cpp \
    danmakudelaygetter_mpv.cpp \
    accessmanager.cpp
!macx: SOURCES += localserver.cpp \
    localsocket.cpp
unix: SOURCES += yougetbridge.cpp \
    danmakuloader.cpp


TRANSLATIONS += moonplayer_zh_CN.ts


HEADERS  += player.h\
    playlist.h \
    webvideo.h \
    skin.h \
    httpget.h \
    downloader.h \
    pyapi.h \
    settings_player.h \
    settings_video.h \
    settings_network.h \
    settingsdialog.h \
    reslibrary.h \
    resplugin.h \
    accessmanager.h \
    mybuttongroup.h \
    detailview.h \
    utils.h \
    mylistwidget.h \
    settings_audio.h \
    cutterbar.h \
    settings_plugins.h \
    videocombiner.h \
    searcher.h \
    settings_danmaku.h \
    classicplayer.h \
    playercore.h \
    plugin.h \
    updatechecker.h \
    selectiondialog.h \
    videoqualities.h

unix: HEADERS += yougetbridge.h \
    danmakuloader.h \
    danmakudelaygetter.h
!macx: HEADERS += localserver.h \
    localsocket.h


FORMS    += \
    player.ui \
    playlist.ui \
    settingsdialog.ui \
    reslibrary.ui \
    detailview.ui \
    cutterbar.ui \
    classicplayer.ui \
    selectiondialog.ui

# Installation on Linux
unix:!macx {
    usr_share.files += skins plugins moonplayer_*.qm Version danmaku2ass
    usr_share.path = /usr/share/moonplayer
    #icon
    icon.files += moonplayer.png
    icon.path = /usr/share/icons
    #bin
    execute.files += moonplayer
    execute.path = /usr/bin
    #menu
    menu.files += moonplayer.desktop
    menu.path = /usr/share/applications
    INSTALLS += usr_share icon execute menu
}

# Build bundle for Mac OS X
macx {
    FFMPEG.files = /usr/local/opt/ffmpeg/bin/ffmpeg
    FFMPEG.path = Contents/MacOS
    RESFILES.files = moonplayer_zh_CN.qm upgrade-you-get.sh danmaku2ass skins plugins icons Version
    RESFILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += RESFILES FFMPEG
    QMAKE_INFO_PLIST = Info.plist
    ICON = moonplayer.icns
}

# Windows icon
win32: RC_FILE = icon.rc

# Libraries
unix:!macx: CONFIG += link_pkgconfig
unix:!macx: PKGCONFIG += python2 mpv

macx: INCLUDEPATH += /System/Library/Frameworks/Python.framework/Versions/2.7/Headers \
    /usr/local/include
macx: LIBS += -L/System/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/config \
    -lpython2.7 -ldl -framework CoreFoundation \
    -L /usr/local/lib -lmpv

win32: INCLUDEPATH += C:\\Python27\\include
win32: LIBS += C:\\Python27\\libs\\python27.lib
