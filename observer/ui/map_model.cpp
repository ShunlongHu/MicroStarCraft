#include "map_model.h"
#include <string>
#include <QImage>
#include <QDebug>
#include <QSharedPointer>
#include <QOpenGLTexture>
#include <QMessageBox>
using namespace std;
MapModel::MapModel(const GameState& state)
{
    //Clear();
    if (!loadModel(state))
    {
        qDebug() << "MapModel init failed!";
    }
}

bool MapModel::loadModel(const GameState& state)
{
    vector<QVector4D> color(state.w * state.h, {0.7,0.7,0.7,1.0f});
    for (const auto& [loc, obj]: state.objMap) {
        if (obj.type == TERRAIN) {
            color[loc.x + loc.y * state.w] = {0.0,0.0,0.0,0.0};
        }
    }
    static QMessageBox messageBox;
    for (auto i = 0; i < state.h; ++i) {
        for (auto j = 0; j < state.w; ++j) {
            float yMin = (float)i * 2.0f / state.h - 1.0f;
            float yMax = (float)i * 2.0f + 2.0f / state.h - 1.0f;
            float xMin = (float)j * 2.0f / state.h - 1.0f;
            float xMax = (float)j * 2.0f + 2.0f / state.w - 1.0f;

            messageBox.setText(messageBox.text() + QString::number(yMin) + " " +QString::number(yMax) + " " +QString::number(xMin) + " " +QString::number(xMax) + "\n" );

            vector<QVector3D> coord {{yMin, xMin, 0}, {yMin, xMax, 0}, {yMax, xMax, 0}, {yMax, xMin, 0}};
            vector<GLuint> idx {0,1,2,1,2,3};
            meshes.emplace_back(make_shared<MapMesh>(coord, idx, color[i * state.w + j]));
        }
    }
    messageBox.show();
    return true;
}
void MapModel::draw(QOpenGLShaderProgram *program) {
    for (auto& mesh: meshes) {
        mesh->draw(program);
    }
}
