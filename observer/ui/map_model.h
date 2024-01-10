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

struct MouseAction {
    Coord mouseClick {-1,-1};
    Coord mouseRightClick {-1,-1};
};

class MapModel
{
public:
    MapModel() = default;
    explicit MapModel(const GameState& state);
    //加载模型
    bool loadModel(const GameState& state);
    void refreshModel(const GameState& state);
    MouseAction draw(QOpenGLShaderProgram *program, float mouseX, float mouseY, int w, float mouseClickX, float mouseClickY, float mouseRightClickX, float mouseRightClickY);
    std::vector<std::shared_ptr<MapMesh>> meshes;
    std::vector<float> tileXMin;
    std::vector<float> tileXMax;
    std::vector<float> tileYMin;
    std::vector<float> tileYMax;
    std::vector<QVector2D> verticalSpliterMin; // x = ky + b
    std::vector<QVector2D> verticalSpliterMax; // x = ky + b
    int prevSelTileIdx = -1;
    int prevClickTileIdx = -1;
    int prevRightClickTileIdx = -1;

    constexpr const static QVector4D TILE_COLOR {0.7,0.7,0.7,1.0f};
    constexpr const static QVector4D TERRAIN_COLOR {0.,0.,0.,1.0f};
private:
    GameState lastState;
};
#endif // !MAP_MODEL_H