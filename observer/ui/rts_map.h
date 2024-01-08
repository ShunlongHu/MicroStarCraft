#ifndef LOADENGINE_H
#define LOADENGINE_H


#include <QOpenGLTexture>
#include <QSharedPointer>
#include <QOpenGLWidget>
#include <QString>
#include <QOpenGLFunctions_3_3_Core>
#include <QTime>
#include "map_model.h"
#include "game_types.h"
#include "model.h"
#include "text_mesh.h"
#include "line.h"
class QOpenGLTexture;
class QOpenGLShaderProgram;

class RtsMap : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
Q_OBJECT
public:
    explicit RtsMap(QWidget *parent = 0);
    void initMap(const GameState& gameState);
    ~RtsMap();
protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int width, int height) override;
private:
    //着色器程序
    std::shared_ptr<QOpenGLShaderProgram> program;
    std::shared_ptr<QOpenGLShaderProgram> colorProgram;
    std::shared_ptr<QOpenGLShaderProgram> lineProgram;
    std::shared_ptr<QOpenGLShaderProgram> textProgram;
    std::vector<std::shared_ptr<Model>> pModelVec;
    std::shared_ptr<MapModel> mModel;
    std::shared_ptr<TextMesh> tMesh;
    std::shared_ptr<Line> line;
    QMatrix4x4 projection, view, model;
    const static std::unordered_map<int, const std::unordered_map<GameObjType, int>> MODEL_MAP;
};

#endif // !LOADMODEL_H