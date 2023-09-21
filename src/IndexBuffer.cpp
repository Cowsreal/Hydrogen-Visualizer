#include "IndexBuffer.hpp"
#include "Renderer.hpp"
#include <iostream>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_Count(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint)); //make sure the size of the data is the same as the size of the buffer
    GLCall(glGenBuffers(1, &m_RendererID));   //generate 1 buffer, store it in buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));  //GL_ARRAY_BUFFER is a type of buffer, bind the current buffer
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW)); //6 * sizeof(float) is the size of the data we are storing
}
/*
IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));	//delete the buffer
}*/

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));  //GL_ARRAY_BUFFER is a type of buffer, bind the current buffer
}
void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));  //GL_ARRAY_BUFFER is a type of buffer, bind the current buffer
}