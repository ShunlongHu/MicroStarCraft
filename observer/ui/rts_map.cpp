//
// Created by zoe50 on 2023/12/17.
//

#include "rts_map.h"
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QGL>
#include <fstream>
#include <iostream>
#include <sstream>
#include <QMessageBox>

using namespace std;

void RtsMap::initializeGL() {
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();

//    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//! [2]

//    geometries = make_unique<GeometryEngine>();


    //给着色器变量赋值,projextion,view默认构造是生成单位矩阵
    QMatrix4x4 projection, view, model;
    view.translate(QVector3D(0.0f, 0.0f, -5.0f));
    projection.perspective(45.0f, 1.0f, 3.0f, 7.0f);

    program.setUniformValue("view", view);
    program.setUniformValue("model", model);
    program.setUniformValue("projection", projection);

    /* 固定属性区域 */
    glEnable(GL_DEPTH_TEST);  //开启深度测试

}

void RtsMap::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
    if (w != h) {
        resize(std::min(w,h),std::min(w, h));
    }
    projection.setToIdentity();
    projection.perspective(45.0, 1.0, 3.0, 7.0);
    update();
}

void RtsMap::paintGL() {
//    // Clear color and depth buffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    texture->bind(0);
//    texture2->bind(1);
///*
// * First cube
// */
////! [6]
//    // Calculate model view transformation
//    QMatrix4x4 matrix;
//    matrix.translate(0.0, 0.0, -5.0);
//    rotationAxis = (rotationAxis*1 + 1 * QVector3D{1,0.6,0.3});
//    rotationAxis = rotationAxis.normalized();
//    rotation = QQuaternion::fromAxisAndAngle(rotationAxis, 1) * rotation;
//    matrix.rotate(rotation);
//
//    // Set modelview-projection matrix
//    program.setUniformValue("mvp_matrix", projection * matrix);
////! [6]
//
//    // Use texture unit 0 which contains cube.png
////    program.setUniformValue("texture", 0);
//
//    // Draw cube geometry
////    geometries->drawCubeGeometry(&program);
///*
// * Second cube
// */
//    // Set modelview-projection matrix
//    matrix.translate({-0.5, -0.5,-0.1});
//    program.setUniformValue("mvp_matrix", projection * matrix);
////! [6]
//
//    // Use texture unit 0 which contains cube.png
////    program.setUniformValue("texture", 1);
//
//    // Draw cube geometry
////    geometries->drawCubeGeometry(&program);
//    pModel->Draw(&program);
//
//    this->makeCurrent();
//清理屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
    QMatrix4x4 model;
    //model.scale(0.005f, 0.005f, 0.005f);
    //model.scale(0.010f, 0.010f, 0.010f);
    model.scale(0.15f, 0.15f, 0.15f);
    //model.rotate((float)time.elapsed() / 10, QVector3D(0.5f, 1.0f, 0.0f));
    model.rotate((float)time.elapsed() / 20, QVector3D(0.0f, 0.5f, 1.0f));
    if (!program.bind())
    {
        static QMessageBox messageBox;
        messageBox.setText("bind error" + program.log());
        messageBox.show();
        close();
    }
    program.setUniformValue("model", model);
    pModel->draw(&program);
    program.release();
    update();
}



RtsMap::RtsMap(QWidget *parent): QOpenGLWidget(parent) {
    xRot = yRot = zRot = 0.0;
    zoom = -5.0;
    xSpeed = ySpeed = 0.0;

    filter = 0;

    light = true;

    //设置OpenGL的版本信息
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    //指定opengl版本为3.3
    format.setVersion(3, 3);
    setFormat(format);
    setCursor(Qt::ArrowCursor);
}

void RtsMap::initShaders() {
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, "D:\\repo\\rts\\observer\\ui\\model.vs")){
        static QMessageBox messageBox;
        messageBox.setText("compile error" + program.log());
        messageBox.show();
        close();
    }


    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, "D:\\repo\\rts\\observer\\ui\\model.fs")){
        static QMessageBox messageBox;
        messageBox.setText("compile error" + program.log());
        messageBox.show();
        close();
    }

    // Link shader pipeline
    if (!program.link()) {
        static QMessageBox messageBox;
        messageBox.setText("link error" + program.log());
        messageBox.show();
        close();
    }

    pModel = make_unique<Model>("D:\\repo\\rts\\observer\\ui\\resource\\nanosuit\\nanosuit.obj");
//    pModel = make_unique<Model>("D:/repo/rts/observer/ui/resource/CG人物/吉姆雷诺raynor/$]V[YKU$Z}`~3V9`{A54{IR.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_VERTEX_ARRAY, 0);
//    glBindVertexArray(0);//结束记录状态信息

    // Bind shader pipeline for use
    if (!program.bind()) {
        static QMessageBox messageBox;
        messageBox.setText("bind error" + program.log());
        messageBox.show();
        close();
    }
}

void RtsMap::initTextures() {
//    // Load cube.png image
//    texture = make_unique<QOpenGLTexture>(QImage("D:/repo/rts/observer/ui/resource/CG人物/星际2- 诺娃nova/[8%OKO@UE(L_X1O[3HWMNZ9.jpg").mirrored());
//
//    // Set nearest filtering mode for texture minification
//    texture->setMinificationFilter(QOpenGLTexture::Linear);
//
//    // Set bilinear filtering mode for texture magnification
//    texture->setMagnificationFilter(QOpenGLTexture::Linear);
//
//    // Wrap texture coordinates by repeating
//    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
//    texture->setWrapMode(QOpenGLTexture::Repeat);
//
//
//    // Load cube.png image
//    texture2 = make_unique<QOpenGLTexture>(QImage("D:/repo/rts/observer/ui/resource/CG人物/吉姆雷诺raynor/$]V[YKU$Z}`~3V9`{A54{IR.jpg").mirrored());
//
//    // Set nearest filtering mode for texture minification
//    texture2->setMinificationFilter(QOpenGLTexture::Linear);
//
//    // Set bilinear filtering mode for texture magnification
//    texture2->setMagnificationFilter(QOpenGLTexture::Linear);
//
//    // Wrap texture coordinates by repeating
//    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
//    texture2->setWrapMode(QOpenGLTexture::Repeat);
}


RtsMap::~RtsMap() {
    //删除所有之前添加到program的着色器
    program.removeAllShaders();
    makeCurrent();
    doneCurrent();
}