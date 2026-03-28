#version 330 core
in vec2 v_tex_coord;
out vec4 FragColor;

uniform int u_use_texture;
uniform vec4 u_color;
uniform vec4 u_tint;
uniform sampler2D u_texture;

void main() {
    if (u_use_texture != 0) {
        FragColor = texture(u_texture, v_tex_coord) * u_tint;
    } else {
        FragColor = u_color;
    }
}
