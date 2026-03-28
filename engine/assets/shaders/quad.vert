#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_tex_coord;

uniform mat4 u_model;
uniform mat4 u_view_projection;
uniform int u_use_sprite_uvs;
uniform vec2 u_sprite_uv0;
uniform vec2 u_sprite_uv1;
uniform vec2 u_sprite_uv2;
uniform vec2 u_sprite_uv3;

out vec2 v_tex_coord;

void main() {
    gl_Position = u_view_projection * u_model * vec4(a_position, 1.0);

    if (u_use_sprite_uvs != 0) {
        if (gl_VertexID == 0) {
            v_tex_coord = u_sprite_uv0;
        } else if (gl_VertexID == 1) {
            v_tex_coord = u_sprite_uv1;
        } else if (gl_VertexID == 2) {
            v_tex_coord = u_sprite_uv2;
        } else {
            v_tex_coord = u_sprite_uv3;
        }
    } else {
        v_tex_coord = a_tex_coord;
    }
}
