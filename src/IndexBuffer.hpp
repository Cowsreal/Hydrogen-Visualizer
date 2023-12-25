#pragma once

class IndexBuffer
{
public:
	IndexBuffer() : m_RendererID(0) {}; //default constructor
	IndexBuffer(const unsigned int* data, unsigned int count); //constructor

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
private:
	unsigned int m_RendererID = 0;
	unsigned int m_Count;
};