#ifndef MAP_MODEL_H
#define MAP_MODEL_H
#include "map_mesh.h"
#include <QString>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <qmatrix4x4.h>
#include <array>
#include "game_types.h"

struct aiMesh;
struct aiScene;
class QString;
class QOpenGLTexture;
class MapModel
{
public:
    MapModel() = default;
    explicit MapModel(const GameState& state);
    //加载模型
    bool loadModel(const GameState& state);
    void draw(QOpenGLShaderProgram *program, float mouseX, float mouseY, int w);
    std::vector<std::shared_ptr<MapMesh>> meshes;
    std::vector<float> tileXMin;
    std::vector<float> tileXMax;
    std::vector<float> tileYMin;
    std::vector<float> tileYMax;
    std::vector<QVector2D> verticalSpliterMin; // x = ky + b
    std::vector<QVector2D> verticalSpliterMax; // x = ky + b
    int lastTileIdx = -1;
};
#endif // !MAP_MODEL_H