#include "map_model.h"
#include <string>
#include <QImage>
#include <QDebug>
#include <QSharedPointer>
#include <QOpenGLTexture>
#include <QMessageBox>
#include <cmath>
using namespace std;
MapModel::MapModel(const string& state)
{
    //Clear();
    if (!loadModel(state))
    {
        qDebug() << "MapModel init failed!";
    }
}

bool MapModel::loadModel(const string& state)
{
    auto size = state.size() / OBSERVATION_PLANE_NUM;
    int w = 0;
    for (int i = 0; i < size; ++i) {
        if (i * i == size) {
            w = i;
            break;
        }
    }
    float vEdge = 1.0f / w/10;
    float hEdge = 1.0f / w/10;
    for (auto i = 0; i < w; ++i) {
        for (auto j = 0; j < w; ++j) {
            float yMin = (float)i * 2.0f / w - 1.0f + vEdge;
            float yMax = ((float)i * 2.0f + 2.0f) / w - 1.0f - vEdge;
            float xMin = (float)j * 2.0f / w - 1.0f + hEdge;
            float xMax = ((float)j * 2.0f + 2.0f) / w - 1.0f - hEdge;
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

            if (state[(OBJ_TYPE + TERRAIN) * w * w + i * w + j]) {
                meshes.emplace_back(make_shared<MapMesh>(coord, idx, TERRAIN_COLOR));
                continue;
            }
            meshes.emplace_back(make_shared<MapMesh>(coord, idx, TILE_COLOR));
        }
    }
    lastState = state;
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
    if (prevSelTileIdx != -1) {
        meshes[prevSelTileIdx]->color = meshes[prevSelTileIdx]->origColor;
    }
    for (int i = 0; i < meshes.size(); ++i) {
        if (i == tileIdx) {
            meshes[i]->color = {1,1,1,1};
        }
        meshes[i]->draw(program);
    }
    prevSelTileIdx = tileIdx;
}

void MapModel::refreshModel(const string &state) {
    if (lastState == state) {
        return;
    }
    auto size = state.size() / OBSERVATION_PLANE_NUM;
    int w = 0;
    for (int i = 0; i < size; ++i) {
        if (i * i == size) {
            w = i;
            break;
        }
    }
    for (auto i = 0; i < w; ++i) {
        for (auto j = 0; j < w; ++j) {
            vector<GLuint> idx {2,1,0,3,2,0};
            if (state[(OBJ_TYPE + TERRAIN) * w * w + i * w + j]) {
                meshes[i * w + j]->origColor = TERRAIN_COLOR;
                meshes[i * w + j]->color = TERRAIN_COLOR;
                continue;
            }
            meshes[i * w + j]->origColor = TILE_COLOR;
            meshes[i * w + j]->color = TILE_COLOR;
        }
    }
    lastState = state;
}
