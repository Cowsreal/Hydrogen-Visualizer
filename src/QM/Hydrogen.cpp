#include "Hydrogen.hpp"
#include <iostream>

Hydrogen::Hydrogen(unsigned int n, unsigned int l, int m, unsigned int dims)
	: m_n(n), m_l(l), m_m(m)
{
	m_a0 = 5.29177210903e-11;
	m_numPointsPerDimension = dims;
	m_physicalDimension = 5.0 * m_a0; // Or choose any other constant
	m_stepSize = m_physicalDimension / (m_numPointsPerDimension - 1);
	GenerateGrid();
}

Hydrogen::~Hydrogen()
{
	delete m_grid;						//The std::vector destructor should take care of things correctly for me...(?)
}

Hydrogen::Hydrogen(const Hydrogen& rhs)
	: m_n(rhs.m_n), m_l(rhs.m_l), m_m(rhs.m_m), m_a0(rhs.m_a0), m_numPointsPerDimension(rhs.m_numPointsPerDimension), m_physicalDimension(rhs.m_physicalDimension), m_stepSize(rhs.m_stepSize)
{
	m_grid = new std::vector<std::vector<std::vector<double>>>(*rhs.m_grid);
}


Hydrogen& Hydrogen::operator=(const Hydrogen& rhs)
{
	if (this == &rhs)		//Self assignment
	{
		return *this;
	}
	delete m_grid;

	m_n = rhs.m_n;													//Copy all the data members
	m_l = rhs.m_l;
	m_m = rhs.m_m;
	m_a0 = rhs.m_a0;
	m_numPointsPerDimension = rhs.m_numPointsPerDimension;
	m_physicalDimension = rhs.m_physicalDimension;
	m_stepSize = rhs.m_stepSize;
	m_grid = new std::vector<std::vector<std::vector<double>>>(*rhs.m_grid);

	return *this;
}

double Hydrogen::GetPDFAtPosition(double x, double y, double z)
{
	double r = sqrt(x * x + y * y + z * z);
	double theta = acos(z / r);
	double phi = atan2(y, x);
	return pow(RadialComponent(r), 2) * pow(std::norm(SphericalHarmonic(theta, phi)), 2);
}

void Hydrogen::GenerateGrid()
{
	m_grid =
		new std::vector<std::vector<std::vector<double>>>(m_numPointsPerDimension, 
			std::vector<std::vector<double>>(m_numPointsPerDimension, 
				std::vector<double>(m_numPointsPerDimension, 0.0)));
	int center = (m_numPointsPerDimension - 1) / 2;

	// Adjust the position calculation so that the center is at (0, 0, 0)
	for (int i = 0; i < m_numPointsPerDimension; i++)
	{
		for (int j = 0; j < m_numPointsPerDimension; j++)
		{
			for (int k = 0; k < m_numPointsPerDimension; k++)
			{
				double x = (i - center) * m_stepSize;
				double y = (j - center) * m_stepSize;
				double z = (k - center) * m_stepSize;

				// Skip center to avoid division by zero in GetPDFAtPosition(0, 0, 0)
				if (x == 0 && y == 0 && z == 0) {
					(*m_grid)[i][j][k] = 0;
					continue;
				}
				(*m_grid)[i][j][k] = GetPDFAtPosition(x, y, z);			//x,y,z are physical coordinates
			}
		}
	}

	NormalizeWaveFunction();
}

