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

GLfloat lightAmbient[4] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat lightDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lightPosition[4] = { 0.0, 0.0, 2.0, 1.0 };
//
//static glm::mat4 projection {glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f)};
//static glm::mat4 view {1.0f};
//static glm::mat4 model {1.0f};

static GLuint ID;

void RtsMap::checkCompileErrors(GLuint shader, std::string type)
{
    ostringstream oss;
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            oss << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            static QMessageBox messageBox;
            messageBox.setText(QString::fromStdString(oss.str()));
            messageBox.show();
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            oss << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            static QMessageBox messageBox;
            messageBox.setText(QString::fromStdString(oss.str()));
            messageBox.show();
        }
    }
}

void RtsMap::initializeGL() {
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();
//    loadGLTextures();
//
//    glEnable( GL_TEXTURE_2D );
//    glShadeModel( GL_SMOOTH );
//    glClearColor( 0.0, 0.0, 0.0, 0.5 );
//    glClearDepth( 1.0 );
//    glEnable( GL_DEPTH_TEST );
//    glDepthFunc( GL_LEQUAL );
//    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
//
//    glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );
//    glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse );
//    glLightfv( GL_LIGHT1, GL_POSITION, lightPosition );
//
//    glEnable( GL_LIGHT1 );
//
//    std::string vertexCode;
//    std::string fragmentCode;
//    std::ifstream vShaderFile;
//    std::ifstream fShaderFile;
//    // ensure ifstream objects can throw exceptions:
//    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    try
//    {
//        // open files
//        vShaderFile.open("D:\\repo\\rts\\observer\\ui\\coord_sys.vs");
//        fShaderFile.open("D:\\repo\\rts\\observer\\ui\\coord_sys.fs");
//        std::stringstream vShaderStream, fShaderStream;
//        // read file's buffer contents into streams
//        vShaderStream << vShaderFile.rdbuf();
//        fShaderStream << fShaderFile.rdbuf();
//        // close file handlers
//        vShaderFile.close();
//        fShaderFile.close();
//        // convert stream into string
//        vertexCode = vShaderStream.str();
//        fragmentCode = fShaderStream.str();
//    }
//    catch (std::ifstream::failure& e)
//    {
//        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
//    }
//    const char* vShaderCode = vertexCode.c_str();
//    const char * fShaderCode = fragmentCode.c_str();
//    // 2. compile shaders
//    unsigned int vertex, fragment;
//    // vertex shader
//    vertex = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertex, 1, &vShaderCode, NULL);
//    glCompileShader(vertex);
//    checkCompileErrors(vertex, "VERTEX");
//
//    // fragment Shader
//    fragment = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragment, 1, &fShaderCode, NULL);
//    glCompileShader(fragment);
//    checkCompileErrors(fragment, "FRAGMENT");
//
//    // shader Program
//    ID = glCreateProgram();
//    glAttachShader(ID, vertex);
//    glAttachShader(ID, fragment);
//    glLinkProgram(ID);
//    checkCompileErrors(ID, "PROGRAM");
//
//    // delete the shaders as they're linked into our program now and no longer necessary
//    glDeleteShader(vertex);
//    glDeleteShader(fragment);
//    glUseProgram(ID);

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    geometries = new GeometryEngine;
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
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//    glDepthFunc(GL_LESS);
//    glEnable(GL_DEPTH_TEST);
//    glShadeModel(GL_SMOOTH);
//    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
//    // Resetear transformaciones
//    glLoadIdentity();
//    glTranslatef(-0.5, -0.5, 0);
//    glRotatef(30, 1, 0, 0);
//
//
//
//    // LADO IZQUIERDO: lado verde
//    glBegin(GL_POLYGON);
//
//
//    glColor3f( 1.0, 0.0, 0.0 );
//    glVertex3f(  0.5, -0.5, -0.5 );      // P1 es rojo
//    glColor3f( 0.0, 1.0, 0.0 );
//    glVertex3f(  0.5,  0.5, -0.5 );      // P2 es verde
//    glColor3f( 0.0, 0.0, 1.0 );
//    glVertex3f( -0.5,  0.5, -0.5 );      // P3 es azul
//    glColor3f( 1.0, 0.0, 1.0 );
//    glVertex3f( -0.5, -0.5, -0.5 );      // P4 es morado
//
//
//
//    glEnd();
//
//
//    glFlush();

////////////////////////////////////////////

//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    glLoadIdentity();
//    glTranslatef(  0.0,  0.0, 0.0 );

//    glRotatef( 30,  1.0,  0.0,  0.0 );
//    glRotatef( 30,  0.0,  1.0,  0.0 );
//    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.3f, 0.5f));
//    glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, &projection[0][0]);
//    glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, &view[0][0]);
//    glUniformMatrix4fv(glGetUniformLocation(ID, "model"), 1, GL_FALSE, &model[0][0]);
//
//
//    glBindTexture( GL_TEXTURE_2D, texture[filter] );
//    glBegin( GL_QUADS );
//    glNormal3f( 0.0, 0.0, 0.5 );
//    glTexCoord2f( 0.0, 0.0 ); glVertex3f( -0.5, -0.5,  0.5 );
//    glTexCoord2f( 1.0, 0.0 ); glVertex3f(  0.5, -0.5,  0.5 );
//    glTexCoord2f( 1.0, 1.0 ); glVertex3f(  0.5,  0.5,  0.5 );
//    glTexCoord2f( 0.0, 1.0 ); glVertex3f( -0.5,  0.5,  0.5 );
//
//    glNormal3f( 0.0, 0.0, -0.5 );
//    glTexCoord2f( 1.0, 0.0 ); glVertex3f( -0.5, -0.5, -0.5 );
//    glTexCoord2f( 1.0, 1.0 ); glVertex3f( -0.5,  0.5, -0.5 );
//    glTexCoord2f( 0.0, 1.0 ); glVertex3f(  0.5,  0.5, -0.5 );
//    glTexCoord2f( 0.0, 0.0 ); glVertex3f(  0.5, -0.5, -0.5 );
//
//    glNormal3f( 0.0, 0.5, 0.0 );
//    glTexCoord2f( 0.0, 1.0 ); glVertex3f( -0.5,  0.5, -0.5 );
//    glTexCoord2f( 0.0, 0.0 ); glVertex3f( -0.5,  0.5,  0.5 );
//    glTexCoord2f( 1.0, 0.0 ); glVertex3f(  0.5,  0.5,  0.5 );
//    glTexCoord2f( 1.0, 1.0 ); glVertex3f(  0.5,  0.5, -0.5 );
//
//    glNormal3f( 0.0, -0.5, 0.0 );
//    glTexCoord2f( 1.0, 1.0 ); glVertex3f( -0.5, -0.5, -0.5 );
//    glTexCoord2f( 0.0, 1.0 ); glVertex3f(  0.5, -0.5, -0.5 );
//    glTexCoord2f( 0.0, 0.0 ); glVertex3f(  0.5, -0.5,  0.5 );
//    glTexCoord2f( 1.0, 0.0 ); glVertex3f( -0.5, -0.5,  0.5 );
//
//    glNormal3f( 0.5, 0.0, 0.0 );
//    glTexCoord2f( 1.0, 0.0 ); glVertex3f(  0.5, -0.5, -0.5 );
//    glTexCoord2f( 1.0, 1.0 ); glVertex3f(  0.5,  0.5, -0.5 );
//    glTexCoord2f( 0.0, 1.0 ); glVertex3f(  0.5,  0.5,  0.5 );
//    glTexCoord2f( 0.0, 0.0 ); glVertex3f(  0.5, -0.5,  0.5 );
//
//    glNormal3f( -0.5, 0.0, 0.0 );
//    glTexCoord2f( 0.0, 0.0 ); glVertex3f( -0.5, -0.5, -0.5 );
//    glTexCoord2f( 1.0, 0.0 ); glVertex3f( -0.5, -0.5,  0.5 );
//    glTexCoord2f( 1.0, 1.0 ); glVertex3f( -0.5,  0.5,  0.5 );
//    glTexCoord2f( 0.0, 1.0 ); glVertex3f( -0.5,  0.5, -0.5 );
//    glEnd();
//    glFlush();
//    glEnable(GL_LIGHTING);
//    glDisable(GL_LIGHTING);
//    updateGeometry();

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

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


    this->makeCurrent();
}




//void RtsMap::loadGLTextures() {
//    QImage tex, buf;
//    if ( !buf.load( "D:/repo/rts/observer/ui/resource/CG人物/星际2- 诺娃nova/[8%OKO@UE(L_X1O[3HWMNZ9.jpg" ) )
//    {
//        qWarning( "Could not read image file, using single-color instead." );
//        QImage dummy( 128, 128, QImage::Format_RGB32);
//        dummy.fill( Qt::green );
//        buf = dummy;
//    }
//    tex = QGLWidget::convertToGLFormat( buf );
//
//    glGenTextures( 3, &texture[0] );
//    glBindTexture( GL_TEXTURE_2D, texture[0] );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
//    glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
//                  GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );
//    glBindTexture( GL_TEXTURE_2D, texture[1] );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
//                  GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );
//
//    glBindTexture( GL_TEXTURE_2D, texture[2] );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
//}

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
    texture = new QOpenGLTexture(QImage("D:/repo/rts/observer/ui/resource/CG人物/星际2- 诺娃nova/[8%OKO@UE(L_X1O[3HWMNZ9.jpg").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Linear);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}


RtsMap::~RtsMap() {
    makeCurrent();
    delete texture;
    delete geometries;
    doneCurrent();
}