/****************************************************************************
 **
 ** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #ifndef MAINWINDOW_H
 #define MAINWINDOW_H

 #include <QMainWindow>
 #include <QDragEnterEvent>
 #include <QMediaPlayer>
 #include <QVideoWidget>
 #include <QLabel>
 #include <QtMultimedia>

 //#include <vlc/vlc.h>
#include "videowidget.h"
#include "graphviewer.h"
#include "yuvwidget.h"
#include "audiowidget.h"

#include <gst/pbutils/pbutils.h>

#define MAX_AUDIO_TRACKS 8

// todo : set some dir for Windows
#define PIPELINE_DIR       "/tmp"
//#define PIPELINE_DIR       "."
#define PIPELINE_PATH      "pipeline_graph"
#define PIPELINE_PATH_DOT  PIPELINE_DIR"/pipeline_graph.dot"
#define PIPELINE_PATH_PNG  PIPELINE_DIR"/pipeline_graph.png"

 class QAction;
 class QListWidget;
 class QMenu;
 class QTextEdit;

 enum MessageType {
     MT_NONE = 0,
     MT_INFO,
     MT_WARNING,
     MT_ERROR,
     MT_DEBUG,
     MT_COUNT
 };

 typedef struct _BunchOfGstElements
 {
     GstElement * filesrc;
     //GstElement * queue;
     //GstElement * tsdemux;
     GstElement * xvimagesink;
     //GstElement * mpeg2dec;

     GstElement * decodebin;    // test

     unsigned int audioTracks;

     //GstElement * demux;

 } BunchOfGstElements;

 typedef struct _AudioPipeline
 {
     //GstPadTemplate *tee_src_pad_template;
     //GstPad *tee_audio_sound_pad, *tee_audio_visual_pad;
     //GstPad *queue_audio_sound_pad, *queue_audio_visual_pad;

     //GstElement * decodebin;
     //GstElement * queue;
     GstElement * wavescope;
     GstElement * convert;
     GstElement * imagesink;
     //GstElement * tee;
     //GstElement * visual_queue;
     //GstElement * sound_queue;
     //GstElement * audiosink;
     //GstElement * typefind;

     GstPad * headSinkPad;

 } AudioPipeline;

typedef struct _VideoInfo
{
    bool isValid;

    int frameWidth;
    int frameHeight;
    int frameStride;
    int frameSize;
    GstVideoInterlaceMode interlace;
    GstVideoFormat        videoFormat;
    GstVideoFlags         flags;
    GstVideoFormatFlags   formatFlags;
    GstVideoFormat        unpackFormat;

    int frameOffset;
    int views;
    int pixelStride;
    int numComponents;
    int shift;
    int bitDepth;
    int plane;
    int pOffset;
    int wSub;
    int hSub;
    int packLines;

    double yuvCoeffW;
    double yuvCoeffH;
    // todo : add all useful fields

} VideoInfo;

 class MainWindow : public QMainWindow
 {
     Q_OBJECT

 public:
     MainWindow();
     ~MainWindow();

     // todo : remove from public
     GstElement    * pipeline;
     GMainLoop     * loop;
     QListWidget   * messageList;
     BunchOfGstElements bunch;
     VideoWidget   * videoWidget;
     YuvWidget     * yuvWidget;
     QDockWidget   * yuvDock;
     int             discoverLevel;
     int             discoverAudioLevel;

     AudioPipeline audioBranches[MAX_AUDIO_TRACKS];

     void qSleep(int ms);
     void addColoredLog (const QString &line, MessageType type);
     int linkAudioBranch (int trackNumber);

     // todo : make more elegant
     QDockWidget * audioDocks[MAX_AUDIO_TRACKS];
     int numberOfAudioDocks;

     VideoInfo videoInfo;

 private slots:
     void about();
     void openFiles();
     void openURI();
     void onSelectPlaylist(const QString &playlistItem);
     void onDoubleClick(const QModelIndex &modelIndex);

 private:
     void createActions();
     void createMenus();
     void createToolBars();
     void createStatusBar();
     void createDockWindows();
     void createCentralWidget();
     void createYuvDock();
     void createDiscoverer();
     void createVlc();

     void addAudioDock(int trackNumber);
     void removeAudioDocks();
     void dumpAndShowPipeline();
     void startPlaying();
     void stopPlaying();
     void pausePlaying();
     void setVideoOverlays();
     void setAudioOverlays();
     void setFakesinkCallbacks();
     void addSourcesToPlaylist(QList<QUrl> urls);
     void addSourcesToPlaylist(QStringList urls);

     void insertMediaInfo (const char * uri);

     QListWidget   * playList;
     QTextEdit     * codecInfo;
     GstDiscoverer * discoverer;
     GraphViewer   * graphViewer;

     QMenu *fileMenu;
     QMenu *editMenu;
     QMenu *viewMenu;
     QMenu *helpMenu;
     QToolBar *fileToolBar;
     QToolBar *editToolBar;
     QAction *aboutAct;
     QAction *aboutQtAct;
     QAction *quitAct;
     QAction *openFileAct;
     QAction *openURIAct;

     //libvlc_instance_t *vlcInstance;
     //libvlc_media_player_t *vlcPlayer;

 private:

     void cleanup();
     int createPipelineByString ();
     int createPipelineByCode ();
     GstElement * checkGstElement(const gchar * name);
     GstElement * findVideosink();
     int createAudioBranchElements(int index);
     int convertDotToPng(char * dotPath, char * pngPath);
     void setCodecAutoplugPriority(const char * codecName, int rank);

 protected:
     void dragEnterEvent(QDragEnterEvent *event);
     void dropEvent(QDropEvent *event);
 };

 #endif
