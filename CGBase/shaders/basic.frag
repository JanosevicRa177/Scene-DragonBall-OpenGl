#version 330 core

out vec4 FragColor;
in vec3 vCol;
in vec2 TexCoord;

uniform vec3 uCol;
uniform sampler2D ourTexture;


void main() {
	FragColor = texture(ourTexture, TexCoord) * vec4((vCol + uCol), 1.0f);
}