//
// Created by zoe50 on 2023/12/11.
//

#ifndef RTS_RTS_OBSERVER_H
#define RTS_RTS_OBSERVER_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtCore/QTimer>
#include <memory>
#include "thread_pool.h"
#include "rpc_client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RtsObserver; }
QT_END_NAMESPACE

class RtsObserver : public QWidget {
    Q_OBJECT

public:
    explicit RtsObserver(QWidget *parent = nullptr);

    ~RtsObserver() override;

public:
    Ui::RtsObserver *ui;
    static ThreadPool threadPool;
    QTimer renderTimer;

private slots:
    void HandleConnectButton();
    void HandleRenderTimer();
};


#endif //RTS_RTS_OBSERVER_H
