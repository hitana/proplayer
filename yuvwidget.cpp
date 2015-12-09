#include "yuvwidget.h"
#include <QLayout>

YuvWidget::YuvWidget(QWidget *parent)
    : QWidget(parent)
{
    imageLabel = new QLabel(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // todo
    //imageLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    //imageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    imageLabel->setScaledContents(true);
    imageLabel->setText("YUV output");
    imageLabel->setStyleSheet("QLabel { background-color : transparent; color : grey; font: italic;}");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addWidget(imageLabel);
    setLayout(layout);
}

void YuvWidget::setPixmap (const QPixmap &p)
{
    pixmap = p;
    imageLabel->setPixmap(pixmap);
}

int YuvWidget::heightForWidth (int width) const
{
    return ((qreal)pixmap.height() * width) / pixmap.width();
}

void YuvWidget::resizeEvent(QResizeEvent * e)
{
    Q_UNUSED(e);
    imageLabel->setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QSize YuvWidget::minimumSizeHint() const
{
    return QSize(YUV_MIN_SIZE, YUV_MIN_SIZE);
}

QSize YuvWidget::sizeHint() const
{
    int w = this->width();
    return QSize (w, heightForWidth(w));
}
