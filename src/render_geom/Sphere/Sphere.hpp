#pragma once

#include "glm/glm.hpp"

#include "../../Renderer.hpp"
#include "../../VertexBuffer.hpp"
#include "../../VertexArray.hpp"
#include "../../VertexBufferLayout.hpp"
#include "../../IndexBuffer.hpp"
#include "../../Shader.hpp"

#include <vector>
#define PI 3.14159265358979323846

struct SphereInstance {
	float position[3];
	float color[4];
	float radius;
};

typedef struct aliasvrt_s {
	GLshort     st[2];
	GLshort     normal[3];
	GLushort    vertex[3];
} aliasvrt_t;

class Sphere
{
public:
	Sphere() {};
	Sphere(float radius);
	Sphere(const std::vector<SphereInstance>& instances);
	~Sphere() {};

	void Draw();
private:
	bool m_Instanced;
	float m_Radius;
	void GenerateSphereVertices();
	void GenerateSphereIndices();
	IndexBuffer m_IBO;
	VertexArray m_VAO;
	VertexBuffer m_VBO;
	VertexBufferLayout m_VBL;
	VertexBuffer m_InstanceVBO;
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::size_t m_numInstances;
	unsigned int m_numCols = 25;
	unsigned int m_numRows = 25;
};