#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>

#include <QTcpServer>
#include <QTcpSocket>

#include "imagewindow.h"
#include "IEngine.h"
#include "Cls_EngineNVR.h"
#include "Cls_EngineNQM.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBoxEngineSelect_currentTextChanged(const QString &engineName);
    void on_checkBoxTestMode_stateChanged(int state);
    void setControlEnables(bool enabled);
    void on_pushButtonProjectorOn_clicked();
    void on_pushButtonProjectorOff_clicked();
    void on_pushButtonLEDOn_clicked();
    void on_pushButtonLEDOff_clicked();
    void on_pushButtonSetCurrent_clicked();
    void on_pushButtonGetCurrent_clicked();
    void on_pushButtonImageMount_clicked();
    void on_pushButtonSelectImage_clicked();
    void on_pushButtonSetFlipX_clicked();
    void on_pushButtonSetFlipY_clicked();
    void on_pushButtonGetFlipX_clicked();
    void on_pushButtonGetFlipY_clicked();
    void on_pushButtonSetRotationAngle_clicked();
    void on_pushButtonGetRotationAngle_clicked();
    void on_pushButtonGetTemp_clicked();

    void handleNewConnection();
    void handleClientData();
    void handleClientDisconnect();
    void updateServerState();

private:
    void setupUi();

    Ui::MainWindow *ui;
    ImageWindow *m_imageWindow;
    IEngine *m_currentEngine;
    QString engineResolution;
    QString imagePath;  // 테스트용 임시 이미지 파일 경로 (소켓 통신시에서는 사용하지 않음)

    QTcpServer *m_server;
    QTcpSocket *m_clientSocket;
    qint64 m_expectedImageSize;     // 수신할 이미지 전체 크기
    QByteArray m_imageDataBuffer;
    bool m_expectingImageHeader;    // true일 경우, 다음 데이터는 이미지 헤더임

    bool socketOpen();
    bool socketClose();
};

#endif // MAINWINDOW_H
