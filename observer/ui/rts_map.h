//
// Created by zoe50 on 2023/12/17.
//

#ifndef RTS_RTS_MAP_H
#define RTS_RTS_MAP_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObjectFormat>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "geometry_engine.h"
class RtsMap : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    explicit RtsMap(QWidget* parent);
    ~RtsMap();
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void initShaders();
    void initTextures();
    void graficarLines();
    void loadGLTextures();
    void checkCompileErrors(GLuint shader, std::string type);

    bool fullscreen;
    GLfloat xRot, yRot, zRot;
    GLfloat zoom;
    GLfloat xSpeed, ySpeed;
//    GLuint texture[3];
    GLuint filter;

    bool light;

public:
    double y=0;
    double x=0;
    double z=0;

    int w = 0, h = 0;

    QOpenGLShaderProgram program;
    GeometryEngine *geometries = nullptr;

    QOpenGLTexture *texture = nullptr;
    QOpenGLTexture *texture2 = nullptr;

    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis {1,1,1};
    qreal angularSpeed = 0;
    QQuaternion rotation;
};


#endif //RTS_RTS_MAP_H
