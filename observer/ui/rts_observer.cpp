//
// Created by zoe50 on 2023/12/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_rts_observer.h" resolved

#include "rts_observer.h"
#include "ui_rts_observer.h"
#include <iostream>

using namespace std;
RtsObserver::RtsObserver(QWidget *parent) :
        QWidget(parent), ui(new Ui::RtsObserver) {
    ui->setupUi(this);
    connect(ui->connectButton, &QPushButton::released, this, &RtsObserver::HandleConnectButton);
}

RtsObserver::~RtsObserver() {
    delete ui;
}

void RtsObserver::HandleConnectButton() {
    static int counter = 0;
    counter++;
    ui->timeDisplayLabel->setText(QString::fromStdString(to_string(counter)));
}
