#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <stdio.h>
#include <unistd.h>

// UI 초기화
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_currentEngine(nullptr),
    m_server(nullptr),
    m_clientSocket(nullptr),
    m_expectedImageSize(0)
{
    ui->setupUi(this);

    // 이미지 표시 창을 먼저 생성하고 띄움
    m_imageWindow = new ImageWindow();
    m_imageWindow->show();

    // 콤보박스에 엔진목록 추가
    ui->comboBoxEngineSelect->addItem("NVR");
    ui->comboBoxEngineSelect->addItem("NQM+");

    // 초기 상태 설정
    //ui->comboBoxEngineSelect->setCurrentIndex(1); // 기본값: NQM+
    ui->checkBoxTestMode->setChecked(false);        // TestMode = false
    on_comboBoxEngineSelect_currentTextChanged(ui->comboBoxEngineSelect->currentText());
    on_checkBoxTestMode_stateChanged(ui->checkBoxTestMode->checkState());

    // 테스트 모드 체크박스, 엔진 콤보박스가 변경될 때 서버 상태 업데이트
    connect(ui->checkBoxTestMode, &QCheckBox::stateChanged, this, &MainWindow::updateServerState);
    connect(ui->comboBoxEngineSelect, &QComboBox::currentTextChanged, this, &MainWindow::updateServerState);

    // 초기 서버 상태 설정
    updateServerState();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_imageWindow;
    if (m_currentEngine)
        delete m_currentEngine;
}

void MainWindow::on_comboBoxEngineSelect_currentTextChanged(const QString &engineName)
{
    if (m_currentEngine) {
        delete m_currentEngine;
        m_currentEngine = nullptr;
    }

    if (engineName == "NVR") {
        m_currentEngine = new Cls_EngineNVR();
        engineResolution = "1920x1080";
    } else if (engineName == "NQM+") {
        m_currentEngine = new Cls_EngineNQM();
        engineResolution = "3840x2160";
    }
    qDebug() << engineName << "엔진이 선택되었습니다.";
}

void MainWindow::on_checkBoxTestMode_stateChanged(int state)
{
    bool isTestMode = (state == Qt::Checked);
    setControlEnables(isTestMode);
}

void MainWindow::setControlEnables(bool enabled)
{
    ui->comboBoxEngineSelect->setEnabled(enabled);
    ui->pushButtonProjectorOn->setEnabled(enabled);
    ui->pushButtonProjectorOff->setEnabled(enabled);
    ui->pushButtonLEDOn->setEnabled(enabled);
    ui->pushButtonLEDOff->setEnabled(enabled);
    ui->pushButtonSetCurrent->setEnabled(enabled);
    ui->pushButtonGetCurrent->setEnabled(enabled);
    ui->pushButtonImageMount->setEnabled(enabled);
    ui->pushButtonSelectImage->setEnabled(enabled);
    ui->pushButtonSetFlipX->setEnabled(enabled);
    ui->pushButtonSetFlipY->setEnabled(enabled);
}

void MainWindow::on_pushButtonProjectorOn_clicked()
{
    if (m_currentEngine) m_currentEngine->projectorOnOff(true);
}

void MainWindow::on_pushButtonProjectorOff_clicked()
{
    if (m_currentEngine) m_currentEngine->projectorOnOff(false);
}

void MainWindow::on_pushButtonLEDOn_clicked()
{
    if (m_currentEngine) m_currentEngine->ledOnOff(true);
}

void MainWindow::on_pushButtonLEDOff_clicked()
{
    if (m_currentEngine) m_currentEngine->ledOnOff(false);
}

void MainWindow::on_pushButtonSetCurrent_clicked()
{
    if (m_currentEngine) {
        bool ok;
        int value = ui->lineEditSetCurrent->text().toInt(&ok);
        if (ok) {
            m_currentEngine->setLEDCurrent(value);
        } else {
            qDebug() << "0~1023 숫자를 입력하세요.";
        }
    }
}

void MainWindow::on_pushButtonGetCurrent_clicked()
{
    if (m_currentEngine) {
        int value = m_currentEngine->getLedCurrent();
        ui->lineEditGetCurrent->setText(QString::number(value));
    }
}

void MainWindow::on_pushButtonImageMount_clicked()
{
    if (!this->imagePath.isEmpty()) {
        QPixmap pixmap(this->imagePath);
        if (!pixmap.isNull()) {
            m_imageWindow->updateImage(pixmap);
        }
    }
}

void MainWindow::on_pushButtonSelectImage_clicked()
{
    this->imagePath = QFileDialog::getOpenFileName(this, "이미지 선택", "", "Image Files (*.png *.jpg *.bmp)");
}

void MainWindow::on_pushButtonSetFlipX_clicked()
{
    // ... if (m_currentEngine) m_currentEngine->imageFlip()
}

