#include <iostream>
#include "Renderer.hpp"

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR); //while there is an error
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) //while there is an error
	{
		std::cout << "[OpenGL Error] (" << error << ") at " << function << ", " << file << ", line: " << line << std::endl; //print the error
		return false;
	}
	return true;
}

void Renderer::Clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

