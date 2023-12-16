//
// Created by zoe50 on 2023/12/17.
//

#include "rts_map.h"

void RtsMap::initializeGL() {
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();
}

void RtsMap::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
}

void RtsMap::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    // Resetear transformaciones
    glLoadIdentity();


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