void MainWindow::on_pushButtonSetFlipY_clicked()
{
    // ... if (m_currentEngine) m_currentEngine->imageFlip()
}

// 테스트 모드 상태에 따라 소켓 서버를 열거나 닫는 함수
void MainWindow::updateServerState()
{
    socketOpen();

//    if (ui->checkBoxTestMode->isChecked()) {
//        socketClose();
//    } else {
//        socketOpen();
//    }
}

// 소켓 열기 함수
bool MainWindow::socketOpen()
{
    // 이미 열려 있으면 아무것도 하지 않음
    if (m_server && m_server->isListening()) {
        return true;
    }

    // 기존 서버 객체가 있으면 삭제
    if (m_server) m_server->deleteLater();

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);

    quint16 port = 52852;
    if (m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "소켓 서버가 " << port << " 포트에서 연결을 기다립니다.";
        return true;
    } else {
        qDebug() << "소켓 서버 시작 실패" << m_server->errorString();
        return false;
    }
}

// 소켓 닫기 함수
bool MainWindow::socketClose()
{
    if (m_clientSocket) {
        m_clientSocket->disconnectFromHost();
        m_clientSocket = nullptr;
    }
    if (m_server) {
        m_server->close();
        m_server->deleteLater();
        m_server = nullptr;
        qDebug() << "소켓 서버를 닫았습니다.";
    }
    return true;
}

// 새로운 클라이언트 접속을 처리하는 슬롯
void MainWindow::handleNewConnection()
{
    if (m_clientSocket) {
        // 이미 연결된 클라이언트가 있으면 새로 들어온 연결은 거부하고 바로 닫음 (1개의 연결만 허용)
        QTcpSocket *extraSocket = m_server->nextPendingConnection();
        qDebug() << "이미 연결된 클라이언트가 있어 새로운 연결을 거부합니다: " << extraSocket->peerAddress();
        extraSocket->disconnectFromHost();
        extraSocket->deleteLater();
        return;
    }

    m_clientSocket = m_server->nextPendingConnection();
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &MainWindow::handleClientData);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &MainWindow::handleClientDisconnect);

    m_expectedImageSize = 0;
    m_imageDataBuffer.clear();

    qDebug() << "클라이언트 연결됨: " << m_clientSocket->peerAddress();

    QString responseString = "";
    if (ui->checkBoxTestMode->isChecked()) {
        responseString = "TestMode";
    } else {
        QString engineModel = ui->comboBoxEngineSelect->currentText();
        QString resolution = this->engineResolution;
        responseString = QString("%1,%2").arg(engineModel, resolution);
    }

    m_clientSocket->write(responseString.toUtf8() + "\n");

    // waitForBytesWritten() 호출하여 데이터가 실제로 전송될 때까지 기다림
    if (!m_clientSocket->waitForBytesWritten(1000)) {
        qDebug() << "데이터 전송 실패 또는 타임아웃: " << m_clientSocket->errorString();
    } else {
        qDebug() << "클라이언트에 정보 전송 완료: " << responseString;
    }
}

// 클라이언트로부터 데이터가 도착했을 때 호출되는 슬롯
void MainWindow::handleClientData()
{
    if (!m_clientSocket) return;

    // 헤더(이미지 크기)를 아직 받지 않은 경우
    if (m_expectedImageSize == 0) {
        if (m_clientSocket->bytesAvailable() < 8) return;   // 8바이트 모일 때까지 대기

        QByteArray header = m_clientSocket->read(8);
        QDataStream stream(&header, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);     //C# BitConverter는 LittleEndian
        stream >> m_expectedImageSize;
        qDebug() << "수신할 이미지 크기: " << m_expectedImageSize;
    }

    // 이미지 데이터 본문을 버퍼에 추가
    m_imageDataBuffer.append(m_clientSocket->readAll());

    // 모든 데이터가 도착했는지 확인
    if (m_imageDataBuffer.size() >= m_expectedImageSize) {
        qDebug() << "이미지 수신 완료: " << m_imageDataBuffer.size() << " bytes";

        QPixmap pixmap;
        // QByteArray(메모리)로부터 QPixmap 이미지 로드
        if (pixmap.loadFromData(m_imageDataBuffer)) {
            // ImageWindow에 pixmap 표시
            m_imageWindow->updateImage(pixmap);
            qDebug() << "원격 이미지를 화면에 마운트했습니다.";
        } else {
            qDebug() << "수신된 데이터로부터 이미지를 로드할 수 없습니다.";
        }

        // 다음 이미지를 받을 수 있도록 버퍼와 크기 변수 초기화
        m_expectedImageSize = 0;
        m_imageDataBuffer.clear();
    }
}

// 클라이언트 연결이 끊겼을 때 처리하는 슬롯
void MainWindow::handleClientDisconnect()
{
    qDebug() << "클라이언트 연결 끊김";
    m_clientSocket->deleteLater();
    m_clientSocket = nullptr;
}
