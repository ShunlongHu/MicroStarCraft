#ifndef MAP_MODEL_H
#define MAP_MODEL_H
#include "map_mesh.h"
#include <QString>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <qmatrix4x4.h>
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
    void draw(QOpenGLShaderProgram *program);
    std::vector<std::shared_ptr<MapMesh>> meshes;
};
#endif // !MAP_MODEL_H