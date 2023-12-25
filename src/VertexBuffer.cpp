#include "VertexBuffer.hpp"
#include "Renderer.hpp"
#include <iostream>

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    GLCall(glGenBuffers(1, &m_RendererID));   //generate 1 buffer, store it in buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));  //GL_ARRAY_BUFFER is a type of buffer, bind the current buffer
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); //6 * sizeof(float) is the size of the data we are storing
}

/*
VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));	//delete the buffer
}*/

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));  //GL_ARRAY_BUFFER is a type of buffer, bind the current buffer
}
void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));  //GL_ARRAY_BUFFER is a type of buffer, bind the current buffer
}