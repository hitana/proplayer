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

#include <QtGui>
#include <QtPrintSupport>
#include <QtWidgets>
#include <QMainWindow>

#define DEFAULT_VIDEOSINK "autovideosink"

#include "mainwindow.h"
/*
static void print_stream_info (GstDiscovererStreamInfo *info, gint depth);
static void print_tag_foreach (const GstTagList *tags, const gchar *tag, gpointer user_data);
static void print_topology (GstDiscovererStreamInfo *info, gint depth);
static void on_discovered_cb (GstDiscoverer *discoverer, GstDiscovererInfo *info, GError *err, , MainWindow * mainWindow);
static void on_finished_cb (GstDiscoverer *discoverer, MainWindow * mainWindow);
*/
/* slightly convoluted way to find a working video sink that's not a bin,
 * one could use autovideosink from gst-plugins-good instead
 */

static void on_new_decodebin_pad (GstElement *element, GstPad *pad, gpointer data);
static void on_autoplug_continue (GstElement *decodebin, GstPad *pad, GstCaps * gstcaps, gpointer data);
static void on_no_more_pads (GstElement * element, gpointer data);
static void on_pad_removed (GstElement * element, GstPad * oldpad, gpointer data);
static void on_have_type (GstElement *typefind, guint probability, GstCaps *caps, gpointer data);

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

