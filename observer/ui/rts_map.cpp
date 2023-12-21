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

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    geometries = make_unique<GeometryEngine>();
}

void RtsMap::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
    if (w != h) {
        resize(std::min(w,h),std::min(w, h));
    }
    projection.setToIdentity();
    projection.perspective(45.0, 1.0, 3.0, 7.0);
}

void RtsMap::paintGL() {
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();
/*
 * First cube
 */
//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    rotationAxis = (rotationAxis*1 + 1 * QVector3D{1,0.6,0.3});
    rotationAxis = rotationAxis.normalized();
    rotation = QQuaternion::fromAxisAndAngle(rotationAxis, 1) * rotation;
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);
//! [6]

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(&program);
/*
 * Second cube
 */
    texture2->bind();
    // Set modelview-projection matrix
    matrix.translate({-0.5, -0.5,-0.1});
    program.setUniformValue("mvp_matrix", projection * matrix);
//! [6]

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(&program);

    this->makeCurrent();
}



RtsMap::RtsMap(QWidget *parent): QOpenGLWidget(parent) {
    xRot = yRot = zRot = 0.0;
    zoom = -5.0;
    xSpeed = ySpeed = 0.0;

    filter = 0;

    light = true;
}

void RtsMap::initShaders() {
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, R"(D:\repo\rts\observer\ui\coord_sys.vs)"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, R"(D:\repo\rts\observer\ui\coord_sys.fs)"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void RtsMap::initTextures() {
    // Load cube.png image
    texture = make_unique<QOpenGLTexture>(QImage("D:/repo/rts/observer/ui/resource/CG人物/星际2- 诺娃nova/[8%OKO@UE(L_X1O[3HWMNZ9.jpg").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Linear);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);


    // Load cube.png image
    texture2 = make_unique<QOpenGLTexture>(QImage("D:/repo/rts/observer/ui/resource/CG人物/吉姆雷诺raynor/$]V[YKU$Z}`~3V9`{A54{IR.jpg").mirrored());

    // Set nearest filtering mode for texture minification
    texture2->setMinificationFilter(QOpenGLTexture::Linear);

    // Set bilinear filtering mode for texture magnification
    texture2->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture2->setWrapMode(QOpenGLTexture::Repeat);
}


RtsMap::~RtsMap() {
    makeCurrent();
    doneCurrent();
}