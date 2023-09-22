#include "CoordinateAxis.hpp"

Axis::Axis(float length)
	: m_length(length), m_vertices({
    // X-axis
    0.0f, 0.0f, 0.0f, // Start point
    m_length, 0.0f, 0.0f, // End point

    // Y-axis
    0.0f, 0.0f, 0.0f, // Start point
    0.0f, m_length, 0.0f, // End point

    // Z-axis
    0.0f, 0.0f, 0.0f, // Start point
    0.0f, 0.0f, m_length  // End point
        }), m_vb(&m_vertices[0], m_vertices.size() * sizeof(float)), m_va(), m_vbl()
{
    m_vbl.Push<float>(3);
    m_va.AddBuffer(m_vb, m_vbl, false);
}

Axis::~Axis() {
}

void Axis::Draw() {
    m_va.Bind();
	m_vb.Bind();
	GLCall(glDrawArrays(GL_LINES, 0, 6));
}
