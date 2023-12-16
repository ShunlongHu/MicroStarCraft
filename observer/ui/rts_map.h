//
// Created by zoe50 on 2023/12/17.
//

#ifndef RTS_RTS_MAP_H
#define RTS_RTS_MAP_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObjectFormat>
class RtsMap : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    explicit RtsMap(QWidget* parent): QOpenGLWidget(parent) {}
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void graficarLines();
public:
    double y=0;
    double x=0;
    double z=0;
};


#endif //RTS_RTS_MAP_H
