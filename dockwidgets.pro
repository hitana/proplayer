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
  LIBS += -L/usr/local/lib -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0

 }

macx
{
  #LIBS += -L$$PWD/vlc-sdk/lib/ -lvlc
  INCLUDEPATH += $$PWD/vlc-sdk/include
  DEPENDPATH += $$PWD/vlc-sdk/include
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
