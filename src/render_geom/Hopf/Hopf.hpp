#pragma once

#include "glm/glm.hpp"

#include "../../Renderer.hpp"
#include "../../VertexBuffer.hpp"
#include "../../VertexArray.hpp"
#include "../../VertexBufferLayout.hpp"
#include "../../IndexBuffer.hpp"
#include "../../Shader.hpp"
#include "../../GlobalFunctions.hpp"

#include "../Circle/Circle.hpp"

#include <vector>

class Hopf
{
public:
    Hopf(const std::vector<std::vector<double>>* points, bool drawAsPoints, float pointSize);
    ~Hopf();

    void UpdateCircles(const std::vector<std::vector<double>>* points);
    void InverseHopfMap();
    void StereographicProjection();
    void GenerateVertices();
    void GenerateColors();
    void SetDrawAsPoints(bool drawAsPoints);
    void Draw(Shader* shader);
    void ChangePointSize(float pointSize);

private:
    unsigned int m_NumFibers;
    bool m_DrawAsPoints;
    float m_PointSize;
    std::vector<std::vector<std::vector<double>>> m_S3Circles;
    std::vector<Circle> m_S2Circles;
    const std::vector<std::vector<double>>* m_S2Points;
    std::vector<std::vector<double>> m_Colors;
    unsigned int m_numCols = 50;
};