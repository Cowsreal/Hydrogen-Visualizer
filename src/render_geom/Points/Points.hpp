#pragma once

#include "glm/glm.hpp"

#include "../../Renderer.hpp"
#include "../../VertexBuffer.hpp"
#include "../../VertexArray.hpp"
#include "../../VertexBufferLayout.hpp"
#include "../../IndexBuffer.hpp"
#include "../../Shader.hpp"
#include "../../GlobalFunctions.hpp"

#include <vector>

class Points
{
public:
	Points(std::vector<std::vector<double>> points, float pointSize);
	Points(){};
	~Points() {};
	void Draw();
	void GenerateVertices();
	void UpdatePoints(std::vector<std::vector<double>>* points);

private:
	VertexArray m_VAO;
	VertexBuffer m_VBO;
	VertexBufferLayout m_VBL;
	std::vector<float> m_Vertices;
    std::vector<std::vector<double>> m_Points;
    std::vector<std::vector<double>> m_Colors;
	float m_pointSize = 1.0f;
};