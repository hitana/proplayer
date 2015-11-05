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
static GstElement * find_video_sink (void)
{
  GstStateChangeReturn sret;
  GstElement *sink;

  if ((sink = gst_element_factory_make ("xvimagesink", NULL))) {
    sret = gst_element_set_state (sink, GST_STATE_READY);
    if (sret == GST_STATE_CHANGE_SUCCESS){
        qDebug() << "found xvimagesink";
        return sink;
    }

    gst_element_set_state (sink, GST_STATE_NULL);
    gst_object_unref (sink);
  }

  if ((sink = gst_element_factory_make ("ximagesink", NULL))) {
    sret = gst_element_set_state (sink, GST_STATE_READY);
    if (sret == GST_STATE_CHANGE_SUCCESS){
        qDebug() << "found ximagesink";
        return sink;
    }

    gst_element_set_state (sink, GST_STATE_NULL);
    gst_object_unref (sink);
  }

  if ((sink = gst_element_factory_make ("osxvideosink", NULL))) {
    sret = gst_element_set_state (sink, GST_STATE_READY);
    if (sret == GST_STATE_CHANGE_SUCCESS){
        qDebug() << "found osxvideosink";
        return sink;
    }

    gst_element_set_state (sink, GST_STATE_NULL);
    gst_object_unref (sink);
  }

  if ((sink = gst_element_factory_make ("autovideosink", NULL))) {
      sret = gst_element_set_state (sink, GST_STATE_READY);
      if (sret == GST_STATE_CHANGE_SUCCESS){
          qDebug() << "found autovideosink";
          return sink;
      }

      gst_element_set_state (sink, GST_STATE_NULL);
      gst_object_unref (sink);
    }

  //if (strcmp (DEFAULT_VIDEOSINK, "xvimagesink") == 0 ||
  //    strcmp (DEFAULT_VIDEOSINK, "ximagesink") == 0)
  //  return NULL;

  if ((sink = gst_element_factory_make (DEFAULT_VIDEOSINK, NULL))) {
    if (GST_IS_BIN (sink)) {
      gst_object_unref (sink);
      return NULL;
    }

    sret = gst_element_set_state (sink, GST_STATE_READY);
    if (sret == GST_STATE_CHANGE_SUCCESS)
 {
        qDebug() << "found " << DEFAULT_VIDEOSINK;
        return sink;
    }

    gst_element_set_state (sink, GST_STATE_NULL);
    gst_object_unref (sink);
  }

  return NULL;
}

 MainWindow::MainWindow()
     :pipeline(NULL)
 {
     gst_init (NULL, NULL);

     createActions();
     createMenus();
     createToolBars();
     createStatusBar();
     createCentralWidget();
     createDockWindows();
     createDiscoverer();
     //createVlc();

     setWindowTitle(tr("ProPlayer Skeleton App"));

     setUnifiedTitleAndToolBarOnMac(true);

     setAcceptDrops(true);
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

 void MainWindow::insertMediaInfo (const char *uri)
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

     QString mediadata;
     GError *err = NULL;
     //gchar *uri = "http://docs.gstreamer.com/media/sintel_trailer-480p.webm";



       //cursor.insertBlock();
       //cursor.beginEditBlock();
       cursor.insertText("Discovering ");
       cursor.insertText(uri);
       //cursor.endEditBlock();

       qDebug() << "Discovering " << uri;

       // todo : write error messages with icons(warning, error, info) to Messages dock.

       /* Start the discoverer process (nothing to do yet) */
       gst_discoverer_start (discoverer);

       /* Add a request to process asynchronously the URI passed through the command line */
       if (!gst_discoverer_discover_uri_async (discoverer, uri)) {
         qDebug() << "Failed to start discovering URI " << uri;
         g_object_unref (discoverer);
         return;
       }

       /* Set GLib Main Loop to run, so we can wait for the signals */
       //loop = g_main_loop_new (NULL, FALSE);
       g_main_loop_run (loop);

       /* Stop the discoverer process */
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
     qDebug() << "Selected " << playlistItem;

     char path[PATH_MAX];
     sprintf (path, "%s", playList->currentItem()->text().toLocal8Bit().data());
     insertMediaInfo(path);
 }

