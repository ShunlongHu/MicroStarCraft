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
    static int seed;
    static double terrainProb;
    static int expansionCnt;
    static int clusterCnt;
    static int resourceCnt;

private slots:
    void HandleConnectButton() const;
    void HandleStartButton();
    void HandleStopButton();
    void HandleStepButton();
    void HandleResetButton();
    void HandleRenderTimer();
    void HandleSeedLineEdit() const;
    void HandleTerrainSlide() const;
    void HandleExpansionSlide() const;
    void HandleClusterSlide() const;
    void HandleResourceSlide() const;
    void RefreshButton();
};


#endif //RTS_RTS_OBSERVER_H
