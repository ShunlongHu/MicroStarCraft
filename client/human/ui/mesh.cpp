#include "mesh.h"
#include <QString>
#include <QOpenGLShaderProgram>
Mesh::Mesh(const std::vector<Vertex> &vertice, const std::vector<GLuint> &indice,
           const std::vector<Texture> &texture):
        vertices(vertice),
        indices(indice),
        textures(texture),
        isinitialized(false)
{
    //初始化OpenGL函数
    initializeOpenGLFunctions();
}
Mesh::~Mesh()
{
    VAO.destroy();
    VBO.destroy();
    EBO.destroy();
}
void Mesh::setupMesh(QOpenGLShaderProgram *program)
{
    // 创建缓冲区/数组
    VAO.create();
    VBO.create();
    EBO.create();

    VAO.bind();
    VBO.bind();

    // 关于结构的一个好处是它们的内存布局对于它的所有项都是顺序的。
    // 结果是我们可以简单地将指针传递给结构，并且它完美地转换为glm :: vec3 / 2数组，该数组再次转换为3/2浮点数，转换为字节数组。
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data()/*&vertices[0]*/, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO.bufferId());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data()/*&indices[0]*/, GL_STATIC_DRAW);
    // 设置顶点属性指针
    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    // u向量
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
    // v向量
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
    VAO.release();
    VBO.release();
}
#include <QMessageBox>
void Mesh::draw(QOpenGLShaderProgram *program)
{

    if (!isinitialized)
    {
        setupMesh(program);
        //setupMesh进行了初始化，状态置为true

    }

    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    unsigned int emissionNr = 1;
    unsigned int opacityNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {

        // retrieve texture number (the N in diffuse_textureN)
        QString number;
        QString type = textures[i].type;
        if (type == "texture_diffuse")
            number = QString::number(diffuseNr++);
        else if (type == "texture_specular")
            number = QString::number(specularNr++); // transfer unsigned int to stream
        else if (type == "texture_normal")
            number = QString::number(normalNr++);   // transfer unsigned int to stream
        else if (type == "texture_height")
            number = QString::number(heightNr++);   // transfer unsigned int to stream
        else if (type == "texture_emission")
            number = QString::number(emissionNr++);   // transfer unsigned int to stream
        else if (type == "texture_opacity")
            number = QString::number(opacityNr++);   // transfer unsigned int to stream
        type = "material." + type;
        qDebug() << QString(type + number).toLocal8Bit().constData();
        textures[i].texture->bind(static_cast<unsigned int>(i));
//        if (!isinitialized) {
//            static QMessageBox qMessageBox;
//            qMessageBox.setText(qMessageBox.text() + '\n' + type + number);
//            qMessageBox.show();
//        }
        program->setUniformValue(QString(type + number).toLocal8Bit().constData(), i);

    }

    program->setUniformValue("material.is_specular", specularNr > 1);
    program->setUniformValue("material.is_emission", emissionNr > 1);
    program->setUniformValue("material.is_normal", normalNr > 1);
    program->setUniformValue("material.is_opacity", opacityNr > 1);

    program->setUniformValue("material.shininess", shininess);
    isinitialized = true;

    // draw mesh
    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    VAO.release();
}