QT      += core widgets printsupport multimedia multimediawidgets

CONFIG += console

 HEADERS         = mainwindow.h
 SOURCES         = main.cpp \
                   mainwindow.cpp
 RESOURCES       = dockwidgets.qrc

 unix {
  LIBS += -L/usr/local/lib -lvlc
  INCLUDEPATH +=/usr/local/include/vlc/
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
