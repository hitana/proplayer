#include "graphviewer.h"

#include <QLayout>

GraphViewer::GraphViewer(QWidget *parent) : QWidget(parent)
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel->setScaledContents(true);

    QFont fontButton = font();
    fontButton.setWeight(QFont::ExtraLight);
    fontButton.setPointSize(ZOOM_BUTTON_FONT);

    zoomInButton = new QPushButton;
    zoomOutButton = new QPushButton;

    zoomInButton->setText("+");
    zoomInButton->setFont(fontButton);
    zoomInButton->setFixedHeight(ZOOM_BUTTON_SIZE);
    zoomInButton->setFixedWidth(ZOOM_BUTTON_SIZE);

    zoomOutButton->setText("−");
    zoomOutButton->setFont(fontButton);
    zoomOutButton->setFixedHeight(ZOOM_BUTTON_SIZE);
    zoomOutButton->setFixedWidth(ZOOM_BUTTON_SIZE);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(imageLabel);

    QVBoxLayout *layoutButtons = new QVBoxLayout();
    layoutButtons->addWidget(zoomInButton);
    layoutButtons->addWidget(zoomOutButton);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(scrollArea);
    layout->addLayout(layoutButtons);
    setLayout(layout);

    //createActions();
    //createMenus();

    //resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool GraphViewer::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull()) {
        qDebug () << "loadFile: ERROR! Cannot load file " << fileName;

        //QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
//                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        imageLabel->adjustSize();
        return false;
    }
    qDebug () << "loadFile: setPixmap...";
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    //fitToWindowAct->setEnabled(true);
    //updateActions();

    qDebug () << "loadFile: adjustSize...";
    //if (!fitToWindowAct->isChecked())

        imageLabel->adjustSize();

    //setWindowFilePath(fileName);
    return true;
}

void GraphViewer::wheelEvent (QWheelEvent *event)
{
    // if Ctrl pressed
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        if (event->delta() > 0) {
            zoomInAction();
        }
        else if(event->delta()<0){
            zoomOutAction();
        }
        event->accept();
        return;
    }
/*
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
    {
        scrollWithPixels(numPixels);
    }
    else if (!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        scrollWithDegrees(numSteps);
    }

    event->accept();*/
}

void GraphViewer::zoomInAction()
{
    scaleImage(1.25);
}

void GraphViewer::zoomOutAction()
{
    scaleImage(0.8);
}

void GraphViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void GraphViewer::fitToWindow()
{
    // todo
//    bool fitToWindow = fitToWindowAct->isChecked();   // todo
    //scrollArea->setWidgetResizable(fitToWindow);
    scrollArea->setWidgetResizable(true);
    /*if (!fitToWindow) {
        normalSize();
    }
    updateActions();*/
}

/*
void GraphViewer::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void GraphViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

void GraphViewer::updateActions()
{
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}
*/
void GraphViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    // todo
    //zoomInAct->setEnabled(scaleFactor < 3.0);
    //zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void GraphViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
