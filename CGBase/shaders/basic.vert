# version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aCol;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uViewport;
out vec2 TexCoord;
out vec3 vCol;

void main() {
	vCol = aCol;
	gl_Position = uViewport * uProjection * uView * uModel * vec4(aPos, 1.0f);
	TexCoord = aTexCoord;
}