void MainWindow::qSleep(int ms)
{
    if (ms <= 0) return;

#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

GstElement * MainWindow::checkGstElement(const gchar * name)
{
    //GstStateChangeReturn sret;
    GstElement * element;

    //if (element = gst_element_factory_make (name, NULL))
    if ((element = gst_element_factory_make (name, name)))    // use the same name
    {
        g_printerr("findVideosink: Found %s\n", name);
        return element;
        /*
        sret = gst_element_set_state (element, GST_STATE_READY);
        if (sret == GST_STATE_CHANGE_SUCCESS)
        {
            g_printerr("findVideosink: Found %s\n", name);
            return element;
        }
        gst_element_set_state (element, GST_STATE_NULL);
        gst_object_unref (element);*/
    }
    return NULL;
}

GstElement * MainWindow::findVideosink()
{
    GstElement * sink;

    sink = checkGstElement("xvimagesink");
    if (sink) return sink;

    sink = checkGstElement("glimagesink");
    if (sink) return sink;

    sink = checkGstElement("osxvideosink");
    if (sink) return sink;

    sink = checkGstElement("autovideosink");
    if (sink) return sink;

    sink = checkGstElement("ximagesink");
    if (sink) return sink;

    return NULL;
}

 MainWindow::MainWindow()
     :pipeline(NULL)
 {
     gst_init (NULL, NULL);

     bunch.audioTracks = 0;
     numberOfAudioDocks = 0;

     createActions();
     createMenus();
     createToolBars();
     createStatusBar();
     createCentralWidget();
     createDockWindows();
     //createAudioDock();
     createDiscoverer();
     //createVlc();

     setWindowTitle(tr("ProPlayer Skeleton App"));

     setUnifiedTitleAndToolBarOnMac(true);

     setAcceptDrops(true);
 }

 void MainWindow::addColoredLog(const QString &line, MessageType type)
 {
     QColor color = Qt::white;
     QListWidgetItem * listItem = new QListWidgetItem(line);
     switch (type){
     case MT_INFO:
         color = QColor(0, 255, 0, 30);   // light green
         break;
     case MT_WARNING:
         color = QColor(255, 255, 0, 30); // yellow
         break;
     case MT_ERROR:
         color = QColor(255, 0, 0, 30);   // red
         break;
     case MT_DEBUG:
         color = QColor(0, 0, 255, 30);   // blue
         break;
     default:
         break;
     }
     //listItem->setForeground(Qt::red); // sets red text
     listItem->setBackground (color);
     messageList->addItem (listItem);
     messageList->show();
     messageList->scrollToBottom();
 }

 void MainWindow::dragEnterEvent(QDragEnterEvent * e)
 {
     if (e->mimeData()->hasUrls()) {
         e->acceptProposedAction();
     }
 }

 void MainWindow::dropEvent(QDropEvent * e)
 {
     foreach (const QUrl & url, e->mimeData()->urls()) {
         const QString & fileName = url.toLocalFile();

         playList->addItem(fileName);

     }
 }

 void MainWindow::insertMediaInfo (const char * filepath)
 {
     QTextDocument *document = codecInfo->document();
     QTextCursor cursor(document);

     document->clear(); // ??

     cursor.beginEditBlock();
     cursor.insertText("Hello");
     cursor.endEditBlock();

   /*
     cursor.beginEditBlock();
     //cursor.insertText();
     cursor.insertBlock();
     cursor.insertText(infoList.at(i));
     cursor.endEditBlock();
 */
     bunch.audioTracks = 0; // test, clear before discover

     //const gchar *uri = "http://docs.gstreamer.com/media/sintel_trailer-480p.webm";   // for test
     gchar uri[PATH_MAX];
     snprintf (uri, PATH_MAX, "file://%s", filepath);
     qDebug () << "insertMediaInfo: " << uri;

       //cursor.insertBlock();
       //cursor.beginEditBlock();
       cursor.insertText(uri);
       //cursor.endEditBlock();

       addColoredLog("I: Discovering " + QString::fromLocal8Bit(uri) + " ...", MT_INFO);

       /* Start the discoverer process (nothing to do yet) */
       gst_discoverer_start (discoverer);

       /* Add a request to process asynchronously the URI passed through the command line */
       if (!gst_discoverer_discover_uri_async (discoverer, uri)) {
           addColoredLog("E: Failed to start discovering URI " + QString::fromLocal8Bit(uri), MT_ERROR);
           g_object_unref (discoverer);
           return;
       }

       /* Set GLib Main Loop to run, so we can wait for the signals */
       g_main_loop_run (loop);

       gst_discoverer_stop (discoverer);

       /* Free resources */
       //g_object_unref (discoverer);
       //g_main_loop_unref (loop);

       // end discover media data
       // ///////////////////////////////////////////////////////////////////////////

     // todo : make a QString with all mediadata
/*
     QStringList infoList = info.split(", ");
     QTextDocument *document = textEdit->document();
     QTextCursor cursor = document->find("NAME");
     if (!cursor.isNull()) {
         cursor.beginEditBlock();
         cursor.insertText(infoList.at(0));
         QTextCursor oldcursor = cursor;
         cursor = document->find("ADDRESS");
         if (!cursor.isNull()) {
             for (int i = 1; i < infoList.size(); ++i) {
                 cursor.insertBlock();
                 cursor.insertText(infoList.at(i));
             }
             cursor.endEditBlock();
         }
         else
             oldcursor.endEditBlock();
     }*/
 }

void MainWindow::onSelectPlaylist(const QString &playlistItem)
{
    Q_UNUSED(playlistItem);
    char path[PATH_MAX];
    sprintf (path, "%s", playList->currentItem()->text().toLocal8Bit().data());
}

int MainWindow::createAudioBranchElements(int index)
{
    if (index >= MAX_AUDIO_TRACKS){
        g_printerr ("Index = %d is out of range.\n", index);
        addColoredLog("E: Index is out of range.", MT_ERROR);
        return -1;
    }
    int i = index;
    char waveScopeName[32];
    char audioConvertName[32];
    char imageSinkName[32];
    char decodebinName[32];
    //char teeName[32];

    sprintf (waveScopeName, "scope_%d", i);
    sprintf (audioConvertName, "aconvert_%d", i);
    sprintf (imageSinkName, "asink_%d", i);
    sprintf (decodebinName, "decbin_%d", i);
    //sprintf (teeName, "tee_%d", i);

    //audioBranches[i].queue = gst_element_factory_make ("queue", "audio_queue");
    //audioBranches[i].decodebin = gst_element_factory_make ("decodebin", "decodebin");
    audioBranches[i].convert = gst_element_factory_make ("audioconvert", audioConvertName);
    audioBranches[i].wavescope = gst_element_factory_make ("wavescope", waveScopeName);
    //audioBranches[i].tee = gst_element_factory_make ("tee", teeName);
    //audioBranches[i].sound_queue = gst_element_factory_make ("queue", "sound_queue");
    //audioBranches[i].visual_queue = gst_element_factory_make ("queue", "visual_queue");
    //audioBranches[i].audiosink = gst_element_factory_make ("autoaudiosink", "audiosink");
    //audioBranches[i].typefind = gst_element_factory_make ("typefind", "typefind");

#if defined(Q_OS_MAC)
    audioBranches[i].imagesink = gst_element_factory_make ("glimagesink", imageSinkName);
#else
    audioBranches[i].imagesink = gst_element_factory_make ("ximagesink", imageSinkName);
#endif

    if (
        // !audioBranches[i].queue || !audioBranches[i].decodebin
        //||
           !audioBranches[i].imagesink || !audioBranches[i].wavescope
        || !audioBranches[i].convert
        //|| !audioBranches[i].typefind   // test
        //|| !audioBranches[i].tee || !audioBranches[i].sound_queue
        //|| !audioBranches[i].visual_queue || !audioBranches[i].audiosink
         )
    {
        g_printerr ("Not all elements could be created.\n");
        addColoredLog("E: " + QString::fromLocal8Bit(__FUNCTION__) + ": Not all elements could be created.", MT_ERROR);
        return -1;
    }

    g_object_set (audioBranches[i].wavescope, "shader", 0, "style", 3, NULL);
    g_object_set (audioBranches[i].imagesink, "name", imageSinkName, NULL);

    //g_signal_connect (audioBranches[i].decodebin, "pad-added", G_CALLBACK (on_new_decodebin_pad), audioBranches[i].tee);
    //g_signal_connect (audioBranches[i].decodebin, "pad-added", G_CALLBACK (on_new_decodebin_pad), audioBranches[i].convert);
    //g_signal_connect (audioBranches[i].decodebin, "autoplug-continue", G_CALLBACK (on_autoplug_continue), audioBranches[i].convert);
    //g_signal_connect (audioBranches[i].decodebin, "no-more-pads", G_CALLBACK (on_no_more_pads), NULL);
    //g_signal_connect (audioBranches[i].decodebin, "pad-added", G_CALLBACK (on_new_decodebin_pad), audioBranches[i].convert);
    //g_signal_connect (audioBranches[i].decodebin, "pad-added", G_CALLBACK (on_new_decodebin_pad), NULL);
    //g_signal_connect (audioBranches[i].decodebin, "pad-removed", G_CALLBACK (on_pad_removed), NULL);
    //g_signal_connect (audioBranches[i].decodebin, "have-type", G_CALLBACK (on_have_type), loop);


    // get sinkpad of first element to link it later
    //  it will be unreferenced in calling function
    //audioBranches[i].headSinkPad = gst_element_get_static_pad (audioBranches[i].queue, "sink"); // todo : carefull, look after unref
    audioBranches[i].headSinkPad = gst_element_get_static_pad (audioBranches[i].convert, "sink");
    g_printerr ("createAudioBranchElements OK.\n");
    return 0;
}

int MainWindow::linkAudioBranch(int trackNumber/*, GstPad * srcpad*/)
{
    // create a piece of pipeline for every audio track in mediafile
    // it splits audio for visual wavescope and playing sound
    // tee name=t ! queue !  audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink_%d t. ! queue ! autoaudiosink

    // update: remove tee and playing audio, just visualize it
    // queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink_%d    

    g_printerr ("linkAudioBranch: in. trackNumber = %d\n", trackNumber);

    if (trackNumber >= MAX_AUDIO_TRACKS){
        g_printerr ("Too much audio tracks.\n");
        addColoredLog("E: Too much audio tracks.", MT_ERROR);
        return NULL;
    }
    int i = trackNumber;

    /*int ret = createAudioBranchElements(i); // todo : create here of beforehand ?
    g_printerr ("linkAudioBranch: createAudioBranchElements rets %d\n", ret);
    if (ret < 0)
    {
        g_printerr ("linkAudioBranch: createAudioBranchElements failed\n");
        addColoredLog("E: linkAudioBranch: createAudioBranchElements failed", MT_ERROR);
        return -1;
    }*/

    gst_bin_add_many (GST_BIN (this->pipeline),
                      //audioBranches[i].queue, audioBranches[i].decodebin,
                      audioBranches[i].convert, audioBranches[i].wavescope, audioBranches[i].imagesink,

                      //audioBranches[i].typefind,

                      //audioBranches[i].tee, audioBranches[i].sound_queue, audioBranches[i].visual_queue,
                      //audioBranches[i].audiosink,
                      NULL);

    /*if (gst_element_link (audioBranches[i].queue, audioBranches[i].decodebin) != TRUE)
    {
        g_printerr ("Elements could not be linked.\n");
        addColoredLog("E: Elements could not be linked..", MT_ERROR);
        return -1;
    }*/

    // link elements with "always" pads
    //if (gst_element_link_many (visual_queue, convert, wavescope, imagesink, NULL) != TRUE ||
    //    gst_element_link_many (sound_queue, audiosink, NULL) != TRUE)
    //if (gst_element_link_many (queue, mpg123audiodec, convert, wavescope, imagesink, NULL) != TRUE)   // was here
    if (gst_element_link_many (audioBranches[i].convert, audioBranches[i].wavescope, audioBranches[i].imagesink, NULL) != TRUE)
    {
        g_printerr ("Elements could not be linked.\n");
        addColoredLog("E: Elements could not be linked..", MT_ERROR);
        return -1;
    }    

    /*// Manually link the Tee, which has "Request" pads
    tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (tee), "src%d");
    tee_audio_sound_pad = gst_element_request_pad (tee, tee_src_pad_template, NULL, NULL);
    g_print ("Obtained request pad %s for audio sound branch.\n", gst_pad_get_name (tee_audio_sound_pad));
    queue_audio_sound_pad = gst_element_get_static_pad (sound_queue, "sink");

    tee_audio_visual_pad = gst_element_request_pad (tee, tee_src_pad_template, NULL, NULL);
    g_print ("Obtained request pad %s for audio visual branch.\n", gst_pad_get_name (tee_audio_visual_pad));
    queue_audio_visual_pad = gst_element_get_static_pad (visual_queue, "sink");

    if (gst_pad_link (tee_audio_sound_pad, queue_audio_sound_pad) != GST_PAD_LINK_OK ||
        gst_pad_link (tee_audio_visual_pad, queue_audio_visual_pad) != GST_PAD_LINK_OK)
    {
        g_printerr ("%s(%d): Tee could not be linked\n", __FUNCTION__, __LINE__);
        addColoredLog("E: Tee could not be linked.", MT_ERROR);
        return -1;
    }
    gst_object_unref (queue_audio_sound_pad);
    gst_object_unref (queue_audio_visual_pad);
    */

    // create a dock and set it as a canvas for wavescope
    //addAudioDock(trackNumber);

    // todo : remove because it should be done in addAudioDock()
    //gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (audioBranches[i].imagesink), (guintptr)audioForm->winId());    // was here
    //gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (audioBranches[i].imagesink), (guintptr)audioWidgets[trackNumber]->winId());

    return 0;
}
/*
static void on_new_demux_pad(GstElement *element, GstPad *pad, gpointer data)
{
    GstPad *sinkpad;
    GstCaps *caps;
    GstStructure *str;
    MainWindow * mainwindow = (MainWindow*)data;
    BunchOfGstElements * bunch = &mainwindow->bunch;

    Q_UNUSED(element);

    caps = gst_pad_get_current_caps (pad);
    str = gst_caps_get_structure (caps, 0);
    gst_caps_unref (caps);

    if (g_strrstr (gst_structure_get_name (str), "video"))
    {
        sinkpad = gst_element_get_static_pad (bunch->queue, "sink");
        qDebug() <<  "on_new_demux_pad: Got queue sinkpad for video branch\n";
    }
    else if (g_strrstr (gst_structure_get_name (str), "audio"))
    {
        //sinkpad = gst_element_get_static_pad (bunch->decodebin, "sink");
        int audioTrack = mainwindow->bunch.audioTracks;
        if (mainwindow->linkAudioBranch(audioTrack) != 0)
        {
            qDebug() <<  "on_new_demux_pad: linkAudioBranch failed.\n";
            return;
        }
        sinkpad = mainwindow->audioBranches[audioTrack].headSinkPad; // todo: does it +1 refcount?
        qDebug() <<  "on_new_demux_pad: Got decodebin sinkpad for audio branch\n";

        // test
        gst_pad_link (pad, sinkpad);
        gst_object_unref (sinkpad);
        return;
        // end test
    }

    if (!gst_pad_is_linked (sinkpad)) {
        if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
            qDebug() <<  "on_new_demux_pad: Failed to link pads!\n";
        }
        else {
            qDebug() <<  "on_new_demux_pad: Link ok.\n";
        }
    }
    gst_object_unref (sinkpad);
}
*/
static void on_no_more_pads (GstElement * element, gpointer data)
{
    qDebug() <<  "on_new_decodebin_pad: IN\n";
}

static void on_autoplug_continue (GstElement *decodebin, GstPad *pad, GstCaps * gstcaps, gpointer data)
{
    qDebug() <<  "on_autoplug_continue: IN\n";
}

static void on_pad_removed (GstElement * element, GstPad * oldpad, gpointer data)
{
    qDebug() <<  "on_pad_removed: IN\n";

    GstCaps *caps;
    gchar *type;

    caps = gst_pad_get_current_caps (oldpad);
    type = gst_caps_to_string (caps);
    g_print ("on_pad_removed: Media type %s found\n", type);
    g_free (type);

    /*
    GstStructure *str;

    caps = gst_pad_get_current_caps (oldpad);
    str = gst_caps_get_structure (caps, 0);

    qDebug() << "on_pad_removed: Pad desc: " << gst_structure_get_name (str);*/
}

static void on_have_type (GstElement *typefind, guint probability, GstCaps *caps, gpointer data)
{
    qDebug() <<  "on_have_type: IN\n";
}

static void on_new_decodebin_pad (GstElement *decodebin, GstPad *pad, gpointer data)
{
    // current pipeline:
    // filesrc location=<path> ! decodebin name=dec ! xvimagesink dec. ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink

    qDebug() <<  "on_new_decodebin_pad: IN\n";

    GstPad *sinkpad;
    GstCaps *caps;
    GstStructure *str;
    MainWindow * window = (MainWindow *)data;

    Q_UNUSED(decodebin);

    caps = gst_pad_get_current_caps (pad);
    str = gst_caps_get_structure (caps, 0);

    if (g_strrstr (gst_structure_get_name (str), "video"))
    {
        sinkpad = gst_element_get_static_pad (window->bunch.xvimagesink, "sink");
        qDebug() <<  "on_new_decodebin_pad: Got sinkpad for video branch\n";

        if (!gst_pad_is_linked (sinkpad)) {
            if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
                qDebug() <<  "on_new_decodebin_pad: Failed to link pads!\n";
            }
        }
    }
    else if (g_strrstr (gst_structure_get_name (str), "audio"))
    {
        // test
        //gst_caps_unref (caps);
        //return;



        int i = window->bunch.audioTracks;
        if (window->linkAudioBranch(i) != 0)
        {
            qDebug() <<  "on_new_demux_pad: linkAudioBranch failed.\n";
            return;
        }
        sinkpad = window->audioBranches[i].headSinkPad;
        //sinkpad = gst_element_get_static_pad (window->audioBranches[i].convert, "sink");
        qDebug() <<  "on_new_decodebin_pad: Got sinkpad for audio branch\n";

        if (!gst_pad_is_linked (sinkpad)) {
            if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
                qDebug() <<  "on_new_decodebin_pad: Failed to link pads!\n";
            }
            else {
                //qDebug() <<  "on_new_decodebin_pad: gst_pad_unlink..\n";
                //gst_pad_unlink(pad, sinkpad);

                qDebug() <<  "on_new_decodebin_pad: Added audio branch\n";
                /*window->bunch.audioTracks ++;
                qDebug() <<  "on_new_decodebin_pad: gst_video_overlay_set_window_handle ...\n";
                gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (window->bunch.xvimagesink), (guintptr)window->glWidget->getWindowId());
*/
            }

        }
    }
    else {
        ;
        // todo : log stream name here, to better understand the stream structure
        qDebug() <<  "on_new_decodebin_pad: Strange stream found!\n";
    }

    gst_caps_unref (caps);
    gst_object_unref (sinkpad);

