//
// Created by zoe50 on 2024/1/8.
//

#ifndef RTS_LINE_H
#define RTS_LINE_H
#include <vector>
#include <string>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector4D>
#include <QOpenGLShaderProgram>
#include "qopenglfunctions_3_3_core.h"

class Line : protected QOpenGLFunctions_3_3_Core {
public:
    Line() { initializeOpenGLFunctions(); };
    ~Line() {
        VAO.destroy();
        VBO.destroy();
    };
    //画网格模型
    void draw(QOpenGLShaderProgram *program, QVector4D color, const std::vector<QVector3D>& vertices);
    //是否进行过setupMesh初始化操作
    bool isinitialized = false;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
private:
    // 初始化所有缓冲区对象/数组
    void setupMesh(QOpenGLShaderProgram *program);
};


#endif //RTS_LINE_H
