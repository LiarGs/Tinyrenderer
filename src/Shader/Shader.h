#pragma once
#include "Texture.h"
#include "Scene.hpp"
#include "rasterizer.h"

Vec3f normal_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &);
Vec3f white_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &);
Vec3f phong_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &);
Vec3f texture_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &);
Vec3f bump_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &);
Vec3f displacement_fragment_shader(const rst::fragment_shader_payload &payload, rst::rasterizer &);

void vertex_shader(rst::vertex_shader_payload &payload, Triangle *t);
