//
// Created by zoe50 on 2023/12/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_rts_observer.h" resolved

#include "rts_observer.h"
#include "ui_rts_observer.h"
#include <iostream>
#include <chrono>
#include <QtWidgets/QMessageBox>
#include <QValidator>
#include "rpc_client.h"

using namespace std;
using namespace chrono;
using namespace this_thread;

ThreadPool RtsObserver::threadPool {8};
int RtsObserver::seed {1};
double RtsObserver::terrainProb {0};
int RtsObserver::expansionCnt {1};
int RtsObserver::clusterCnt {1};
int RtsObserver::resourceCnt {1};

RtsObserver::RtsObserver(QWidget *parent) :
        QWidget(parent), ui(new Ui::RtsObserver) {
    ui->setupUi(this);
    ui->seedLineEdit->setValidator(new QIntValidator(0, 1<<30, this));

    connect(ui->connectButton, &QPushButton::released, this, &RtsObserver::HandleConnectButton);
    connect(&renderTimer, &QTimer::timeout, this, &RtsObserver::HandleRenderTimer);
    connect(ui->startButton, &QPushButton::released, this, &RtsObserver::HandleStartButton);
    connect(ui->stopButton, &QPushButton::released, this, &RtsObserver::HandleStopButton);
    connect(ui->stepButton, &QPushButton::released, this, &RtsObserver::HandleStepButton);
    connect(ui->resetButton, &QPushButton::released, this, &RtsObserver::HandleResetButton);
    connect(ui->seedLineEdit, &QLineEdit::textChanged, this, &RtsObserver::HandleSeedLineEdit);
    connect(ui->terrainSlider, &QSlider::valueChanged, this, &RtsObserver::HandleTerrainSlide);
    connect(ui->expansionSlider, &QSlider::valueChanged, this, &RtsObserver::HandleExpansionSlide);
    connect(ui->clusterSlider, &QSlider::valueChanged, this, &RtsObserver::HandleClusterSlide);
    connect(ui->resourceSlider, &QSlider::valueChanged, this, &RtsObserver::HandleResourceSlide);
    renderTimer.setInterval(10);
    renderTimer.start();
}

RtsObserver::~RtsObserver() {
    RpcClient::SendCommand(static_cast<message::Command>(DISCONNECT));
    delete ui;
}

void RtsObserver::HandleConnectButton() const {
    auto ip = ui->ipLineEdit->text().toStdString();
    auto port = ui->portLineEdit->text().toStdString();
    if (!RpcClient::stop) {
        RpcClient::SendCommand(static_cast<message::Command>(DISCONNECT));
        RpcClient::stop = true;
        return;
    }
    threadPool.enqueue(RpcClient::Connect, ip + ":" + port);
}

void RtsObserver::HandleRenderTimer() {
    ui->timeDisplayLabel->setText(QString::fromStdString(to_string(RpcClient::GetObservation())));
    this->RefreshButton();

}

void RtsObserver::HandleStartButton() {
    RpcClient::SendCommand(message::START);
}

void RtsObserver::HandleStopButton() {
    RpcClient::SendCommand(message::STOP);
}

void RtsObserver::HandleStepButton() {
    RpcClient::SendCommand(message::STEP);
}

void RtsObserver::HandleResetButton() {
    RpcClient::SendCommand(message::RESET);
}

void RtsObserver::RefreshButton() {
    if (RpcClient::stop) {
        ui->connectButton->setText(QString::fromStdString("Connect"));
        ui->startButton->setEnabled(false);
        ui->stepButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
    } else {
        ui->connectButton->setText(QString::fromStdString("Disconnect"));
        ui->startButton->setEnabled(true);
        ui->stepButton->setEnabled(true);
        ui->resetButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
    }
}

void RtsObserver::HandleTerrainSlide() const {
    auto numStr = to_string(ui->terrainSlider->value());
    ui->terrainLabel->setText(QString::fromStdString("Terrain Prob: " + numStr + "%"));
    terrainProb = ui->terrainSlider->value();
}

void RtsObserver::HandleExpansionSlide() const {
    auto numStr = to_string(ui->expansionSlider->value());
    ui->expansionLabel->setText(QString::fromStdString("Expansion Count: " + numStr));
    expansionCnt = ui->expansionSlider->value();
}

void RtsObserver::HandleClusterSlide() const {
    auto numStr = to_string(ui->clusterSlider->value());
    ui->clusterLabel->setText(QString::fromStdString("Cluster/Expansion: " + numStr));
    clusterCnt = ui->clusterSlider->value();
}

void RtsObserver::HandleResourceSlide() const {
    auto numStr = to_string(ui->resourceSlider->value());
    ui->resourceLabel->setText(QString::fromStdString("Resource/Cluster: " + numStr));
    resourceCnt = ui->resourceSlider->value();
}

void RtsObserver::HandleSeedLineEdit() const {
    seed = stoi(ui->seedLineEdit->text().toStdString());
}
