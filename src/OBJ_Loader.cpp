#include "OBJ_Loader.h"

bool objl::Loader::Load(const std::string &path)
{
    // If the file is not an .obj file return false
    if (path.substr(path.size() - 4, 4) != ".obj") {
        LOGE("the file is not an .obj file");
        return false;
    }
        
    
    // 读取模型文件
    std::ifstream in;
    in.open(path, std::ifstream::in);
    if (in.fail())
    {
        LOGE("Failed to open file: {}", path);
        return false;
    }

    _Vertices.clear();
    _Texture.clear();
    _Normals.clear();
    LoadedMeshes.clear();
    // LoadedVertices.clear();
    // LoadedIndices.clear();

    // 读取模型文件的每一行
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash; // 用于读取无用字符

        if (!line.compare(0, 2, "v ")) // 顶点
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) {
                iss >> v.raw[i];
            }

            _Vertices.push_back(v);
        }
        else if (!line.compare(0, 3, "vt ")) // 纹理坐标
        {
            iss >> trash >> trash;
            Vec2f tex;
            for (int i = 0; i < 2; i++) {
                iss >> tex.raw[i];
            }

            _Texture.push_back(tex);
        }
        else if (!line.compare(0, 3, "vn ")) // 法向量
        {
            iss >> trash >> trash;
            Vec3f nor;
            for (int i = 0; i < 3; i++) {
                iss >> nor.raw[i];
            }

            _Normals.push_back(nor);
        }
        else if (!line.compare(0, 2, "f ")) // 面
        {
            std::vector<Vertex> Vertices;
            Vertex v;
            int idx, tex_idx, n_idx;
            iss >> trash;
            while (iss >> idx >> trash >> tex_idx >> trash >> n_idx)
            {
                idx--;     // .obj 文件中的索引从1开始
                tex_idx--; // .obj 文件中的索引从1开始
                n_idx--;   // .obj 文件中的索引从1开始
                v.Position = _Vertices[idx];
                v.TextureCoordinate = _Texture[tex_idx];
                v.Normal = _Normals[n_idx];
                Vertices.push_back(v);
            }

            LoadedMeshes.push_back(Mesh(Vertices));
        }
    }

    LOGI("model vert# {} face# {} tex_coord# {} normals# {}",
         static_cast<int>(_Vertices.size()),
         static_cast<int>(LoadedMeshes.size()),
         static_cast<int>(_Texture.size()),
         static_cast<int>(_Normals.size()));
    
    return true;
}