//
// Created by zoe50 on 2023/12/29.
//

#ifndef RTS_TEXT_MESH_H
#define RTS_TEXT_MESH_H
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector4D>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOffscreenSurface>
#include "qopenglfunctions_3_3_core.h"

class TextMesh : protected QOpenGLFunctions_3_3_Core {
public:
    TextMesh();
    ~TextMesh();
    void RenderText(QOpenGLShaderProgram &shader, const std::string& text, float x, float y, float scale, QVector3D color);
    void Init();
private:
    struct Character {
        std::shared_ptr<QOpenGLTexture> texture; // ID handle of the glyph texture
        QVector2D   size;      // Size of glyph
        QVector2D   bearing;   // Offset from baseline to left/top of glyph
        unsigned int advance;   // Horizontal offset to advance to next glyph
    };
    std::unordered_map<GLchar, Character> charTextureMap;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
    std::shared_ptr<QOpenGLContext> context;
    std::shared_ptr<QOffscreenSurface> surface;
    bool isInitialized = false;
};


#endif //RTS_TEXT_MESH_H
