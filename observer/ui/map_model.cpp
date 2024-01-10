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

    vector<bool> isTerrain(state.w * state.h, false);

    for (const auto& [_, obj]: state.objMap) {
        if (obj.type == TERRAIN) {
            isTerrain[obj.coord.x + obj.coord.y * state.w] = true;
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
                meshes.emplace_back(make_shared<MapMesh>(coord, idx, TERRAIN_COLOR));
                continue;
            }
            meshes.emplace_back(make_shared<MapMesh>(coord, idx, TILE_COLOR));
        }
    }
    lastState = state;
    return true;
}
MouseAction MapModel::draw(QOpenGLShaderProgram *program, float mouseX, float mouseY, int w, float mouseClickX, float mouseClickY, float mouseRightClickX, float mouseRightClickY) {
    int tileIdx = -1;
    int tileX = -1;
    int tileY = -1;
    int tileClickIdx = -1;
    int tileClickX = -1;
    int tileClickY = -1;
    int tileRightClickIdx = -1;
    int tileRightClickX = -1;
    int tileRightClickY = -1;
    for (int i = 0; i < tileYMin.size(); ++i) {
        if (tileYMin[i] <= mouseY && tileYMax[i] >= mouseY) {
            tileY = i;
        }
        if (tileYMin[i] <= mouseClickY && tileYMax[i] >= mouseClickY) {
            tileClickY = i;
        }
        if (tileYMin[i] <= mouseRightClickY && tileYMax[i] >= mouseRightClickY) {
            tileRightClickY = i;
        }
    }
    for (int i = 0; i < verticalSpliterMin.size(); ++i) {
        if (mouseY * verticalSpliterMin[i].x() + verticalSpliterMin[i].y() <= mouseX &&
            mouseY * verticalSpliterMax[i].x() + verticalSpliterMax[i].y() >= mouseX) {
            tileX = i;
        }
        if (mouseClickY * verticalSpliterMin[i].x() + verticalSpliterMin[i].y() <= mouseClickX &&
            mouseClickY * verticalSpliterMax[i].x() + verticalSpliterMax[i].y() >= mouseClickX) {
            tileClickX = i;
        }
        if (mouseRightClickY * verticalSpliterMin[i].x() + verticalSpliterMin[i].y() <= mouseRightClickX &&
            mouseRightClickY * verticalSpliterMax[i].x() + verticalSpliterMax[i].y() >= mouseRightClickX) {
            tileRightClickX = i;
        }
    }
    if (tileX != -1 && tileY != -1) {
        tileIdx = tileX + tileY * w;
    }
    if (tileClickX != -1 && tileClickY != -1) {
        tileClickIdx = tileClickX + tileClickY * w;
    }
    if (tileRightClickX != -1 && tileRightClickY != -1) {
        tileRightClickIdx = tileRightClickX + tileRightClickY * w;
    }
    if (prevSelTileIdx != -1) {
        meshes[prevSelTileIdx]->color = meshes[prevSelTileIdx]->origColor;
    }
    prevSelTileIdx = tileIdx;

    if (prevClickTileIdx != -1) {
        meshes[prevClickTileIdx]->color = meshes[prevClickTileIdx]->origColor;
    }
    prevClickTileIdx = tileClickIdx;

    if (prevRightClickTileIdx != -1) {
        meshes[prevRightClickTileIdx]->color = meshes[prevRightClickTileIdx]->origColor;
    }
    prevRightClickTileIdx = tileRightClickIdx;

    for (int i = 0; i < meshes.size(); ++i) {
        if (i == tileClickIdx) {
            meshes[i]->color = {1,0,1,1};
        } else if (i == tileRightClickIdx) {
            meshes[i]->color = {0.5,0,0.5,1};
        } else if (i == tileIdx) {
            meshes[i]->color = {1,1,1,1};
        }
        meshes[i]->draw(program);
    }
    MouseAction retVal;
    if (tileClickIdx != -1){
        retVal.mouseClick = {tileClickY, tileClickX};
    }
    if (tileClickIdx != -1){
        retVal.mouseRightClick = {tileRightClickY, tileRightClickX};
    }
    return retVal;
}

void MapModel::refreshModel(const GameState &state) {
    if (state.time != 0) {
        return;
    }
    if (lastState == state) {
        return;
    }
    vector<bool> isTerrain(state.w * state.h, false);
    for (const auto& [_, obj]: state.objMap) {
        if (obj.type == TERRAIN) {
            isTerrain[obj.coord.x + obj.coord.y * state.w] = true;
        }
    }
    for (auto i = 0; i < state.h; ++i) {
        for (auto j = 0; j < state.w; ++j) {
            vector<GLuint> idx {2,1,0,3,2,0};
            if (isTerrain[i * state.w + j]) {
                meshes[i * state.w + j]->origColor = TERRAIN_COLOR;
                meshes[i * state.w + j]->color = TERRAIN_COLOR;
                continue;
            }
            meshes[i * state.w + j]->origColor = TILE_COLOR;
            meshes[i * state.w + j]->color = TILE_COLOR;
        }
    }
    lastState = state;
}
