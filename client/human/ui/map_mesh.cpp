#include "map_mesh.h"
#include <QString>
#include <QOpenGLShaderProgram>
MapMesh::MapMesh(const std::vector<QVector3D> &vertice, const std::vector<GLuint> &indice, const QVector4D& color):
        vertices(vertice),
        indices(indice),
        color(color),
        origColor(color),
        isinitialized(false)
{
    //初始化OpenGL函数
    initializeOpenGLFunctions();
}
MapMesh::~MapMesh()
{
    VAO.destroy();
    VBO.destroy();
    EBO.destroy();
}
void MapMesh::setupMesh(QOpenGLShaderProgram *program)
{
    // 创建缓冲区/数组
    VAO.create();
    VBO.create();
    EBO.create();

    VAO.bind();
    VBO.bind();

    // 关于结构的一个好处是它们的内存布局对于它的所有项都是顺序的。
    // 结果是我们可以简单地将指针传递给结构，并且它完美地转换为glm :: vec3 / 2数组，该数组再次转换为3/2浮点数，转换为字节数组。
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D), vertices.data()/*&vertices[0]*/, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO.bufferId());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data()/*&indices[0]*/, GL_STATIC_DRAW);
    // 设置顶点属性指针
    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *)0);
    VAO.release();
    VBO.release();
}
#include <QMessageBox>
void MapMesh::draw(QOpenGLShaderProgram *program)
{

    if (!isinitialized)
    {
        setupMesh(program);
        //setupMesh进行了初始化，状态置为true
    }
    // bind appropriate color
    program->setUniformValue("color", color);
    isinitialized = true;

    // draw mesh
    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    VAO.release();
}