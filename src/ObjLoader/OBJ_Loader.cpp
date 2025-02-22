#include "OBJ_Loader.h"

bool objl::Loader::Load(const std::string &path)
{
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

std::vector<Triangle> objl::LoadTriangleList(const Loader &model)
{
    std::vector<Triangle> TriangleList;
    for (const auto &mesh : model.LoadedMeshes)
    {
        for (int i = 0; i < mesh.Vertices.size(); i += 3)
        {
            Triangle t;
            for (int j = 0; j < 3; j++)
            {
                t.setVertex(j, Vec3f{mesh.Vertices[i + j].Position.x, mesh.Vertices[i + j].Position.y, mesh.Vertices[i + j].Position.z});
                t.setNormal(j, Vec3f{mesh.Vertices[i + j].Normal.x, mesh.Vertices[i + j].Normal.y, mesh.Vertices[i + j].Normal.z});
                t.setTexCoord(j, Vec2f{mesh.Vertices[i + j].TextureCoordinate.x, mesh.Vertices[i + j].TextureCoordinate.y});
                t.setColor(j, {148.f, 121.f, 92.f});
            }
            TriangleList.emplace_back(t);
        }
    }
    return TriangleList;
}