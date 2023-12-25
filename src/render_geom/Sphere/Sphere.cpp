#include "Sphere.hpp"
#include <iostream>

Sphere::Sphere(float radius)
	: m_Radius(radius), m_VAO(), m_VBO(), m_IBO(), m_VBL(), m_Vertices(), m_Indices(), m_Instanced(false)
{
	GenerateSphereVertices();
	GenerateSphereIndices();
	m_IBO = IndexBuffer(&m_Indices[0], m_Indices.size());
	m_VBO = VertexBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(float));
	
	m_VBL.Push<float>(3);
	m_VAO.AddBuffer(m_VBO, m_VBL, false);
}

Sphere::Sphere(const std::vector<SphereInstance>& instances)
	: m_Radius(1.0f), m_VAO(), m_VBO(), m_IBO(), m_VBL(), m_Vertices(), m_Indices(), m_Instanced(true)
{
	GenerateSphereVertices();
	GenerateSphereIndices();
	m_IBO = IndexBuffer(&m_Indices[0], m_Indices.size());
	m_VBO = VertexBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(float));
	
	m_VBL.Push<float>(3);
	m_VAO.Bind();
	m_VAO.AddBuffer(m_VBO, m_VBL, false);
	
	m_InstanceVBO = VertexBuffer(&instances[0], instances.size() * sizeof(SphereInstance));
	m_InstanceVBO.Bind();

	GLsizei stride = sizeof(SphereInstance);

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(SphereInstance, position)));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(SphereInstance, color)));
	GLCall(glEnableVertexAttribArray(3));
	GLCall(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(SphereInstance, radius)));
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);

	m_numInstances = instances.size();

	m_VAO.Unbind();
	m_InstanceVBO.Unbind();
}

void Sphere::GenerateSphereVertices()
{
	//We'll define the surface coordinates UV as phi and theta, then create a function to map them to XYZ coordinates
	//phi is in the range [0, 2pi], theta is in the range [0, pi]
	float dphi = 2 * PI / m_numCols;		//Normalized phi
	float dtheta = PI / (m_numRows - 1);		//Normalized theta
	float phi = 0;
	float theta = 0;

	for(int i = 0; i < m_numRows; i++)
	{
		theta = i * dtheta;
		for(int j = 0; j < m_numCols; j++)
		{
			phi = j * dphi;
			float x = m_Radius * sin(theta) * cos(phi);
			float y = m_Radius * sin(theta) * sin(phi);
			float z = m_Radius * cos(theta);
			m_Vertices.push_back(x);		//x
			m_Vertices.push_back(y);		//y
			m_Vertices.push_back(z);		//z
		}
	}
}

void Sphere::GenerateSphereIndices()
{
	for (int i = 0; i < m_numRows - 1; i++)
	{
		for (int j = 0; j < m_numCols; j++)
		{
			int currentVertex = i * m_numCols + j;
			int nextVertex = currentVertex + m_numCols;

			// Add indices for the current vertex and the next vertex
			m_Indices.push_back(currentVertex);
			m_Indices.push_back(nextVertex);

			// If we're at the last column, also add an index for the first vertex
			if (j == m_numCols - 1)
			{
				m_Indices.push_back(i * m_numCols);
				m_Indices.push_back((i + 1) * m_numCols);
			}
		}
	}
}

void Sphere::Draw()
{
	m_VAO.Bind();
	m_IBO.Bind();
	if (m_Instanced)
	{
		glDrawElementsInstanced(GL_TRIANGLE_STRIP, m_Indices.size(), GL_UNSIGNED_INT, 0, m_numInstances);
	}
	else
	{
		glDrawElements(GL_TRIANGLE_STRIP, m_Indices.size(), GL_UNSIGNED_INT, 0);
	}
}

