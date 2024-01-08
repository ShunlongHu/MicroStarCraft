//
// Created by zoe50 on 2024/1/8.
//

#include "line.h"

void Line::setupMesh(QOpenGLShaderProgram *program) {
// 创建缓冲区/数组
    VAO.create();
    VBO.create();

    VAO.bind();
    VBO.bind();

    // 关于结构的一个好处是它们的内存布局对于它的所有项都是顺序的。
    // 结果是我们可以简单地将指针传递给结构，并且它完美地转换为glm :: vec3 / 2数组，该数组再次转换为3/2浮点数，转换为字节数组。
    glBufferData(GL_ARRAY_BUFFER, 0 * sizeof(QVector3D), nullptr/*&vertices[0]*/, GL_DYNAMIC_DRAW);
    // 设置顶点属性指针
    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *)0);

    VAO.release();
    VBO.release();
}

void Line::draw(QOpenGLShaderProgram *program, QVector4D color, const std::vector<QVector3D>& vertices) {
    if (!isinitialized)
    {
        setupMesh(program);
        //setupMesh进行了初始化，状态置为true
    }
    // bind appropriate color
    program->setUniformValue("color", color);
    isinitialized = true;
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
//    glLineWidth(200);
    VAO.bind();
    VBO.bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D), vertices.data(), GL_DYNAMIC_DRAW); // be sure to use glBufferSubData and not glBufferData
    VBO.release();

    glDrawArrays(GL_LINES, 0, vertices.size());
    VAO.release();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}


