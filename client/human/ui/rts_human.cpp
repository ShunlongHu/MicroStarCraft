//
// Created by zoe50 on 2023/12/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_rts_observer.h" resolved

#include "rts_human.h"
#include "ui_rts_human.h"
#include <iostream>
#include <chrono>
#include <QtWidgets/QMessageBox>
#include <QValidator>
#include "rpc_client.h"

using namespace std;
using namespace chrono;
using namespace this_thread;

ThreadPool RtsHuman::threadPool {8};
int RtsHuman::seed {0};
double RtsHuman::terrainProb {0};
int RtsHuman::expansionCnt {1};
int RtsHuman::clusterCnt {1};
int RtsHuman::resourceCnt {1};
bool RtsHuman::isAxSym {true};

RtsHuman::RtsHuman(QWidget *parent) :
        QWidget(parent), ui(new Ui::RtsHuman) {
    ui->setupUi(this);
    ui->seedLineEdit->setValidator(new QIntValidator(0, 1<<30, this));

    connect(ui->connectButton, &QPushButton::released, this, &RtsHuman::HandleConnectButton);
    connect(&renderTimer, &QTimer::timeout, this, &RtsHuman::HandleRenderTimer);
    connect(ui->startButton, &QPushButton::released, this, &RtsHuman::HandleStartButton);
    connect(ui->stopButton, &QPushButton::released, this, &RtsHuman::HandleStopButton);
    connect(ui->stepButton, &QPushButton::released, this, &RtsHuman::HandleStepButton);
    connect(ui->resetButton, &QPushButton::released, this, &RtsHuman::HandleResetButton);
    connect(ui->seedLineEdit, &QLineEdit::textChanged, this, &RtsHuman::HandleSeedLineEdit);
    connect(ui->terrainSlider, &QSlider::valueChanged, this, &RtsHuman::HandleTerrainSlide);
    connect(ui->expansionSlider, &QSlider::valueChanged, this, &RtsHuman::HandleExpansionSlide);
    connect(ui->clusterSlider, &QSlider::valueChanged, this, &RtsHuman::HandleClusterSlide);
    connect(ui->resourceSlider, &QSlider::valueChanged, this, &RtsHuman::HandleResourceSlide);
    connect(ui->axialRadio, &QRadioButton::toggled, this, &RtsHuman::HandleIsAxialRadio);
    renderTimer.setInterval(10);
    renderTimer.start();
}

RtsHuman::~RtsHuman() {
    RpcClient::SendCommand(static_cast<message::Command>(DISCONNECT));
    delete ui;
}

void RtsHuman::HandleConnectButton() {
    auto ip = ui->ipLineEdit->text().toStdString();
    auto port = ui->portLineEdit->text().toStdString();
    if (!RpcClient::stop) {
        RpcClient::SendCommand(static_cast<message::Command>(DISCONNECT));
        RpcClient::stop = true;
        return;
    }
    threadPool.enqueue(RpcClient::Connect, ip + ":" + port);
    sleep_for(milliseconds(100));
    RpcClient::SendCommand(message::START);
}

void RtsHuman::HandleRenderTimer() {
    this->RefreshButton();
    if (RpcClient::newState.exchange(false)) {
        ui->timeDisplayLabel->setText(QString::fromStdString(to_string(RpcClient::GetObservation().size())));
//        this->ui->mapWidget->repaint();
    }
}

void RtsHuman::HandleStartButton() {
    RpcClient::SendCommand(message::START);
}

void RtsHuman::HandleStopButton() {
    RpcClient::SendCommand(message::STOP);
}

void RtsHuman::HandleStepButton() {
    RpcClient::SendCommand(message::STEP);
}

void RtsHuman::HandleResetButton() {
    terrainProb = ui->terrainSlider->value();
    expansionCnt = ui->expansionSlider->value();
    clusterCnt = ui->clusterSlider->value();
    resourceCnt = ui->resourceSlider->value();
    seed = stoi(ui->seedLineEdit->text().toStdString());
    isAxSym = ui->axialRadio->isChecked();
    RpcClient::SendCommand(message::RESET);
}

void RtsHuman::RefreshButton() {
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

void RtsHuman::HandleTerrainSlide() const {
    auto numStr = to_string(ui->terrainSlider->value());
    ui->terrainLabel->setText(QString::fromStdString("Terrain Prob: " + numStr + "%"));
    terrainProb = ui->terrainSlider->value();
}

void RtsHuman::HandleExpansionSlide() const {
    auto numStr = to_string(ui->expansionSlider->value());
    ui->expansionLabel->setText(QString::fromStdString("Expansion Count: " + numStr));
    expansionCnt = ui->expansionSlider->value();
}

void RtsHuman::HandleClusterSlide() const {
    auto numStr = to_string(ui->clusterSlider->value());
    ui->clusterLabel->setText(QString::fromStdString("Cluster/Expansion: " + numStr));
    clusterCnt = ui->clusterSlider->value();
}

void RtsHuman::HandleResourceSlide() const {
    auto numStr = to_string(ui->resourceSlider->value());
    ui->resourceLabel->setText(QString::fromStdString("Resource/Cluster: " + numStr));
    resourceCnt = ui->resourceSlider->value();
}

void RtsHuman::HandleSeedLineEdit() const {
    seed = stoi(ui->seedLineEdit->text().toStdString());
}

void RtsHuman::HandleIsAxialRadio() const {
    isAxSym = ui->axialRadio->isChecked();
}
