#ifndef MODEL_H
#define MODEL_H
#include "mesh.h"
#include <QString>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <qmatrix4x4.h>

struct aiMesh;
struct aiScene;
class QString;
class QOpenGLTexture;
class Model
{
public:
    Model() = default;
    explicit Model(const QString& path);
    ~Model();
    //加载模型
    bool loadModel(const QString &path);


    void processNode(aiNode *node, const aiScene *scene);
    Mesh *processMesh(aiMesh *mesh, const aiScene *scene);

    /////////////////////////////////////
    bool initFromScene(const aiScene *pscene, const QString &path);
    Mesh *initMesh(const aiScene *pscene, const aiMesh *pmesh);
    /////////////////////////////////////


    //QSharedPointer<QOpenGLTexture> textureFile();
    std::vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, const QString &typeName);

    void draw(QOpenGLShaderProgram *program);
    //目录
    QString directory;
    std::vector<Mesh *> meshes;
    std::vector<Texture> textures_loaded;

    // translation offset
    float vMax[3] = {FLT_MIN, FLT_MIN, FLT_MIN};
    float vMin[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
    float vAvg[3] = {0, 0, 0};
    uint32_t vCount = 1;
    QMatrix4x4 model;
};
#endif // !MODEL_H