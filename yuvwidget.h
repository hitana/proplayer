#ifndef YUVWIDGET_H
#define YUVWIDGET_H

#include <QGLWidget>
#include <QLabel>

class YuvWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit YuvWidget(const QGLFormat &format, QWidget *parent = 0);
    QLabel *imageLabel;

signals:

public slots:
};

#endif // YUVWIDGET_H