#if 0
    GstElement * nextElement = (GstElement*) data;

    // old way, not worked
    qDebug() <<  "on_new_decodebin_pad: IN\n";

    GstPad *sinkpad;
    GstCaps *caps;
    GstStructure *str;
    GstElement * nextElement = (GstElement*) data;

    Q_UNUSED(decodebin);

    caps = gst_pad_get_current_caps (pad);
    str = gst_caps_get_structure (caps, 0);

    if (g_strrstr (gst_structure_get_name (str), "audio"))
    {
        sinkpad = gst_element_get_static_pad (nextElement, "sink");
        qDebug() <<  "on_new_decodebin_pad: Got nextElement sinkpad for audio branch\n";

        if (!gst_pad_is_linked (sinkpad)) {
            if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
                qDebug() <<  "on_new_decodebin_pad: Failed to link pads!\n";
            }
        }
    }
    else {
        ;
        // todo : log stream name here, to better understand the stream structure
        qDebug() <<  "on_new_decodebin_pad: Strange stream found!\n";
    }

    gst_caps_unref (caps);
    gst_object_unref (sinkpad);
#endif
}

int MainWindow::createPipelineByString ()
{
    if (pipeline != NULL) {
        gst_element_set_state (pipeline, GST_STATE_NULL);
    }

    //QString pipelineString("filesrc location=" + playList->currentItem()->text() + " ! decodebin ! glimagesink name=vsink sync=false");   // plays ok
    //QString pipelineString("filesrc location=" + playList->currentItem()->text() + " ! decodebin ! autovideosink name=vsink sync=false"); // plays in separate window

#if defined(Q_OS_MAC)
    QString pipelineString("filesrc location=" + playList->currentItem()->text() +
      //                     " ! decodebin name=dec ! queue ! glimagesink name=vsink dec. ! audioconvert ! wavescope shader=0 style=3 ! glimagesink name=asink");
    // for eg., /Users/qa/Desktop/media/atomic.ts
    " ! tsdemux name=demux ! queue ! avdec_mpeg2video ! glimagesink name=vsink ");

    // todo : use more stable version of gst-discoverer
    // and use it for demux selection and pipeline creation

    for (int i=0; i<bunch.audioTracks; i++){
        char audioBranch[256];
        //snprintf (audioBranch, 256, "demux.audio_%d ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! glimagesink name=asink_%d", i, i);
        snprintf (audioBranch, 256, "demux. ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! glimagesink name=asink_%d", i, i);
        pipelineString.append(audioBranch);
    }
 #elif defined(Q_OS_UNIX)

    /*
    QString pipelineString("filesrc location=" + playList->currentItem()->text() +
      // ok
       //" ! decodebin name=dec ! queue ! glimagesink name=vsink dec. ! audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink");

    // test, works but shows garbage on top left corner
     //" ! decodebin name=dec ! queue ! xvimagesink name=vsink dec. ! tee name=t ! queue !  audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink t. ! queue ! autoaudiosink");



      // works but video flashes
      //" ! tsdemux name=demux ! queue ! mpeg2dec ! xvimagesink name=vsink demux. ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink");

     // test, works but shows garbage on top left corner
       // " ! tsdemux name=demux ! queue ! mpeg2dec ! xvimagesink name=vsink demux. ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink");


  " ! decodebin name=dec ! xvimagesink name=vsink dec. ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink"); // was ok for one audio


   //" ! decodebin name=dec ! xvimagesink name=vsink "); // for appending several audio


   */

    /*
     gst-launch-1.0 filesrc location=/home/vq/Видео/billiards.mkv !
        matroskademux name=demux \
        demux.video_0 ! queue ! h264parse ! eavcdec ! autovideosink \
        demux.audio_0 ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink \
        demux.audio_1 ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink \
        demux.audio_2 ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink \
        demux.audio_3 ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink
*/
/*
    // for /home/vq/Видео/Paradise.mp4
    QString pipelineString("filesrc location=" + playList->currentItem()->text() +
 "! qtdemux name=demux demux.video_0 ! queue ! h264parse ! eavcdec ! xvimagesink name=vsink ");
    // demux.audio_0 ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink"
*/
/*
    // for /home/vq/Видео/atomic.ts
    QString pipelineString("filesrc location=" + playList->currentItem()->text() +
 " ! tsdemux name=demux demux.video_0 ! queue ! mpeg2dec ! xvimagesink name=vsink ");
    // demux.audio_0 ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink"
*/
    // for /home/vq/Видео/atomic.ts
    QString pipelineString("filesrc location=" + playList->currentItem()->text() +
 " ! tsdemux name=demux ! queue ! mpeg2dec ! xvimagesink name=vsink ");
    // demux. ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink_0

    // todo : make video branch and test it
    // todo : use more stable version of gst-discoverer
    // and use it for demux selection and pipeline creation

    for (int i=0; i<bunch.audioTracks; i++){
        char audioBranch[256];
        //snprintf (audioBranch, 256, "demux.audio_%d ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink_%d", i, i);
        snprintf (audioBranch, 256, "demux. ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink_%d", i, i);
        pipelineString.append(audioBranch);
    }

 #elif defined(Q_OS_WIN)
    QString pipelineString("filesrc location=" + playList->currentItem()->text() +
                           " ! decodebin name=dec ! queue ! glimagesink name=vsink dec. ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink");
 #endif
    char pipelineChars[PATH_MAX];
    sprintf (pipelineChars, "%s", pipelineString.toLocal8Bit().data());
    //sprintf (pipelineChars, "%s", pipelineString.toLatin1().data());
    //sprintf (pipelineChars, "%s", pipelineString.toStdString().c_str());

    addColoredLog("I: Pipeline = " + pipelineString, MT_DEBUG);
    qDebug() << "I: Pipeline = " << pipelineChars;

    // todo : seems glimagesink plays i-frames only
    pipeline = gst_parse_launch(pipelineChars, NULL);
    if (!pipeline) {
        addColoredLog("E: createPipelineByString: gst_parse_launch failed", MT_DEBUG);
        qDebug() << "E: createPipelineByString: gst_parse_launch failed\n";
        return -1;
    }
    GstElement * vsink = gst_bin_get_by_name (GST_BIN (pipeline), "vsink");
    if (!vsink) {
        return -1;
    }
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (vsink), (guintptr)videoWidget->getWindowId());
    gst_object_unref (vsink);

    gst_element_set_state (pipeline, GST_STATE_PLAYING);


    removeAudioDocks();
    for (int i=0; i<bunch.audioTracks; i++){
        addAudioDock(i);
    }

    return 0;
}

