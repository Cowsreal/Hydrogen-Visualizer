#include "VertexArray.hpp"
#include "Renderer.hpp"
#include "VertexBufferLayout.hpp"
#include <iostream>

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID)); //generate a vertex array object)
}

VertexArray::~VertexArray() 
{
	
}

void VertexArray::Delete() const
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID)); //delete the vertex array object
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, bool isInstance)
{
	Bind(); //bind the vertex array
	vb.Bind(); //bind the vertex buffer
	const auto& elements = layout.GetElements(); //get the elements of the layout
	unsigned int offset = 0; //offset of the vertex
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glVertexAttribPointer(i, element.count , element.type, element.normalized, layout.GetStride(), (const void*)offset)); //0 is the index of the attribute, 2 is the size of the attribute, GL_FLOAT is the type of the attribute, GL_FALSE is whether the data should be normalized, sizeof(float) * 2 is the size of the vertex, 0 is the offset of the 
		GLCall(glEnableVertexAttribArray(i));   //enable the vertex attribute
		if (isInstance)
		{
			GLCall(glVertexAttribDivisor(i, 1));
		}
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	vb.Unbind(); //unbind the vertex buffer
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}
void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}