void MainWindow::onDoubleClick(const QModelIndex &modelIndex)
{
#if 0
    // working ok
    if (pipeline != NULL) {
        gst_element_set_state (pipeline, GST_STATE_NULL);
    }

    QString pipelineString("filesrc location=" + playList->currentItem()->text() + " ! decodebin ! glimagesink name=vsink sync=false");
    //QString pipelineString("filesrc location=" + playList->currentItem()->text() + " ! decodebin ! autovideosink name=vsink sync=false"); // plays in separate window
    char pipelineChars[PATH_MAX];
    sprintf (pipelineChars, "%s", pipelineString.toLocal8Bit().data());
    //sprintf (pipelineChars, "%s", pipelineString.toLatin1().data());
    //sprintf (pipelineChars, "%s", pipelineString.toStdString().c_str());

    qDebug() << "Playing " << playList->currentRow() << ", " << playList->currentItem()->text();
    qDebug() << "pipeline = " << pipelineString;

    // todo : seems glimagesink plays i-frames only
    pipeline = gst_parse_launch(pipelineChars, NULL);
    GstElement * vsink = gst_bin_get_by_name (GST_BIN (pipeline), "vsink");
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (vsink), (guintptr)glWidget->getWindowId());
    gst_object_unref (vsink);
    GstStateChangeReturn sret = gst_element_set_state (pipeline, GST_STATE_PLAYING);

    GstBus * bus = gst_element_get_bus (pipeline);
    //GstMessage * msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS)); // blocks whole app

    // todo : clear somewhere
    // if (msg != NULL) gst_message_unref (msg);
    gst_object_unref (bus);
    //gst_element_set_state (pipeline, GST_STATE_NULL);
    //gst_object_unref (pipeline);

#endif
#if 1

    // todo : lego pipeline by hand
    GstElement *src, *sink, *bin;
    GstBus * bus;
    GstMessage * msg;
    GstStateChangeReturn ret;

    if (pipeline != NULL) {
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (pipeline);    // todo : test it
    }

    //src = gst_element_factory_make("filesrc", "src");
    //sink = gst_element_factory_make("glimagesink", "sink");
    src = gst_element_factory_make("videotestsrc", "src");
    //sink = gst_element_factory_make("autovideosink", "sink");
    sink = gst_element_factory_make("glimagesink", "sink");
    //bin = gst_element_factory_make("decodebin", "bin");
    bin = gst_element_factory_make("videoconvert", "bin");
    pipeline = gst_pipeline_new("simple_pipeline");

    if (!src || !sink || !bin || !pipeline){
        qDebug() << "Not all elements could be created.";
        return;
    }
    //gst_bin_add_many (GST_BIN (pipeline), src, sink, NULL);
    gst_bin_add_many (GST_BIN (pipeline), src, sink, bin, NULL);
    if (gst_element_link_many(src, bin, sink) != TRUE) {
    //if (gst_element_link(src, sink) != TRUE) {
        qDebug() << "Elements could not be linked.";
        gst_object_unref (pipeline);    // ??
        return;
    }
    //g_object_set(src, "location", playList->currentItem()->text().toLocal8Bit().data(), NULL);
    g_object_set(src, "pattern", 0, NULL);

    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (sink), (guintptr)glWidget->getWindowId());
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE) {
        qDebug() << "Unable to set the pipeline to the playing state.";
        gst_object_unref (pipeline);
        return;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline);
    //msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // todo : not here!
    // Free resources
    //gst_message_unref (msg);
    //gst_object_unref (bus);
    //gst_element_set_state (pipeline, GST_STATE_NULL);
    //gst_object_unref (pipeline);


#endif


    // todo : get a bus and set some callbacks to gather media metadata
    // then show metadata somewhere on dock

     /* Create a new Media */
     //libvlc_media_t *vlcMedia = libvlc_media_new_path(vlcInstance, playList->currentItem()->text().toStdString().c_str());
     //if (!vlcMedia)
     //    return;

     /* Create a new libvlc player */
     //vlcPlayer = libvlc_media_player_new_from_media (vlcMedia);

     /* Release the media */
     //libvlc_media_release(vlcMedia);

     /* Integrate the video in the interface */
/*
#if defined(Q_OS_MAC)
     libvlc_media_player_set_nsobject(vlcPlayer, (void *)label->winId());
 #elif defined(Q_OS_UNIX)
     libvlc_media_player_set_xwindow(vlcPlayer, label->winId());
 #elif defined(Q_OS_WIN)
     libvlc_media_player_set_hwnd(vlcPlayer, label->winId());
 #endif
*/
     /* And start playback */
     //libvlc_media_player_play (vlcPlayer);
 }

/* Print a tag in a human-readable format (name: value) */
static void print_tag_foreach (const GstTagList *tags, const gchar *tag, gpointer user_data)
{
  GValue val = { 0, };
  gchar *str;
  gint depth = GPOINTER_TO_INT (user_data);

  gst_tag_list_copy_value (&val, tags, tag);

  if (G_VALUE_HOLDS_STRING (&val))
    str = g_value_dup_string (&val);
  else
    str = gst_value_serialize (&val);

  qDebug() << gst_tag_get_nick (tag) << " " << str;
  g_free (str);

  g_value_unset (&val);
}

static void print_stream_info (GstDiscovererStreamInfo *info, gint depth)
{
    gchar *desc = NULL;
    GstCaps *caps;
    const GstTagList *tags;

    caps = gst_discoverer_stream_info_get_caps (info);

    if (caps) {
       if (gst_caps_is_fixed (caps))
            desc = gst_pb_utils_get_codec_description (caps);
        else
            desc = gst_caps_to_string (caps);
        gst_caps_unref (caps);
    }

    qDebug() << gst_discoverer_stream_info_get_stream_type_nick (info) << ": " << (desc ? desc : "") << "\n";

    if (desc) {
        g_free (desc);
        desc = NULL;
    }

    tags = gst_discoverer_stream_info_get_tags (info);
    if (tags) {
        qDebug() << "Tags:\n";
        gst_tag_list_foreach (tags, print_tag_foreach, GINT_TO_POINTER (depth + 2));
    }
}