void Hydrogen::NormalizeWaveFunction()
{
	double sumOfSquares = 0.0;

	// Calculate the sum of squares over the entire grid
	for (unsigned int i = 0; i < m_numPointsPerDimension; i++)
	{
		for (unsigned int j = 0; j < m_numPointsPerDimension; j++)
		{
			for (unsigned int k = 0; k < m_numPointsPerDimension; k++)
			{
				sumOfSquares += pow((*m_grid)[i][j][k], 2);
			}
		}
	}

	// Calculate the normalization constant
	double normalizationConstant = 1.0 / sqrt(sumOfSquares);

	// Normalize the wave function
	for (unsigned int i = 0; i < m_numPointsPerDimension; i++)
	{
		for (unsigned int j = 0; j < m_numPointsPerDimension; j++)
		{
			for (unsigned int k = 0; k < m_numPointsPerDimension; k++)
			{
				(*m_grid)[i][j][k] = (*m_grid)[i][j][k] * normalizationConstant;
			}
		}
	}
	//Test if normalization worked correctly
	/*
	double sum = 0;
	for (unsigned int i = 0; i < m_numPointsPerDimension; i++)
	{
		for (unsigned int j = 0; j < m_numPointsPerDimension; j++)
		{
			for (unsigned int k = 0; k < m_numPointsPerDimension; k++)
			{
				sum += pow((*m_grid)[i][j][k], 2);
			}
		}
	}
	std::cout << "Sum of squares: " << sum << std::endl;
	*/
}


void Hydrogen::SetQuantumNumbers(unsigned int n, unsigned int l, unsigned int m)
{
	m_n = n;
	m_l = l;
	m_m = m;
}

double Hydrogen::RadialComponent(double r)
{
	float rho = 2.0 * r / (m_n * m_a0);
	float coeff = sqrt(pow(2.0 / (m_n * m_a0), 3) * gsl_sf_fact(m_n - m_l - 1) / (2.0 * m_n * pow(gsl_sf_fact(m_n + m_l), 3)));
	coeff *= exp(-1 * (rho / 2.0)) * pow(rho, m_l);
	//std::cout << "rho: " << rho << ", coeff: " << coeff << ", exp: " << exp(-1 * (rho / 2.0)) << ", rho^l: " << pow(rho, m_l) << std::endl;

	return coeff * gsl_sf_laguerre_n(m_n - m_l - 1, 2 * m_l + 1, rho);
}

std::complex<double> Hydrogen::SphericalHarmonic(double theta, double phi)
{
	std::complex<double> y = gsl_sf_legendre_sphPlm(m_l, m_m, cos(theta)) * exp(std::complex<double>(0, m_m * phi));
	y *= pow(-1, m_m) * sqrt(((2 * m_l + 1) * gsl_sf_fact(m_l - m_m)) / ((4 * M_PI) * gsl_sf_fact(m_l + m_m)));
	//std::cout << "Y(" << theta << ", " << phi << ") = " << y << std::endl;
	return y;
}

std::vector<std::vector<std::vector<double>>>* Hydrogen::getGrid()
{
	return m_grid;
}

std::vector<std::vector<double>> Hydrogen::MonteCarloSample(std::size_t numSamples)	//Monte Carlo sampling to hopefully offload per frame calculations before rendering
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);

	// Calculate the total probability in the grid
	double totalProbability = 0;
	for (const auto& plane : *m_grid) {
		for (const auto& row : plane) {
			for (double probability : row) {
				totalProbability += probability;
			}
		}
	}

	// Sample positions based on the PDF values
	std::vector<std::vector<double>> samples;
	for (std::size_t m = 0; m < numSamples; m++) {
		double randomValue = dis(gen) * totalProbability;
		double cumulativeProbability = 0;

		// Traverse the grid to find the sampled position
		for (int i = 0; i < m_numPointsPerDimension; i++) {
			for (int j = 0; j < m_numPointsPerDimension; j++) {
				for (int k = 0; k < m_numPointsPerDimension; k++) {
					double probability = (*m_grid)[i][j][k];
					cumulativeProbability += probability;
					if (cumulativeProbability >= randomValue) {
						// Calculate the physical coordinates of the sampled position
						int center = (m_numPointsPerDimension - 1) / 2;
						double x = (i - center) * m_stepSize;
						double y = (j - center) * m_stepSize;
						double z = (k - center) * m_stepSize;
						samples.push_back({ x, y, z, probability });
						goto next;
					}
				}
			}
		}
		next:;
	}
	return samples;
}

double round(double x, int decimalPlaces)
{
	double factor = pow(10, decimalPlaces);
	return std::round(x * factor) / factor;
}