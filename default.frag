#version 410 core
out vec4 FragColor;

in vec2 TexCoords; // We don't need Normal or FragPos anymore!

uniform sampler2D texture_diffuse1;

void main() {
    // Just grab the color and send it straight to the screen
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}