#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <QLabel>
#include <QPixmap>

class ImageWindow : public QLabel
{
    Q_OBJECT

public:
    explicit ImageWindow(QWidget *parent = nullptr);
    ~ImageWindow();

    void setFlipX(bool flipped);
    void setFlipY(bool flipped);

    bool isFlipX() const;
    bool isFlipY() const;

    void setRotationAngle(int angle);
    int rotationAngle() const;

public slots:
    void updateImage(const QPixmap &pixmap);
    void clearImage();

private:
    bool m_isFlipX;
    bool m_isFlipY;

    int m_rotationAngle;
};

#endif // IMAGEWINDOW_H