/* Print information regarding a stream and its substreams, if any */
static void print_topology (GstDiscovererStreamInfo *info, gint depth)
{
  GstDiscovererStreamInfo *next;

  if (!info)
    return;

  print_stream_info (info, depth);

  next = gst_discoverer_stream_info_get_next (info);
  if (next) {
    print_topology (next, depth + 1);
    gst_discoverer_stream_info_unref (next);
  } else if (GST_IS_DISCOVERER_CONTAINER_INFO (info)) {
    GList *tmp, *streams;

    streams = gst_discoverer_container_info_get_streams (GST_DISCOVERER_CONTAINER_INFO (info));
    for (tmp = streams; tmp; tmp = tmp->next) {
      GstDiscovererStreamInfo *tmpinf = (GstDiscovererStreamInfo *) tmp->data;
      print_topology (tmpinf, depth + 1);
    }
    gst_discoverer_stream_info_list_free (streams);
  }
}
/* This function is called every time the discoverer has information regarding
 * one of the URIs we provided.*/
static void on_discovered_cb (GstDiscoverer *discoverer, GstDiscovererInfo *info, GError *err, MainWindow * mainWindow)
{
  GstDiscovererResult result;
  const gchar *uri;
  const GstTagList *tags;
  GstDiscovererStreamInfo *sinfo;

  uri = gst_discoverer_info_get_uri (info);
  result = gst_discoverer_info_get_result (info);
  switch (result) {
    case GST_DISCOVERER_URI_INVALID:
      qDebug() << "Invalid URI '%s'\n" << uri;
      break;
    case GST_DISCOVERER_ERROR:
      qDebug() << "Discoverer error: %s\n" << err->message;
      break;
    case GST_DISCOVERER_TIMEOUT:
      qDebug() << "Timeout\n";
      break;
    case GST_DISCOVERER_BUSY:
      qDebug() << "Busy\n";
      break;
    case GST_DISCOVERER_MISSING_PLUGINS:{
      const GstStructure *s;
      gchar *str;

      s = gst_discoverer_info_get_misc (info);
      str = gst_structure_to_string (s);

      qDebug() << "Missing plugins: %s\n" << str;
      g_free (str);
      break;
    }
    case GST_DISCOVERER_OK:
      qDebug() << "Discovered '%s'\n" << uri;
      break;
  }

  if (result != GST_DISCOVERER_OK) {
    qDebug() << "This URI cannot be played\n";
    return;
  }

  /* If we got no error, show the retrieved information */

  //qDebug() << "\nDuration: " << GST_TIME_FORMAT << "\n" << GST_TIME_ARGS (gst_discoverer_info_get_duration (info));
  qDebug() << "\nDuration: " << gst_discoverer_info_get_duration (info);

  tags = gst_discoverer_info_get_tags (info);
  if (tags) {
    qDebug() << "Tags:\n";
    gst_tag_list_foreach (tags, print_tag_foreach, GINT_TO_POINTER (1));
  }

  qDebug() << "Seekable:" << (gst_discoverer_info_get_seekable (info) ? "yes" : "no");

  qDebug() << "\n";

  sinfo = gst_discoverer_info_get_stream_info (info);
  if (!sinfo)
    return;

  qDebug() << "Stream information:\n";

  print_topology (sinfo, 1);

  gst_discoverer_stream_info_unref (sinfo);

  qDebug() << "\n";
}

/* This function is called when the discoverer has finished examining
 * all the URIs we provided.*/
static void on_finished_cb (GstDiscoverer *discoverer, MainWindow * mainWindow)
{
    // todo : send message to Message dock and get rid or console
  qDebug() << "Finished discovering\n";

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

    glWidget = new GLWidget(glFormat, this);

    setCentralWidget(glWidget);

    return;
}

void MainWindow::createDiscoverer()
{
    GError * err = NULL;
    discoverer = gst_discoverer_new (5 * GST_SECOND, &err);
    if (!discoverer) {
      qDebug() << "Error creating discoverer instance: " << err->message;
      g_clear_error (&err);
      return;
    }

    //g_signal_connect (discoverer, "discovered", G_CALLBACK (on_discovered_cb), &discoverer);
    //g_signal_connect (discoverer, "finished", G_CALLBACK (on_finished_cb), &discoverer);
    g_signal_connect (discoverer, "discovered", G_CALLBACK (on_discovered_cb), this);
    g_signal_connect (discoverer, "finished", G_CALLBACK (on_finished_cb), this);

    loop = g_main_loop_new (NULL, FALSE);
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

 MainWindow::~MainWindow()
 {
     g_object_unref (discoverer);
     g_main_loop_unref (loop);

     /* Release libVLC instance on quit */
     //if (vlcInstance) {
     //    libvlc_release(vlcInstance);
     //}
 }
