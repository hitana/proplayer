#include "yuvwidget.h"
#include <QLayout>

YuvWidget::YuvWidget(const QGLFormat &format, QWidget *parent)
    : QGLWidget(format, parent)
{
    imageLabel = new QLabel(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // todo
    imageLabel->setScaledContents(true);
    imageLabel->setText("YUV output");
    imageLabel->setStyleSheet("QLabel { background-color : transparent; color : grey; font: italic;}");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addWidget(imageLabel);
    setLayout(layout);
}