int MainWindow::createPipelineByCode ()
{
    if (pipeline != NULL) {
        g_printerr ("createPipelineByCode: gst_element_set_state NULL\n");
        gst_element_set_state (pipeline, GST_STATE_NULL);

        // todo : cleanup here ?

        gst_object_unref (pipeline);
    }

    // todo : count number of audiotracks
    // then call createAudioPipelineBranch(int trackNumber)

    // test --------------------------------------------------------------------
#if defined(Q_OS_MAC)
    // todo : code for mac-specific element creation
#else
    // ok
    //"filesrc location=" + playList->currentItem()->text() + ! decodebin name=dec ! queue ! glimagesink name=vsink dec. ! audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink");

    // test, works but shows garbage on top left corner
     //" ! decodebin name=dec ! queue ! xvimagesink name=vsink dec. ! tee name=t ! queue !  audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink t. ! queue ! autoaudiosink");

      // works but video flashes
      //" ! tsdemux name=demux ! queue ! mpeg2dec ! xvimagesink name=vsink demux. ! queue ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink");

     // test, works but shows garbage on top left corner
       // " ! tsdemux name=demux ! queue ! mpeg2dec ! xvimagesink name=vsink demux. ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink");

 #endif

    pipeline = gst_pipeline_new (NULL);

    bunch.filesrc = gst_element_factory_make ("filesrc", "filesrc");
    //bunch.queue = gst_element_factory_make ("queue", "video_queue");
    //bunch.tsdemux = gst_element_factory_make ("tsdemux", "tsdemux");

    bunch.decodebin = gst_element_factory_make ("decodebin", "decodebin");

    bunch.xvimagesink = findVideosink();
/*
#if defined(Q_OS_MAC)
    bunch.mpeg2dec = gst_element_factory_make("avdec_mpeg2video", "mpeg2dec");
    //bunch.xvimagesink = gst_element_factory_make ("glimagesink", "xvimagesink");
#else
    bunch.mpeg2dec = gst_element_factory_make("mpeg2dec", "mpeg2dec");
    //bunch.xvimagesink = gst_element_factory_make ("xvimagesink", "xvimagesink");
#endif
*/
    // test
    for (unsigned int i=0; i<MAX_AUDIO_TRACKS; i++)
    {
        createAudioBranchElements(i);
    }


    g_object_set (G_OBJECT (bunch.filesrc), "location", playList->currentItem()->text().toLocal8Bit().data(), NULL);
    g_object_set (bunch.xvimagesink, "name", "vsink", NULL);
    //g_object_set (bunch.tsdemux, "name", "demux", NULL);
    g_object_set (bunch.decodebin, "name", "dec", NULL);

    //if (!pipeline || !bunch.xvimagesink || !bunch.filesrc || !bunch.mpeg2dec || !bunch.queue || !bunch.tsdemux)
    if (!pipeline || !bunch.xvimagesink || !bunch.filesrc || !bunch.decodebin)
    {
        g_printerr ("%s(%d): One of the GStreamer decoding elements is missing\n", __FUNCTION__, __LINE__);
        addColoredLog("E: One of the GStreamer decoding elements is missing.", MT_ERROR);
        return -1;
    }

    //gst_bin_add_many (GST_BIN (pipeline), bunch.filesrc, bunch.tsdemux, bunch.queue, bunch.mpeg2dec, bunch.xvimagesink, NULL);
    gst_bin_add_many (GST_BIN (pipeline), bunch.filesrc, bunch.decodebin, bunch.xvimagesink, NULL);

    //g_signal_connect (bunch.tsdemux, "pad-added", G_CALLBACK (on_new_demux_pad), this);
    g_signal_connect (bunch.decodebin, "pad-added", G_CALLBACK (on_new_decodebin_pad), this);

    // old:
    // ! tsdemux name=demux ! queue ! mpeg2dec ! xvimagesink name=vsink demux. ! decodebin ! audioconvert ! wavescope shader=0 style=3 ! videoconvert ! ximagesink name=asink
    //gst_element_link (bunch.filesrc, bunch.tsdemux);
    //gst_element_link_many(bunch.queue, bunch.mpeg2dec, bunch.xvimagesink, NULL);

    // current pipeline:
    // filesrc location=/home/vq/Видео/atomic.ts ! decodebin name=dec ! xvimagesink dec. ! audioconvert ! wavescope shader=0 style=3 ! ximagesink name=asink
    gst_element_link(bunch.filesrc, bunch.decodebin);

    //GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    //gst_bus_add_watch(bus, (GstBusFunc) bus_call, this);
    //gst_object_unref(bus);

    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    //gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (bunch.xvimagesink), (guintptr)glWidget->getWindowId());

    // todo : remove it - this is now done in createAudioPipelineBranch()
    /*GstElement * audiowave = gst_bin_get_by_name (GST_BIN (pipeline), "asink_0");
    if (audiowave){
        gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (audiowave), (guintptr)audioForm->winId());
        gst_object_unref (audiowave);
    }
    else
    {
        g_printerr ("createPipelineByCode: ERROR: asink_0 not found!\n");
    }*/

    return 0;
}

