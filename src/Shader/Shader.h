﻿#pragma once
#include "Texture.h"
#include "Scene.h"

struct vertex_shader_payload
{
    Triangle* triangle;
    Matrix4f model;
    Matrix4f view;
    Matrix4f projection;
    Matrix4f mvp;
    Matrix4f inv_trans;
};

struct fragment_shader_payload
{
    Vec3f view_pos; 
    Vec3f color;
    Vec3f normal;
    Vec3f amb_light_intensity;
    Material material;
    Camera camera;     
    Vec2f tex_coords; 

    Texture *texture;
    std::vector<Light> lights;

    // 构造函数
    fragment_shader_payload()
        : texture(nullptr) {}

    fragment_shader_payload(const Vec3f &col, const Vec3f &nor, const Vec2f &tc, Texture *tex)
        : color(col), normal(nor), tex_coords(tc), texture(tex) {}
};

Vec3f normal_fragment_shader(const fragment_shader_payload &payload);
Vec3f white_fragment_shader(const fragment_shader_payload &payload);
Vec3f phong_fragment_shader(const fragment_shader_payload &payload);
Vec3f texture_fragment_shader(const fragment_shader_payload &payload);
Vec3f bump_fragment_shader(const fragment_shader_payload &payload);
Vec3f displacement_fragment_shader(const fragment_shader_payload &payload);


void vertex_shader(vertex_shader_payload &payload);