#ifndef YUVWIDGET_H
#define YUVWIDGET_H

#include <QGLWidget>
#include <QLabel>

class YuvWidget : public QWidget
{
    Q_OBJECT
public:
    explicit YuvWidget(QWidget *parent = 0);
    QLabel *imageLabel;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:

public slots:
};

#endif // YUVWIDGET_H