void MainWindow::onDoubleClick(const QModelIndex &modelIndex)
{
    Q_UNUSED (modelIndex);

    qDebug() << "onDoubleClick: insertMediaInfo " << playList->currentItem()->text();
    insertMediaInfo(playList->currentItem()->text().toLocal8Bit().data());

    qDebug() << "onDoubleClick: createPipeline ...";

    //if (createPipelineByCode() != 0){
    if (createPipelineByString() != 0){
        addColoredLog("E: Couldn't create pipeline.", MT_ERROR);
        return;
    }

    // todo : where to unref pipeline ??
    //gst_element_set_state (pipeline, GST_STATE_NULL);
    //gst_object_unref (pipeline);

    //
    // VLC version
    //
    // Create a new Media
    //libvlc_media_t *vlcMedia = libvlc_media_new_path(vlcInstance, playList->currentItem()->text().toStdString().c_str());
    //if (!vlcMedia)
    //    return;

    // Create a new libvlc player
    //vlcPlayer = libvlc_media_player_new_from_media (vlcMedia);

    // Release the media
    //libvlc_media_release(vlcMedia);

    // Integrate the video in the interface
    /*
#if defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(vlcPlayer, (void *)label->winId());
#elif defined(Q_OS_UNIX)
    libvlc_media_player_set_xwindow(vlcPlayer, label->winId());
#elif defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(vlcPlayer, label->winId());
#endif
    */
    // And start playback
    //libvlc_media_player_play (vlcPlayer);
 }

