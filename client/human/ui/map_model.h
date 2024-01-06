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
    explicit MapModel(const std::string& state);
    //加载模型
    bool loadModel(const std::string& state);
    void refreshModel(const std::string& state);
    void draw(QOpenGLShaderProgram *program, float mouseX, float mouseY, int w);
    std::vector<std::shared_ptr<MapMesh>> meshes;
    std::vector<float> tileXMin;
    std::vector<float> tileXMax;
    std::vector<float> tileYMin;
    std::vector<float> tileYMax;
    std::vector<QVector2D> verticalSpliterMin; // x = ky + b
    std::vector<QVector2D> verticalSpliterMax; // x = ky + b
    int prevSelTileIdx = -1;

    constexpr const static QVector4D TILE_COLOR {0.7,0.7,0.7,1.0f};
    constexpr const static QVector4D TERRAIN_COLOR {0.,0.,0.,1.0f};
private:
    std::string lastState;
};
#endif // !MAP_MODEL_H