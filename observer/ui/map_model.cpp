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
    const static QVector4D tileColor = {0.7,0.7,0.7,1.0f};
    const static QVector4D terrainColor = {0.,0.,0.,1.0f};
    vector<bool> isTerrain(state.w * state.h, false);

    for (const auto& [loc, obj]: state.objMap) {
        if (obj.type == TERRAIN) {
            isTerrain[loc.x + loc.y * state.w] = true;
        }
    }
    float vEdge = 1.0f / state.h/10;
    float hEdge = 1.0f / state.w/10;
    for (auto i = 0; i < state.h; ++i) {
        for (auto j = 0; j < state.w; ++j) {
            float yMin = (float)i * 2.0f / state.h - 1.0f + vEdge;
            float yMax = ((float)i * 2.0f + 2.0f) / state.h - 1.0f - vEdge;
            float xMin = (float)j * 2.0f / state.w - 1.0f + hEdge;
            float xMax = ((float)j * 2.0f + 2.0f) / state.w - 1.0f - hEdge;
            vector<QVector3D> coord {{xMin,yMin, 0}, {xMin, yMax, 0}, {xMax, yMax, 0}, {xMax, yMin, 0}};
            vector<GLuint> idx {2,1,0,3,2,0};
            if (i == 0) {
                tileXMin.emplace_back(xMin);
                tileXMax.emplace_back(xMax);
            }
            if (j == 0) {
                tileYMin.emplace_back(yMin);
                tileYMax.emplace_back(yMax);
            }

            if (isTerrain[i * state.w + j]) {
                meshes.emplace_back(make_shared<MapMesh>(coord, idx, terrainColor));
                continue;
            }
            meshes.emplace_back(make_shared<MapMesh>(coord, idx, tileColor));
        }
    }
    return true;
}
void MapModel::draw(QOpenGLShaderProgram *program, float mouseX, float mouseY, int w) {
    int tileIdx = -1;
    int tileX = -1;
    int tileY = -1;
    for (int i = 0; i < tileYMin.size(); ++i) {
        if (tileYMin[i] <= mouseY && tileYMax[i] >= mouseY) {
            tileY = i;
        }
    }
    for (int i = 0; i < verticalSpliterMin.size(); ++i) {
        if (mouseY * verticalSpliterMin[i].x() + verticalSpliterMin[i].y() <= mouseX &&
            mouseY * verticalSpliterMax[i].x() + verticalSpliterMax[i].y() >= mouseX) {
            tileX = i;
        }
    }
    if (tileX != -1 && tileY != -1) {
        tileIdx = tileX + tileY * w;
    }
    if (lastTileIdx != -1) {
        meshes[lastTileIdx]->color = meshes[lastTileIdx]->origColor;
    }
    for (int i = 0; i < meshes.size(); ++i) {
        if (i == tileIdx) {
            meshes[i]->color = {0,1,0,1};
        }
        meshes[i]->draw(program);
    }
    lastTileIdx = tileIdx;
}
