#pragma once
#include "VertexBuffer.hpp"
#include <string>

class VertexBufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, bool isInstance);
	void Delete() const;
	void Bind() const;
	void Unbind() const;

private:
	unsigned int m_RendererID;
};