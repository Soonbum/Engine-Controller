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
    m_currentEngine(nullptr)
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
    } else if (engineName == "NQM+") {
        m_currentEngine = new Cls_EngineNQM();
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

