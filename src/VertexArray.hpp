#pragma once
#include "VertexBuffer.hpp"

class VertexBufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, bool isInstance);
	void Bind() const;
	void Unbind() const;

private:
	unsigned int m_RendererID;
};