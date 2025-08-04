#include "imagewindow.h"
#include <QPalette>

ImageWindow::ImageWindow(QWidget *parent) : QLabel(parent)
{
    // 창 경계선 제거 및 전체 화면 설정
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    showFullScreen();

    // 배경을 검은색으로 설정
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(palette);

    //이미지를  좌상단으로 정렬
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

ImageWindow::~ImageWindow()
{
}

void ImageWindow::updateImage(const QPixmap &pixmap)
{
    this->setPixmap(pixmap);
}

void ImageWindow::clearImage()
{
    this->clear();
}