/* Print a tag in a human-readable format (name: value) */
static void print_tag_foreach (const GstTagList *tags, const gchar *tag, gpointer user_data)
{
  GValue val = { 0, };
  gchar *str;
  MainWindow * mainWindow = (MainWindow*)user_data;

  gst_tag_list_copy_value (&val, tags, tag);

  if (G_VALUE_HOLDS_STRING (&val))
    str = g_value_dup_string (&val);
  else
    str = gst_value_serialize (&val);

  mainWindow->addColoredLog("I:      " + QString::fromLocal8Bit(gst_tag_get_nick (tag)) +
                            ": " + QString::fromLocal8Bit(str), MT_INFO);

  qDebug() << "print_tag_foreach: " << QString::fromLocal8Bit(gst_tag_get_nick (tag)) << ": " << QString::fromLocal8Bit(str);

  g_free (str);

  g_value_unset (&val);
}

static void print_stream_info (GstDiscovererStreamInfo *info, gpointer user_data)
{
    gchar *desc = NULL;
    GstCaps *caps;
    const GstTagList *tags;
    MainWindow * mainWindow = (MainWindow*)user_data;

    caps = gst_discoverer_stream_info_get_caps (info);

    if (caps) {
       if (gst_caps_is_fixed (caps))
            desc = gst_pb_utils_get_codec_description (caps);
        else
            desc = gst_caps_to_string (caps);
        gst_caps_unref (caps);
    }

    mainWindow->addColoredLog("I: " + QString::fromLocal8Bit(gst_discoverer_stream_info_get_stream_type_nick (info)) +
                              ": " + QString::fromLocal8Bit((desc ? desc : "")), MT_INFO);

    qDebug() << "print_stream_info" << QString::fromLocal8Bit(gst_discoverer_stream_info_get_stream_type_nick (info)) << ": " + QString::fromLocal8Bit((desc ? desc : ""));

    if (desc) {
        g_free (desc);
        desc = NULL;
    }

    if (strstr (gst_discoverer_stream_info_get_stream_type_nick (info), "audio")){
        mainWindow->bunch.audioTracks ++;
        qDebug() << "print_stream_info: Found audio track";
    }

    tags = gst_discoverer_stream_info_get_tags (info);
    if (tags) {
        //gst_tag_list_foreach (tags, print_tag_foreach, user_data);
        //mainWindow->addColoredLog("", MT_NONE);
    }
}


/* Print information regarding a stream and its substreams, if any */
static void print_topology (GstDiscovererStreamInfo *info, gpointer user_data)
{
  GstDiscovererStreamInfo *next;

  if (!info)
    return;

  print_stream_info (info, user_data);

  next = gst_discoverer_stream_info_get_next (info);
  if (next) {
    print_topology (next, user_data);
    gst_discoverer_stream_info_unref (next);
  } else if (GST_IS_DISCOVERER_CONTAINER_INFO (info)) {
    GList *tmp, *streams;

    streams = gst_discoverer_container_info_get_streams (GST_DISCOVERER_CONTAINER_INFO (info));
    for (tmp = streams; tmp; tmp = tmp->next) {
      GstDiscovererStreamInfo *tmpinf = (GstDiscovererStreamInfo *) tmp->data;
      print_topology (tmpinf, user_data);
    }
    gst_discoverer_stream_info_list_free (streams);
  }
}
/* This function is called every time the discoverer has information regarding
 * one of the URIs we provided.*/
