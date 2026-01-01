#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords; // Location 1 was Normal, we skip it

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    // We still need the position to actually place the object in the world
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}