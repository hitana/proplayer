QT      += core widgets printsupport multimedia multimediawidgets gui opengl

CONFIG += console

 HEADERS         = mainwindow.h \
    pipeline.h \
    asyncwaitingqueue.h \
    gstpipeline.h \
    glwidget.h \
    yuvdebugwindow.h
 SOURCES         = main.cpp \
                   mainwindow.cpp \
    pipeline.cpp \
    gstpipeline.cpp \
    glwidget.cpp \
    yuvdebugwindow.cpp
 RESOURCES       = dockwidgets.qrc

 unix {
  # add vlc
  #LIBS += -L/usr/local/lib -lvlc
  #INCLUDEPATH +=/usr/local/include/vlc/

  # add glib-2.0
  INCLUDEPATH += -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include
  INCLUDEPATH += -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

  # add gstreamer-1.0
  INCLUDEPATH +=/usr/local/include/gstreamer-1.0
  INCLUDEPATH +=/usr/include/gstreamer-1.0
  INCLUDEPATH +=/usr/local/lib/gstreamer-1.0/include/
  LIBS += -L/usr/local/lib -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstbase-1.0 -lgstvideo-1.0

 }

macx
{
  #LIBS += -L$$PWD/vlc-sdk/lib/ -lvlc
  INCLUDEPATH += $$PWD/vlc-sdk/include
  DEPENDPATH += $$PWD/vlc-sdk/include

  # add gstreamer
  #INCLUDEPATH += `pkg-config --cflags gstreamer-1.0`
  #LIBS += `pkg-config --libs gstreamer-1.0`
  DEPENDPATH += -D_REENTRANT -I/usr/local/Cellar/gstreamer/1.4.5/include/gstreamer-1.0
  DEPENDPATH += -I/usr/local/Cellar/glib/2.44.1/include/glib-2.0
  DEPENDPATH += -I/usr/local/Cellar/glib/2.44.1/lib/glib-2.0/include
  DEPENDPATH += -I/usr/local/opt/gettext/include
  LIBS += -L/usr/local/Cellar/gstreamer/1.4.5/lib
  LIBS += -L/usr/local/Cellar/glib/2.44.1/lib
  LIBS += -L/usr/local/opt/gettext/lib
  #LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lintl
  LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstbase-1.0 -lgstvideo-1.0
}

win32 {
  INCLUDEPATH += C:\Gstreamer\1.0\x86_64\include\gstreamer-1.0\gst
  INCLUDEPATH += C:\Gstreamer\1.0\x86_64\include\gstreamer-1.0
  LIBS += -LC:\Gstreamer\1.0\x86_64\lib -lgstreamer-1.0

  INCLUDEPATH += C:\Gstreamer\1.0\x86_64\include\glib-2.0
  INCLUDEPATH += C:\Gstreamer\1.0\x86_64\lib\glib-2.0\include
  INCLUDEPATH += C:\Gstreamer\1.0\x86_64\lib\gstreamer-1.0\include
  LIBS +=  -lglib-2.0
  LIBS += -LC:\Gstreamer\1.0\x86_64\lib -lgobject-2.0
}

 # install
 target.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/dockwidgets
 sources.files = $$SOURCES $$HEADERS $$RESOURCES dockwidgets.pro images
 sources.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/dockwidgets
 INSTALLS += target sources

 symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
 maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

 symbian: warning(This example might not fully work on Symbian platform)
 maemo5: warning(This example might not fully work on Maemo platform)
 simulator: warning(This example might not fully work on Simulator platform)