static void on_discovered_cb (GstDiscoverer *discoverer, GstDiscovererInfo *info, GError *err, MainWindow * mainWindow)
{
    Q_UNUSED(discoverer);

  GstDiscovererResult result;
  const gchar *uri;
  const GstTagList *tags;
  GstDiscovererStreamInfo *sinfo;

  uri = gst_discoverer_info_get_uri (info);
  result = gst_discoverer_info_get_result (info);
  switch (result) {
    case GST_DISCOVERER_URI_INVALID:
      mainWindow->addColoredLog("E: Invalid URI " + QString::fromLocal8Bit(uri), MT_ERROR);
      break;
    case GST_DISCOVERER_ERROR:
      mainWindow->addColoredLog("E: Discoverer error: " + QString::fromLocal8Bit(err->message), MT_ERROR);
      break;
    case GST_DISCOVERER_TIMEOUT:
      mainWindow->addColoredLog("E: Timeout.", MT_WARNING);
      break;
    case GST_DISCOVERER_BUSY:
      mainWindow->addColoredLog("E: Busy.", MT_WARNING);
      break;
    case GST_DISCOVERER_MISSING_PLUGINS:{
      const GstStructure *s;
      gchar *str;

      s = gst_discoverer_info_get_misc (info);
      str = gst_structure_to_string (s);

      mainWindow->addColoredLog("E: Missing plugins: " + QString::fromLocal8Bit(str), MT_ERROR);
      g_free (str);
      break;
    }
    case GST_DISCOVERER_OK:
      mainWindow->addColoredLog("I: Discovered " + QString::fromLocal8Bit(uri), MT_INFO);
      break;
  }

  if (result != GST_DISCOVERER_OK) {
      mainWindow->addColoredLog("E: This URI cannot be played ", MT_ERROR);
      return;
  }

  /* If we got no error, show the retrieved information */

#define TIME_FORMAT_MSEC(t) GST_CLOCK_TIME_IS_VALID (t) ? (guint) (((GstClockTime)(t)) % GST_SECOND) : 999999999
#define TIME_FORMAT_SEC(t)  GST_CLOCK_TIME_IS_VALID (t) ? (guint) ((((GstClockTime)(t)) / GST_SECOND) % 60) : 99
#define TIME_FORMAT_MIN(t)  GST_CLOCK_TIME_IS_VALID (t) ? (guint) ((((GstClockTime)(t)) / (GST_SECOND * 60)) % 60) : 99
#define TIME_FORMAT_HOUR(t) GST_CLOCK_TIME_IS_VALID (t) ? (guint) (((GstClockTime)(t)) / (GST_SECOND * 60 * 60)) : 99

  GstClockTime duration = gst_discoverer_info_get_duration (info);
  mainWindow->addColoredLog("I: Duration: " +
                            QString::number(TIME_FORMAT_HOUR(duration)) + ":" +
                            QString::number(TIME_FORMAT_MIN(duration)) + ":" +
                            QString::number(TIME_FORMAT_SEC(duration)) + ":" +
                            QString::number(TIME_FORMAT_MSEC(duration)) + ":",
                            MT_INFO);

  qDebug("\nDuration: %u:%02u:%02u:%09u", GST_TIME_ARGS (duration));

  tags = gst_discoverer_info_get_tags (info);
  if (tags) {
      //gst_tag_list_foreach (tags, print_tag_foreach, (gpointer)mainWindow);
      //mainWindow->addColoredLog("", MT_NONE);
  }

  mainWindow->addColoredLog("I: Seekable: " +
                            QString::fromLocal8Bit(gst_discoverer_info_get_seekable (info) ? "yes" : "no") + "\n", MT_INFO);

  sinfo = gst_discoverer_info_get_stream_info (info);
  if (!sinfo)
    return;

  mainWindow->addColoredLog("I: Stream information:\n", MT_INFO);

  print_topology (sinfo, (gpointer)mainWindow);

  gst_discoverer_stream_info_unref (sinfo);
}

/* This function is called when the discoverer has finished examining
 * all the URIs we provided.*/
static void on_finished_cb (GstDiscoverer *discoverer, MainWindow * mainWindow)
{
    Q_UNUSED(discoverer);

    mainWindow->addColoredLog("I: Finished discovering.", MT_INFO);
    mainWindow->addColoredLog("", MT_NONE);

    g_main_loop_quit (mainWindow->loop);
}

 void MainWindow::about()
 {
    QMessageBox::about(this, tr("About Dock Widgets"),
             tr("The <b>Dock Widgets</b> example demonstrates how to "
                "use Qt's dock widgets. You can enter your own text, "
                "click a customer to add a customer name and "
                "address, and click standard paragraphs to add them."));
 }

 void MainWindow::createActions()
 {
     quitAct = new QAction(tr("&Quit"), this);
     quitAct->setShortcuts(QKeySequence::Quit);
     quitAct->setStatusTip(tr("Quit the application"));
     connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

     aboutAct = new QAction(tr("&About"), this);
     aboutAct->setStatusTip(tr("Show the application's About box"));
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

     aboutQtAct = new QAction(tr("About &Qt"), this);
     aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
 }

 void MainWindow::createMenus()
 {
     fileMenu = menuBar()->addMenu(tr("&File"));
     fileMenu->addAction(quitAct);

     viewMenu = menuBar()->addMenu(tr("&View"));

     menuBar()->addSeparator();

     helpMenu = menuBar()->addMenu(tr("&Help"));
     helpMenu->addAction(aboutAct);
     helpMenu->addAction(aboutQtAct);
 }

 void MainWindow::createToolBars()
 {
     // todo : fill toolbars with useful actions
     fileToolBar = addToolBar(tr("File"));
     //fileToolBar->addAction(saveAct);
     //fileToolBar->addAction(printAct);

     editToolBar = addToolBar(tr("Edit"));
     //editToolBar->addAction(undoAct);
 }

 void MainWindow::createStatusBar()
 {
     statusBar()->showMessage(tr("Ready"));
 }

