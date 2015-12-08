#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include <QGLWidget>

#define AUDIO_WIDGET_MIN_W  250
#define AUDIO_WIDGET_MIN_H  80

class AudioWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit AudioWidget(QWidget *parent = 0);

signals:

public slots:
};

#endif // AUDIOWIDGET_H

