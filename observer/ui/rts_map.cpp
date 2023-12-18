//
// Created by zoe50 on 2023/12/17.
//

#include "rts_map.h"
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>

void RtsMap::initializeGL() {
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();
}

void RtsMap::resizeGL(int w, int h) {
    glViewport(0,0,std::min(w,h),std::min(w, h));
    if (w != h) {
        resize(std::min(w,h),std::min(w, h));
    }
}

void RtsMap::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    // Resetear transformaciones
    glLoadIdentity();
    glTranslatef(-0.5, -0.5, 0);
    glRotatef(30, 1, 0, 0);



    // LADO IZQUIERDO: lado verde
    glBegin(GL_POLYGON);


    glColor3f( 1.0, 0.0, 0.0 );
    glVertex3f(  0.5, -0.5, -0.5 );      // P1 es rojo
    glColor3f( 0.0, 1.0, 0.0 );
    glVertex3f(  0.5,  0.5, -0.5 );      // P2 es verde
    glColor3f( 0.0, 0.0, 1.0 );
    glVertex3f( -0.5,  0.5, -0.5 );      // P3 es azul
    glColor3f( 1.0, 0.0, 1.0 );
    glVertex3f( -0.5, -0.5, -0.5 );      // P4 es morado



    glEnd();


    glFlush();
    this->makeCurrent();
}
