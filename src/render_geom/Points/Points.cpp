#include "Points.hpp"

Points::Points(std::vector<std::vector<double>> points, float pointSize = 1.0f)
    : m_Points(points), m_VAO(), m_VBO(), m_VBL()
{
    m_Colors.clear();
    for (int i = 0; i < m_Points.size(); i++)
    {
        m_Colors.push_back(GetColor(points[i]));
        m_Points[i][0] *= 15.5f;
        m_Points[i][1] *= 15.5f;
        m_Points[i][2] *= 15.5f;
    }
    
    m_pointSize = pointSize;
    GenerateVertices();
    m_VBO = VertexBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(float));
    m_VBL.Push<float>(3);
    m_VBL.Push<float>(3);
    m_VAO.AddBuffer(m_VBO, m_VBL, false);
}

void Points::UpdatePoints(std::vector<std::vector<double>>* points)
{
    m_Colors.clear();
    m_Points = (*points);
    for (int i = 0; i < m_Points.size(); i++)
    {
        m_Colors.push_back(GetColor((*points)[i]));
        m_Points[i][0] *= 15.5f;
        m_Points[i][1] *= 15.5f;
        m_Points[i][2] *= 15.5f;
    }
    GenerateVertices();
    m_VBO.UpdateData(&m_Vertices[0], m_Vertices.size() * sizeof(float));
}

void Points::GenerateVertices()
{
    m_Vertices.clear();
    for(int i = 0; i < m_Points.size(); i++)
    {
        m_Vertices.push_back((float)m_Points[i][0]);
        m_Vertices.push_back((float)m_Points[i][1]);
        m_Vertices.push_back((float)m_Points[i][2]);
        m_Vertices.push_back((float)m_Colors[i][0]);
        m_Vertices.push_back((float)m_Colors[i][1]);
        m_Vertices.push_back((float)m_Colors[i][2]);
    }
}

void Points::Draw()
{
    m_VAO.Bind();
    glPointSize(m_pointSize);
    glDrawArrays(GL_POINTS, 0, m_Points.size());
}