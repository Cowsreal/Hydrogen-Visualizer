#pragma once

#include <glm/glm.hpp>

class VertexBuffer
{
public:
	VertexBuffer() : m_RendererID(0) {}; //Default constructor
	VertexBuffer(const void* data, unsigned int size); //constructor

	void UpdateData(const void* data, unsigned int size);
	void Bind() const;
	void Unbind() const;
	void Delete() const;
private:
	unsigned int m_RendererID = 0;
};