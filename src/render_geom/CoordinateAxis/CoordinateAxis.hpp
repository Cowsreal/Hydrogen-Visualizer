#pragma once
#include "glm/glm.hpp"

#include "../../Renderer.hpp"
#include "../../VertexBuffer.hpp"
#include "../../VertexArray.hpp"
#include "../../VertexBufferLayout.hpp"

#include <vector>

class Axis 
{
public:
	Axis(float length);
	~Axis();

	void Draw();
private:
    float m_length;
	std::vector<float> m_vertices;
	VertexBuffer m_vb;
	VertexArray m_va;
	VertexBufferLayout m_vbl;
};