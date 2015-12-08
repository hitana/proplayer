#include "audiowidget.h"

AudioWidget::AudioWidget(QWidget *parent) : QGLWidget(parent)
{
    setAutoFillBackground (true);
    setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Minimum);
    setMinimumHeight (AUDIO_WIDGET_MIN_H);
    setMinimumWidth (AUDIO_WIDGET_MIN_W);
}

