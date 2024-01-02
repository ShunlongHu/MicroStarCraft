//
// Created by zoe50 on 2023/12/29.
//

#include "text_mesh.h"
#include <freetype/freetype.h>
#include <QMessageBox>
using namespace std;
TextMesh::TextMesh()
{
    context = make_shared<QOpenGLContext>();
    surface = make_shared<QOffscreenSurface>();
    context->makeCurrent(surface.get());
    initializeOpenGLFunctions();
}
TextMesh::~TextMesh()
{
    VAO.destroy();
    VBO.destroy();
}

void TextMesh::RenderText(QOpenGLShaderProgram &shader, const string& text, float x, float y, float scale, QVector3D color)
{
    if (!isInitialized) {
        Init();
        isInitialized = true;
    }
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.setUniformValue("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    VAO.bind();
    int nCount = text.size();
    for (int i = 0; i < nCount; i++)
    {
        char c = text[i];
        auto& [_,ch] = *charTextureMap.find(c);

        if (ch.texture != nullptr)
        {
            float xpos = x + ch.bearing.x() * scale;
            float ypos = y - (ch.size.y() - ch.bearing.y()) * scale;

            float w = ch.size.x() * scale;
            float h = ch.size.y() * scale;
            // update VBO for each character
            float vertices[6][4] = {
                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },

                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            ch.texture->bind();
            VBO.bind();
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            VBO.release();
            glDrawArrays(GL_TRIANGLES, 0, 6);
            x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            ch.texture->release();
        }
    }
    VAO.release();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void TextMesh::Init() {
    context->makeCurrent(surface.get());
    static QMessageBox box;
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        box.setText("ERROR::FREETYPE: Could not init FreeType Library");
        box.show();
        return;
    }
    std::string fontName = "D:\\repo\\rts\\observer\\ui\\resource\\fonts\\Antonio-Bold.ttf";
    FT_Face face;
    if (FT_New_Face(ft, fontName.c_str(), 0, &face)) {
        box.setText(box.text() + '\n' + "ERROR::FREETYPE: Failed to load font");
        box.show();
        return;
    } else {
        FT_Set_Pixel_Sizes(face, 0, 48);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (unsigned char c = 0; c < 128; c++)        {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                box.setText(box.text() + '\n' + "ERROR::FREETYPE: Failed to load Glyph");
                box.show();
                continue;
            }
            // generate texture
            Character ch;
            ch.texture = make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D);
            ch.texture->setMinificationFilter(QOpenGLTexture::Nearest);
            ch.texture->setMagnificationFilter(QOpenGLTexture::Linear);
            ch.texture->setWrapMode(QOpenGLTexture::Repeat);
            ch.texture->create();
            ch.texture->setSize(face->glyph->bitmap.width, face->glyph->bitmap.rows, 1);
#ifdef PC_SETTINGS
            text->setFormat(QOpenGLTexture::RGBA8_UNorm);
#endif // PC_SETTINGS
#ifdef BOARD_SETTINGS
            text->setFormat(QOpenGLTexture::AlphaFormat);
#endif // BOARD_SETTINGS
            //    text->allocateStorage();
            ch.texture->allocateStorage(QOpenGLTexture::Alpha, QOpenGLTexture::UInt8);
            ch.texture->setData(QOpenGLTexture::Alpha, QOpenGLTexture::UInt8, face->glyph->bitmap.buffer);
            ch.size = QVector2D(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            ch.bearing = QVector2D(face->glyph->bitmap_left, face->glyph->bitmap_top);
            ch.advance = face->glyph->advance.x;
            charTextureMap.emplace(c, ch);
        }
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    VAO.create();
    VBO.create();
    VAO.bind();
    VBO.bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    VAO.release();
    VBO.release();
}
//
//
//void TextMesh::RenderText(QOpenGLShaderProgram &shader, std::string text, float x, float y, float scale, QVector3D color)
//{
//    // activate corresponding render state
//    shader.bind();
//    glUniform3f(glGetUniformLocation(shader.programId(), "textColor"), color.x(), color.y(), color.z());
//    txtVAO.bind();
//    glActiveTexture(GL_TEXTURE0);
//    glBindVertexArray(VAO);
//
//    // iterate through all characters
//    for (const auto& c:text)
//    {
//        Character ch = Characters[c];
//
//        float xpos = x +  ch.Bearing.x() * scale;
//        float ypos = y - (ch.Size.y() - ch.Bearing.y()) * scale;
//
//        float w = ch.Size.x() * scale;
//        float h = ch.Size.y() * scale;
//        // update VBO for each character
//        float vertices[6][4] = {
//                { xpos,     ypos + h,   0.0f, 0.0f },
//                { xpos,     ypos,       0.0f, 1.0f },
//                { xpos + w, ypos,       1.0f, 1.0f },
//
//                { xpos,     ypos + h,   0.0f, 0.0f },
//                { xpos + w, ypos,       1.0f, 1.0f },
//                { xpos + w, ypos + h,   1.0f, 0.0f }
//        };
//        // render glyph texture over quad
//        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//        // update content of VBO memory
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
//
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        // render quad
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//    }
//    glBindVertexArray(0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//}
