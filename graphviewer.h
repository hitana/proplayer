#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QMainWindow>
#include <QWidget>
#include <QScrollBar>
#include <QScrollArea>
#include <QLabel>
#include <QImageReader>
#include <QWheelEvent>

#include <QDebug>
#include <QAction>

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
