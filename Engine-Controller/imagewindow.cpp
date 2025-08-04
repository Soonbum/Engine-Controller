#include "imagewindow.h"
#include <QPalette>

ImageWindow::ImageWindow(QWidget *parent) :
    QLabel(parent),
    m_isFlipX(false),
    m_isFlipY(false),
    m_rotationAngle(0)
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

void ImageWindow::setFlipX(bool flipped)
{
    m_isFlipX = flipped;
}

void ImageWindow::setFlipY(bool flipped)
{
    m_isFlipY = flipped;
}

bool ImageWindow::isFlipX() const
{
    return m_isFlipX;
}

bool ImageWindow::isFlipY() const
{
    return m_isFlipY;
}

void ImageWindow::setRotationAngle(int angle)
{
    // 0, 90, 180, 270도만 유효한 값으로 처리
    if (angle == 0 || angle == 90 || angle == 180 || angle == 270)
    {
        m_rotationAngle = angle;
    }
}

int ImageWindow::rotationAngle() const
{
    return m_rotationAngle;
}

void ImageWindow::updateImage(const QPixmap &pixmap)
{
    QImage image = pixmap.toImage();

    // 필요한 경우 회전 적용
    if (m_rotationAngle != 0)
    {
        QTransform transform;
        transform.rotate(m_rotationAngle);
        image = image.transformed(transform, Qt::SmoothTransformation);
    }

    // mirrored 함수로 이미지 뒤집기
    QImage flippedImage = image.mirrored(m_isFlipX, m_isFlipY);

    // 최종 이미지 변환
    this->setPixmap(QPixmap::fromImage(flippedImage));
}

void ImageWindow::clearImage()
{
    this->clear();
}
