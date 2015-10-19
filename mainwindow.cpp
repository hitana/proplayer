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
 {
     textEdit = new QTextEdit;
     setCentralWidget(textEdit);

     createActions();
     createMenus();
     createToolBars();
     createStatusBar();
     createDockWindows();
     createVlc();

     setWindowTitle(tr("Dock Widgets"));

     newLetter();
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

 void MainWindow::newLetter()
 {
     textEdit->clear();

     QTextCursor cursor(textEdit->textCursor());
     cursor.movePosition(QTextCursor::Start);
     QTextFrame *topFrame = cursor.currentFrame();
     QTextFrameFormat topFrameFormat = topFrame->frameFormat();
     topFrameFormat.setPadding(16);
     topFrame->setFrameFormat(topFrameFormat);

     QTextCharFormat textFormat;
     QTextCharFormat boldFormat;
     boldFormat.setFontWeight(QFont::Bold);
     QTextCharFormat italicFormat;
     italicFormat.setFontItalic(true);

     QTextTableFormat tableFormat;
     tableFormat.setBorder(1);
     tableFormat.setCellPadding(16);
     tableFormat.setAlignment(Qt::AlignRight);
     cursor.insertTable(1, 1, tableFormat);
     cursor.insertText("The Firm", boldFormat);
     cursor.insertBlock();
     cursor.insertText("321 City Street", textFormat);
     cursor.insertBlock();
     cursor.insertText("Industry Park");
     cursor.insertBlock();
     cursor.insertText("Some Country");
     cursor.setPosition(topFrame->lastPosition());
     cursor.insertText(QDate::currentDate().toString("d MMMM yyyy"), textFormat);
     cursor.insertBlock();
     cursor.insertBlock();
     cursor.insertText("Dear ", textFormat);
     cursor.insertText("NAME", italicFormat);
     cursor.insertText(",", textFormat);
     for (int i = 0; i < 3; ++i)
         cursor.insertBlock();
     cursor.insertText(tr("Yours sincerely,"), textFormat);
     for (int i = 0; i < 3; ++i)
         cursor.insertBlock();
     cursor.insertText("The Boss", textFormat);
     cursor.insertBlock();
     cursor.insertText("ADDRESS", italicFormat);
 }

 void MainWindow::print()
 {
 #ifndef QT_NO_PRINTDIALOG
     QTextDocument *document = textEdit->document();
     QPrinter printer;

     QPrintDialog *dlg = new QPrintDialog(&printer, this);
     if (dlg->exec() != QDialog::Accepted)
         return;

     document->print(&printer);

     statusBar()->showMessage(tr("Ready"), 2000);
 #endif
 }

 void MainWindow::save()
 {
     QString fileName = QFileDialog::getSaveFileName(this,
                         tr("Choose a file name"), ".",
                         tr("HTML (*.html *.htm)"));
     if (fileName.isEmpty())
         return;
     QFile file(fileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("Dock Widgets"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return;
     }

     QTextStream out(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     out << textEdit->toHtml();
     QApplication::restoreOverrideCursor();

     statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
 }

 void MainWindow::undo()
 {
     QTextDocument *document = textEdit->document();
     document->undo();
 }

 void MainWindow::insertCustomer(const QString &customer)
 {
     if (customer.isEmpty())
         return;
     QStringList customerList = customer.split(", ");
     QTextDocument *document = textEdit->document();
     QTextCursor cursor = document->find("NAME");
     if (!cursor.isNull()) {
         cursor.beginEditBlock();
         cursor.insertText(customerList.at(0));
         QTextCursor oldcursor = cursor;
         cursor = document->find("ADDRESS");
         if (!cursor.isNull()) {
             for (int i = 1; i < customerList.size(); ++i) {
                 cursor.insertBlock();
                 cursor.insertText(customerList.at(i));
             }
             cursor.endEditBlock();
         }
         else
             oldcursor.endEditBlock();
     }
 }

 void MainWindow::addParagraph(const QString &paragraph)
 {
     if (paragraph.isEmpty())
         return;
     QTextDocument *document = textEdit->document();
     QTextCursor cursor = document->find(tr("Yours sincerely,"));
     if (cursor.isNull())
         return;
     cursor.beginEditBlock();
     cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, 2);
     cursor.insertBlock();
     cursor.insertText(paragraph);
     cursor.insertBlock();
     cursor.endEditBlock();

 }

 void MainWindow::onSelectPlaylist(const QString &playlistItem)
 {
     qDebug() << "Selected " << playlistItem;
     return;
 }

void MainWindow::onDoubleClick(const QModelIndex &modelIndex)
{
    QString pipelineString("filesrc location=" + playList->currentItem()->text() + " ! decodebin ! glimagesink name=vsink sync=false");
    char pipelineChars[PATH_MAX];
    sprintf (pipelineChars, "%s", pipelineString.toLocal8Bit().data());
    //sprintf (pipelineChars, "%s", pipelineString.toLatin1().data());
    //sprintf (pipelineChars, "%s", pipelineString.toStdString().c_str());

    qDebug() << "Playing " << playList->currentRow() << ", " << playList->currentItem()->text();
    qDebug() << "pipeline = " << pipelineString;

    // todo : seems glimagesink plays i-frames only
    GstElement * pipeline = gst_parse_launch(pipelineChars, NULL);
    GstElement * vsink = gst_bin_get_by_name (GST_BIN (pipeline), "vsink");
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (vsink), (guintptr)glWidget->getWindowId());
    gst_object_unref (vsink);
    GstStateChangeReturn sret = gst_element_set_state (pipeline, GST_STATE_PLAYING);


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
     newLetterAct = new QAction(QIcon(":/images/new.png"), tr("&New Letter"),
                                this);
     newLetterAct->setShortcuts(QKeySequence::New);
     newLetterAct->setStatusTip(tr("Create a new form letter"));
     connect(newLetterAct, SIGNAL(triggered()), this, SLOT(newLetter()));

     saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
     saveAct->setShortcuts(QKeySequence::Save);
     saveAct->setStatusTip(tr("Save the current form letter"));
     connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

     printAct = new QAction(QIcon(":/images/print.png"), tr("&Print..."), this);
     printAct->setShortcuts(QKeySequence::Print);
     printAct->setStatusTip(tr("Print the current form letter"));
     connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

     undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
     undoAct->setShortcuts(QKeySequence::Undo);
     undoAct->setStatusTip(tr("Undo the last editing action"));
     connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

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
     fileMenu->addAction(newLetterAct);
     fileMenu->addAction(saveAct);
     fileMenu->addAction(printAct);
     fileMenu->addSeparator();
     fileMenu->addAction(quitAct);

     editMenu = menuBar()->addMenu(tr("&Edit"));
     editMenu->addAction(undoAct);

     viewMenu = menuBar()->addMenu(tr("&View"));

     menuBar()->addSeparator();

     helpMenu = menuBar()->addMenu(tr("&Help"));
     helpMenu->addAction(aboutAct);
     helpMenu->addAction(aboutQtAct);
 }

 void MainWindow::createToolBars()
 {
     fileToolBar = addToolBar(tr("File"));
     fileToolBar->addAction(newLetterAct);
     fileToolBar->addAction(saveAct);
     fileToolBar->addAction(printAct);

     editToolBar = addToolBar(tr("Edit"));
     editToolBar->addAction(undoAct);
 }

 void MainWindow::createStatusBar()
 {
     statusBar()->showMessage(tr("Ready"));
 }

 void MainWindow::createDockWindows()
 {
     QDockWidget *dock = new QDockWidget(tr("Customers"), this);
     dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
     customerList = new QListWidget(dock);
     customerList->addItems(QStringList()
             << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
             << "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
             << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
             << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
             << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
             << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
     dock->setWidget(customerList);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());

     dock = new QDockWidget(tr("Paragraphs"), this);
     paragraphsList = new QListWidget(dock);
     paragraphsList->addItems(QStringList()
             << "Thank you for your payment which we have received today."
             << "Your order has been dispatched and should be with you "
                "within 28 days."
             << "We have dispatched those items that were in stock. The "
                "rest of your order will be dispatched once all the "
                "remaining items have arrived at our warehouse. No "
                "additional shipping charges will be made."
             << "You made a small overpayment (less than $5) which we "
                "will keep on account for you, or return at your request."
             << "You made a small underpayment (less than $1), but we have "
                "sent your order anyway. We'll add this underpayment to "
                "your next bill."
             << "Unfortunately you did not send enough money. Please remit "
                "an additional $. Your order will be dispatched as soon as "
                "the complete amount has been received."
             << "You made an overpayment (more than $5). Do you wish to "
                "buy more items, or should we return the excess to you?");
     dock->setWidget(paragraphsList);
     addDockWidget(Qt::LeftDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());

     dock = new QDockWidget (tr("Playlist"), this);
     playList = new QListWidget(dock);

     dock->setWidget(playList);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());

     dock = new QDockWidget (tr("Video"), this);
     label = new QLabel();
     dock->setWidget(label);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());

     //--------------------------------------------------
     QGLFormat glFormat;
     glFormat.setVersion (2, 1);
     glFormat.setProfile (QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
     glFormat.setSampleBuffers (true);

     glWidget = new GLWidget(glFormat, this);
     dock = new QDockWidget (tr ("GlWidget"), this);
     dock->setWidget(glWidget);
     addDockWidget(Qt::RightDockWidgetArea, dock);
     viewMenu->addAction(dock->toggleViewAction());

     //---------------------------------------------------
     gst_init (NULL, NULL);
/*
#ifdef Q_OS_UNIX
    GstElement * pipeline = gst_parse_launch("filesrc location=/home/vq/atomic.ts ! decodebin ! glimagesink name=vsink sync=false", NULL);
#endif
#ifdef Q_OS_MAC
    GstElement * pipeline = gst_parse_launch("filesrc location=/Users/qa/Desktop/media/atomic.ts ! decodebin ! glimagesink name=vsink sync=false", NULL);
#endif

     GstElement * vsink = gst_bin_get_by_name (GST_BIN (pipeline), "vsink");
     gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (vsink), (guintptr)glWidget->getWindowId());
     gst_object_unref (vsink);
     GstStateChangeReturn sret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
*/
     connect(customerList, SIGNAL(currentTextChanged(QString)),
             this, SLOT(insertCustomer(QString)));
     connect(paragraphsList, SIGNAL(currentTextChanged(QString)),
             this, SLOT(addParagraph(QString)));

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

 MainWindow::~MainWindow() {
     /* Release libVLC instance on quit */
     //if (vlcInstance) {
     //    libvlc_release(vlcInstance);
     //}
 }
