#ifndef YUVWIDGET_H
#define YUVWIDGET_H

#include <QGLWidget>
#include <QLabel>

#define YUV_MIN_SIZE 50

class YuvWidget : public QWidget
{
    Q_OBJECT

public:
    explicit YuvWidget(QWidget *parent = 0);
    QLabel *imageLabel;

    int heightForWidth( int width ) const;
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:

public slots:
    void setPixmap ( const QPixmap & );
    void resizeEvent(QResizeEvent *);

private:
    QPixmap pixmap;

};

#endif // YUVWIDGET_H
