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

public slots:
    void updateImage(const QPixmap &pixmap);
    void clearImage();
};

#endif // IMAGEWINDOW_H
