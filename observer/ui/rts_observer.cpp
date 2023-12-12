//
// Created by zoe50 on 2023/12/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_rts_observer.h" resolved

#include "rts_observer.h"
#include "ui_rts_observer.h"
#include <iostream>
#include <QtWidgets/QMessageBox>
#include "rpc_client.h"

using namespace std;

ThreadPool RtsObserver::threadPool {8};

RtsObserver::RtsObserver(QWidget *parent) :
        QWidget(parent), ui(new Ui::RtsObserver) {
    ui->setupUi(this);


    connect(ui->connectButton, &QPushButton::released, this, &RtsObserver::HandleConnectButton);
}

RtsObserver::~RtsObserver() {
    RpcClient::SendCommand(static_cast<message::Command>(DISCONNECT));
    delete ui;
}

void RtsObserver::HandleConnectButton() {
    static bool isStarted = false;
    auto ip = ui->ipLineEdit->text().toStdString();
    auto port = ui->portLineEdit->text().toStdString();
    if (isStarted) {
        RpcClient::SendCommand(static_cast<message::Command>(DISCONNECT));
    }
    threadPool.enqueue(RpcClient::Connect, ip + ":" + port);
    isStarted = true;
}
