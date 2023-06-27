//Za olaksano crtanje objekata. Generise potrebne bafere pri konstrukciji objekta, brise ih pri destrukciji.
#include <iostream>
#include <GL/glew.h> //Da bi koristili OpenGL funkcije za bafere

class Renderable { 
	unsigned int VAO, VBO, EBO;
	unsigned int vCount;
	unsigned int iCount;
public:
	static int rCount;
	Renderable(const float* vertices, const unsigned int verticesSize, const  unsigned int* indices, const int indicesSize);
	~Renderable();
	void Render(unsigned diffuseTexture, unsigned specularTexture);
	void Render();
};