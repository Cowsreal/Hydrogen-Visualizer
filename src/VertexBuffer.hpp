#pragma once

class VertexBuffer
{
public:
	VertexBuffer() : m_RendererID(0) {}; //Default constructor
	VertexBuffer(const void* data, unsigned int size); //constructor

	
	void Bind() const;
	void Unbind() const;
private:
	unsigned int m_RendererID = 0;
};