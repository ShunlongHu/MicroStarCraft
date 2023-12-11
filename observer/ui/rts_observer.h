//
// Created by zoe50 on 2023/12/11.
//

#ifndef RTS_RTS_OBSERVER_H
#define RTS_RTS_OBSERVER_H

#include <QtWidgets/QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class RtsObserver; }
QT_END_NAMESPACE

class RtsObserver : public QWidget {
    Q_OBJECT

public:
    explicit RtsObserver(QWidget *parent = nullptr);

    ~RtsObserver() override;

private:
    Ui::RtsObserver *ui;
};


#endif //RTS_RTS_OBSERVER_H
