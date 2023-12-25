#pragma once
#include <vector>
#include <GL/glew.h>
#include "Renderer.hpp"

class VertexBufferElement
{
public:
	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:			return sizeof(GLfloat);
		case GL_UNSIGNED_INT:	return sizeof(GLuint);
		case GL_UNSIGNED_BYTE:	return sizeof(GLubyte);
		}
		ASSERT(false);
		return 0;
	}

	VertexBufferElement(unsigned int t, unsigned int c, bool n) :
		count(c), type(t), normalized(n)
	{
	}
	unsigned int count;
	unsigned int type;
	unsigned char normalized; 
};

class VertexBufferLayout
{
public:
	VertexBufferLayout()
		: m_Stride(0)
	{

	}
	~VertexBufferLayout()
	{

	}

	template<typename T>
	void Push(unsigned int count)
	{
		ASSERT(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back(VertexBufferElement({ GL_FLOAT, count, GL_FALSE }));
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int  count)
	{
		m_Elements.push_back(VertexBufferElement({ GL_UNSIGNED_INT, count, GL_FALSE }));
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int  count)
	{
		m_Elements.push_back(VertexBufferElement({ GL_UNSIGNED_BYTE, count, GL_TRUE }));
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline unsigned int GetStride() const { return m_Stride; }
	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
};