void MainWindow::createCentralWidget()
{
    QGLFormat glFormat;
    glFormat.setVersion (2, 1);
    glFormat.setProfile (QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers (true);

    videoWidget = new VideoWidget(glFormat, this);
    setCentralWidget(videoWidget);
}

void MainWindow::createDiscoverer()
{
    GError * err = NULL;
    discoverer = gst_discoverer_new (5 * GST_SECOND, &err);
    if (!discoverer) {
      addColoredLog("E: Error creating discoverer instance: " + QString::fromLocal8Bit(err->message), MT_ERROR);
      g_clear_error (&err);
      return;
    }

    g_signal_connect (discoverer, "discovered", G_CALLBACK (on_discovered_cb), this);
    g_signal_connect (discoverer, "finished", G_CALLBACK (on_finished_cb), this);

    loop = g_main_loop_new (NULL, FALSE);
}

void MainWindow::removeAudioDocks()
{
    for (int i=0; i<numberOfAudioDocks; i++){
        QMainWindow::removeDockWidget(audioDocks[i]);
    }
}
/*
void MainWindow::showEvent(QShowEvent *)
{
    for (int i=0; i<numberOfAudioDocks; i++){
        if (audioDocks[i]->height() < 200){
            audioDocks[i]->resize(audioDocks[i]->width(), 200);
        }
    }
}*/

void MainWindow::addAudioDock(int trackNumber)
{    
    char caption[64];
    char elementName[32];
    sprintf (elementName, "asink_%d", trackNumber);
    sprintf (caption, "Audio track #%d", trackNumber);

    QDockWidget * audioDock = new QDockWidget(tr(caption), this);
    //QGLWidget * audioWidget = new QGLWidget(audioDock);
    AudioWidget * audioWidget = new AudioWidget(audioDock);

    audioDock->setWidget(audioWidget);
    addDockWidget(Qt::RightDockWidgetArea, audioDock);
    viewMenu->addAction(audioDock->toggleViewAction());
    audioDocks[numberOfAudioDocks++] = audioDock;

    GstElement * waveElement = gst_bin_get_by_name (GST_BIN (pipeline), elementName);
    if (waveElement != NULL)
    {
        gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (waveElement), (guintptr)audioWidget->winId());
        gst_object_unref (waveElement);
        g_printerr ("addAudioDock: gst_video_overlay_set_window_handle OK.\n");
    }
    else
    {
        addColoredLog("E: Audiosink element named " + QString::fromLocal8Bit(elementName) + " not found.", MT_ERROR);
    }

    // todo : make more complicated dock with labels, playbuttons
    // todo : add slots for play\pause buttons
}

 void MainWindow::createDockWindows()
 {
     QDockWidget *dock;
     dock = new QDockWidget (tr("Playlist"), this);
     playList = new QListWidget(dock);

     dock->setWidget(playList);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());

     dock = new QDockWidget (tr("Codec info"), this);
     codecInfo = new QTextEdit;
     dock->setWidget(codecInfo);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());
/*
     QTextDocument *document = codecInfo->document();
     QTextCursor cursor(document);
     cursor.beginEditBlock();
     cursor.insertText("Select file in playlist to get it's codec information");
     cursor.endEditBlock();
*/
     dock = new QDockWidget (tr("Messages"), this);
     messageList = new QListWidget(dock);
     dock->setWidget(messageList);
     addDockWidget(Qt::BottomDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());

     /*// todo : remove it - this is now done in createAudioPipelineBranch()
     dock = new QDockWidget (tr("Audio track #0"), this);
     audioForm = new QGLWidget(dock);
     dock->setWidget(audioForm);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());
*/  
     connect (playList, SIGNAL(currentTextChanged(QString)), this, SLOT(onSelectPlaylist(QString)));
     connect (playList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClick(QModelIndex)));
 }

 void MainWindow::createVlc()
 {
     QMessageBox::critical(this, "VLC_PLUGIN_PATH", getenv("VLC_PLUGIN_PATH"));
     /* Initialize libVLC */
     //vlcInstance = libvlc_new(0, NULL);

     /* Complain in case of broken installation */
     //if (vlcInstance == NULL) {
     //    QMessageBox::critical(this, "Qt libVLC player", "Could not init libVLC");
     //    exit(1);
     //}
 }

 void MainWindow::cleanup()
 {
     /*for (int i=0; i<bunch.audioTracks; i++){
         if (bunch.tees[i] != NULL){
             gst_element_release_request_pad (bunch.tees[i], bunch.tee_audio_sound_pad[i]);
             gst_element_release_request_pad (bunch.tees[i], bunch.tee_audio_visual_pad[i]);
             gst_object_unref (bunch.tee_audio_sound_pad[i]);
             gst_object_unref (bunch.tee_audio_visual_pad[i]);

             // todo : carefull
             gst_object_unref (bunch.tees[i]);

             // todo : release all other bunch elements ?

         }
     }*/

     g_printerr ("createPipelineByCode: gst_element_set_state NULL\n");
     gst_element_set_state (pipeline, GST_STATE_NULL);
     gst_object_unref (pipeline);

     g_object_unref (discoverer);
     g_main_loop_unref (loop);

     /* Release libVLC instance on quit */
     //if (vlcInstance) {
     //    libvlc_release(vlcInstance);
     //}

     return;
 }

 MainWindow::~MainWindow()
 {
     // todo: where create dot-file? it should be completely linked for now
     // todo : make sure graphviz is installed on every system that uses ths app
     // todo : find place to paint generated PNG on dock canvas
     GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(/*mainwindow->*/pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline_graph");
     g_printerr ("Dtor: dot -Tpng pipeline_graph.dot > pipeline_graph.png\n");
     system ("dot -Tpng pipeline_graph.dot > pipeline_graph.png");

     // todo : cleanup every time video stops
     cleanup();
 }
