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
namespace Ui { class RtsHuman; }
QT_END_NAMESPACE

class RtsHuman : public QWidget {
    Q_OBJECT

public:
    explicit RtsHuman(QWidget *parent = nullptr);

    ~RtsHuman() override;

public:
    Ui::RtsHuman *ui;
    static ThreadPool threadPool;
    QTimer renderTimer;
    static int seed;
    static double terrainProb;
    static int expansionCnt;
    static int clusterCnt;
    static int resourceCnt;
    static bool isAxSym; // is axial sym or rotational sym

private slots:
    void HandleConnectButton();
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
    void HandleIsAxialRadio() const;
    void RefreshButton();
};


#endif //RTS_RTS_OBSERVER_H
