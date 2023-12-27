#ifndef LOADENGINE_H
#define LOADENGINE_H


#include <QOpenGLTexture>
#include <QSharedPointer>
#include <QOpenGLWidget>
#include <QString>
#include <QOpenGLFunctions_3_3_Core>
#include <QTime>
#include "map_model.h"
#include "model.h"
class QOpenGLTexture;
class QOpenGLShaderProgram;

class RtsMap : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
Q_OBJECT
public:
    explicit RtsMap(QWidget *parent = 0);
    ~RtsMap();
protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int width, int height) override;
private:
    //着色器程序
    std::shared_ptr<QOpenGLShaderProgram> program;
    std::shared_ptr<QOpenGLShaderProgram> colorProgram;
    std::vector<std::shared_ptr<Model>> pModelVec;
    std::shared_ptr<MapModel> mModel;
};

#endif // !LOADMODEL_H