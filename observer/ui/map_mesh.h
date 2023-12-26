#ifndef MAP_MESH_H
#define MAP_MESH_H
#include "vertex.h"
#include "texture.h"

#include <vector>
#include <string>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector4D>
#include "qopenglfunctions_3_3_core.h"
class QOpenGLShaderProgram;

class MapMesh : protected QOpenGLFunctions_3_3_Core
{
public:
    MapMesh() = default;
    ~MapMesh();
    //构造
    MapMesh(const std::vector<QVector3D> &vertices, const std::vector<GLuint> &indices, const QVector4D& color);
    //画网格模型
    void draw(QOpenGLShaderProgram *program);
    //顶点
    std::vector<QVector3D> vertices;
    //索引
    std::vector<GLuint> indices;
    //material
    QVector4D color;
    //是否进行过setupMesh初始化操作
    bool isinitialized;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
    QOpenGLBuffer EBO;
private:
    // 初始化所有缓冲区对象/数组
    void setupMesh(QOpenGLShaderProgram *program);
};

#endif // !MAP_MESH_H