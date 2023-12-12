//
// Created by zoe50 on 2023/12/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_rts_observer.h" resolved

#include "rts_observer.h"
#include "ui_rts_observer.h"
#include <iostream>
#include "rpc_client.h"

using namespace std;

ThreadPool RtsObserver::threadPool {8};

RtsObserver::RtsObserver(QWidget *parent) :
        QWidget(parent), ui(new Ui::RtsObserver) {
    ui->setupUi(this);


    connect(ui->connectButton, &QPushButton::released, this, &RtsObserver::HandleConnectButton);
}

RtsObserver::~RtsObserver() {
    delete ui;
}

void RtsObserver::HandleConnectButton() {
//    auto ip = ui->ipLineEdit->text().toStdString();
//    auto port = ui->portLineEdit->text().toStdString();
//    RpcClient::Connect(ip + ":" + port);
    static int counter = 0;
    counter++;
    ui->timeDisplayLabel->setText(QString::fromStdString(to_string(counter)));
}
