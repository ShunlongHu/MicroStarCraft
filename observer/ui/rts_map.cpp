#include "rts_map.h"
#include <QOPenglTexture>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <unordered_map>
#include "model.h"
#include "rpc_client.h"

using namespace std;

RtsMap::RtsMap(QWidget *parent) : QOpenGLWidget(parent),
                                                      program(make_shared<QOpenGLShaderProgram>())
{
    //设置OpenGL的版本信息`
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    //指定opengl版本为3.3
    format.setVersion(3, 3);
    setFormat(format);
    setCursor(Qt::ArrowCursor);
}
RtsMap::~RtsMap()
{
    //删除所有之前添加到program的着色器
    program->removeAllShaders();
}
void RtsMap::initializeGL()
{

    //初始化OpenGL函数
    initializeOpenGLFunctions();

    glEnable(GL_CULL_FACE);
    //设置全局变量
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //这里不指定父类，我们自己来管理这个类
    //program = new QOpenGLShaderProgram;
    //将文件内容编译为指定类型的着色器，并将其添加到着色器程序program
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, "D:\\repo\\rts\\observer\\ui\\model.vs"))
    {
        //如果执行不成功，打印错误信息
        qDebug() << "compile error" << program->log();
    }
    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, "D:\\repo\\rts\\observer\\ui\\model.fs"))
    {
        //如果执行不成功，打印错误信息
        qDebug() << "compile error" << program->log();
    }
    //将顶点着色器跟片元着色器链接起来
    if (!program->link())
    {
        //如果连接不成功，打印错误信息
        qDebug() << "link error" << program->log();
    }
    //模型网上自己找个，注意格式要符合assimp库支持的。
//    pModelVec = new Model("D:\\repo\\rts\\observer\\ui\\resource\\objects\\nanosuit\\nanosuit.obj");
//    pModelVec = new Model("D:\\repo\\rts\\observer\\ui\\resource\\cg character\\nova\\dump_obj\\nova.obj");
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Zerg\\drone\\drone.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Zerg\\zergline\\zergline.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Zerg\\hydralisk\\hydralisk.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Zerg\\ultralisk\\ultralisk.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\buildings\\zerg\\zerg_hive.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\buildings\\zerg\\zerg_tower.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Protoss\\probe\\probe.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Protoss\\zealot\\Zealot.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Protoss\\stalker\\stalker.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\race model\\Protoss\\colossus\\colossus.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\buildings\\protoss\\nexus.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\buildings\\protoss\\gateway.obj"));
    pModelVec.emplace_back(make_shared<Model>("D:\\repo\\rts\\observer\\ui\\resource\\buildings\\protoss\\crystal.obj"));


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);//结束记录状态信息

    if (!program->bind())
    {
        qDebug() << "bind error" << program->log();
    }
    //开启定时器
    time.start();

    //给着色器变量赋值,projextion,view默认构造是生成单位矩阵
    QMatrix4x4 projection, view, model;
    view.translate(QVector3D(0, 0, -5.0f));
    view.rotate(-45, 1, 0, 0);
    projection.perspective(30.0f, (GLfloat)width() / (GLfloat)height(), 0.1f, 100.0f);
    /*
    将此程序绑定到active的OPenGLContext，并使其成为当前着色器程序
    先前绑定的着色器程序将被释放，等价于调用glUseProgram(GLuint id)
    id为着色器程序调用programId()接口返回的GLuint类型的值
    */
    if (!program->bind())
    {
        qDebug() << "bind error" << program->log();
    }
    program->setUniformValue("view", view);
    program->setUniformValue("model", model);
    program->setUniformValue("projection", projection);

    program->setUniformValue("viewPos", QVector3D(0.0f, 0.0f, 3.0f));
    program->setUniformValue("light.position", QVector3D(1.2f, 1.0f, 2.0f));
    program->setUniformValue("light.ambient", QVector3D(0.5f, 0.5f, 0.5f));
    program->setUniformValue("light.diffuse", QVector3D(0.5f, 0.5f, 0.5f));
    program->setUniformValue("light.specular", QVector3D(0.5f, 0.5f, 0.5f));
    program->setUniformValue("heightScale", GLfloat( 0.1));
    program->setUniformValue("material.shininess", GLfloat(65.0f));

    /* 固定属性区域 */
    glEnable(GL_DEPTH_TEST);  //开启深度测试
}
const static unordered_map<int, unordered_map<GameObjType, int>> playerObjModelMap {
        {-1, {{WORKER, 0}, {LIGHT, 1}, {RANGED, 2}, {HEAVY, 3}, {BASE, 4}, {BARRACK, 5}}},
        {1, {{WORKER, 6}, {LIGHT, 7}, {RANGED, 8}, {HEAVY, 9}, {BASE, 10}, {BARRACK, 11}}},
        {0, {{MINERAL, 12}}}
};
void RtsMap::paintGL()
{
    //清理屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
    int idx = 30;
    auto game = RpcClient::GetObservation();
    if (!program->bind())
    {
        qDebug() << "bind error" << program->log();
    }
    QMatrix4x4 mMatrix;
    for (auto& model: pModelVec) {
        mMatrix.setToIdentity();
        auto x = idx % game.w;
        auto y = idx / game.h;
        auto xLoc = 2.0f * static_cast<float>(x) / game.w + 1.0f / game.w - 1.0f;
        auto yLoc = 2.0f * static_cast<float>(y) / game.h + 1.0f / game.h - 1.0f;
        mMatrix.translate(xLoc, yLoc);
        mMatrix.scale(1.0f / game.w);
        program->setUniformValue("model", mMatrix * model->model);
        model->draw(program.get());
        idx++;
    }
    program->release();

    update();
}
void RtsMap::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    if (width != height) {
        resize(std::min(width,height),std::min(width, height));
    }
    update();
}