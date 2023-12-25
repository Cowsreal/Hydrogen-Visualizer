#include "Hopf.hpp"
#include "../Circle/Circle.hpp"
#include <iostream>
#include <cmath>

Hopf::Hopf(const std::vector<std::vector<double>>* points, bool drawAsPoints = false, float pointSize = 1.0f)
    : m_NumFibers(points->size()), m_S2Points(points)
{
    m_DrawAsPoints = drawAsPoints;
    m_PointSize = pointSize;
    m_S2Circles.resize(m_NumFibers);
    m_S3Circles = std::vector<std::vector<std::vector<double>>>(m_NumFibers);
    GenerateVertices();
    GenerateColors();
}

void Hopf::UpdateCircles(const std::vector<std::vector<double>>* points)
{
    m_S2Circles.resize(points->size());
    m_S3Circles = std::vector<std::vector<std::vector<double>>>(points->size());
    m_S2Points = points;
    m_NumFibers = points->size();
    GenerateVertices();
    GenerateColors();
}

void Hopf::InverseHopfMap()
{
    for (int i = 0; i < m_NumFibers; i++)
    {
        std::vector<std::vector<double>> pointsR4;
        double x = (*m_S2Points)[i][0];
        double y = (*m_S2Points)[i][1];
        double z = (*m_S2Points)[i][2];
        
        double phiInc = 0.02;
        for(double phi = 0; phi <= 2 * PI; phi += phiInc)
        {
            double f = 1 / sqrt(2 * (1 + z));
            pointsR4.push_back({
                cos(phi) * (1 + z) * f,
                (sin(phi) * x - cos(phi) * y) * f,
                (cos(phi) * x + sin(phi) * y) * f,
                sin(phi) * (1 + z) * f
            });
        }
        m_S3Circles[i] = pointsR4;
    }
}

void Hopf::StereographicProjection()
{
    for (int i = 0; i < m_NumFibers; i++)
    {
        std::vector<float> pointsR3;
        for (int j = 0; j < m_S3Circles[i].size(); j++)
        {
            float x = m_S3Circles[i][j][0];
            float y = m_S3Circles[i][j][1];
            float z = m_S3Circles[i][j][2];
            float w = m_S3Circles[i][j][3];
            pointsR3.push_back(x / (1 - w) * 400.0);
            pointsR3.push_back(y / (1 - w) * 400.0);
            pointsR3.push_back(z / (1 - w) * 400.0);
        }
        m_S2Circles[i] = Circle(pointsR3, (int)m_S3Circles[i].size() - 1, m_DrawAsPoints, m_PointSize);
    }
}

void Hopf::GenerateVertices()
{
    InverseHopfMap();
    StereographicProjection();
}

void Hopf::GenerateColors()
{
    m_Colors.clear();
    for (int i = 0; i < m_NumFibers; i++)
    {
        m_Colors.push_back(GetColor((*m_S2Points)[i]));
    }
}

void Hopf::Draw(Shader * shader)
{
    for(size_t i = 0; i < m_S2Circles.size(); i++)
    {
        float r = m_Colors[i][0];
        float g = m_Colors[i][1];
        float b = m_Colors[i][2];
        float a = 1.0f;
        shader->SetUniform4f("u_Color", r, g, b, a); //set the uniform
        m_S2Circles[i].Draw();
    }
}

void Hopf::SetDrawAsPoints(bool drawAsPoints)
{
    m_DrawAsPoints = drawAsPoints;
    for (int i = 0; i < m_NumFibers; i++)
    {
        m_S2Circles[i].SetDrawAsPoints(drawAsPoints);
    }
}

void Hopf::ChangePointSize(float pointSize)
{
    m_PointSize = pointSize;
    for (int i = 0; i < m_NumFibers; i++)
    {
        m_S2Circles[i].ChangePointSize(pointSize);
    }
}

Hopf::~Hopf()
{
}
