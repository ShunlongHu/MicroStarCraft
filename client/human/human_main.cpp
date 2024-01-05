//
// Created by zoe50 on 2023/12/10.
//
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include "rts_human.h"
int WinMain(int argc, char *argv[]) {
    QApplication a(argc, argv);
    RtsHuman w;
    w.show();
    return QApplication::exec();
}
