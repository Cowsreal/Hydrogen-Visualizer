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

class Sphere
{
public:
	Sphere() {};
	Sphere(float radius);
	~Sphere() {};

	void Draw();
private:
	float m_Radius;
	void GenerateSphereVertices();
	void GenerateSphereIndices();
	IndexBuffer m_IBO;
	VertexArray m_VAO;
	VertexBuffer m_VBO;
	VertexBufferLayout m_VBL;
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	unsigned int m_numCols = 25;
	unsigned int m_numRows = 25;
};