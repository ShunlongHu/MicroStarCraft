//
// Created by zoe50 on 2023/12/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_rts_observer.h" resolved

#include "rts_observer.h"
#include "ui_rts_observer.h"
#include <iostream>
#include <chrono>
#include <QtWidgets/QMessageBox>
#include "rpc_client.h"

using namespace std;
using namespace chrono;
using namespace this_thread;

ThreadPool RtsObserver::threadPool {8};

RtsObserver::RtsObserver(QWidget *parent) :
        QWidget(parent), ui(new Ui::RtsObserver) {
    ui->setupUi(this);
    connect(ui->connectButton, &QPushButton::released, this, &RtsObserver::HandleConnectButton);
    connect(&renderTimer, &QTimer::timeout, this, &RtsObserver::HandleRenderTimer);
    renderTimer.setInterval(10);
    renderTimer.start();
    connect(ui->startButton, &QPushButton::released, this, &RtsObserver::HandleStartButton);
    connect(ui->stopButton, &QPushButton::released, this, &RtsObserver::HandleStopButton);
    connect(ui->stepButton, &QPushButton::released, this, &RtsObserver::HandleStepButton);
    connect(ui->resetButton, &QPushButton::released, this, &RtsObserver::HandleResetButton);

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
