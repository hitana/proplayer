#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QMainWindow>
#include <QWidget>
#include <QScrollBar>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QImageReader>
#include <QWheelEvent>

#include <QDebug>
#include <QAction>

#define ZOOM_BUTTON_SIZE 40
#define ZOOM_BUTTON_FONT 18

class GraphViewer : public QWidget
{
    Q_OBJECT
public:
    explicit GraphViewer(QWidget *parent = 0);

    bool loadFile(const QString &);

private slots:
    void zoomInAction();
    void zoomOutAction();
    void normalSize();
    void fitToWindow();

private:
    void createActions();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;
    QPushButton * zoomInButton;
    QPushButton * zoomOutButton;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    //QAction *fitToWindowAct;

signals:

public slots:
protected:
    void wheelEvent (QWheelEvent * event);
};

#endif // GRAPHVIEWER_H
