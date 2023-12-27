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
    float vEdge = 1.0f / state.h/10;
    float hEdge = 1.0f / state.w/10;
    for (auto i = 0; i < state.h; ++i) {
        for (auto j = 0; j < state.w; ++j) {
            float yMin = (float)i * 2.0f / state.h - 1.0f + vEdge;
            float yMax = ((float)i * 2.0f + 2.0f) / state.h - 1.0f - vEdge;
            float xMin = (float)j * 2.0f / state.h - 1.0f + hEdge;
            float xMax = ((float)j * 2.0f + 2.0f) / state.w - 1.0f - hEdge;
            vector<QVector3D> coord {{yMin, xMin, 0}, {yMin, xMax, 0}, {yMax, xMax, 0}, {yMax, xMin, 0}};
            vector<GLuint> idx {2,1,0,3,2,0};
            meshes.emplace_back(make_shared<MapMesh>(coord, idx, color[i * state.w + j]));
        }
    }
    return true;
}
void MapModel::draw(QOpenGLShaderProgram *program) {
    for (auto& mesh: meshes) {
        mesh->draw(program);
    }
}
