#pragma once
#include <cmath>
#include <complex>
#include <vector>
#include <gsl/gsl_sf_legendre.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_math.h>
#include <random>

class Hydrogen
{
public:
	Hydrogen(unsigned int n, unsigned int l, int m, unsigned int dims);
	~Hydrogen();
	Hydrogen(const Hydrogen& rhs);
	Hydrogen& operator=(const Hydrogen& rhs);

	double GetPDFAtPosition(double x, double y, double z);
	void SetQuantumNumbers(unsigned int n, unsigned int l, unsigned int m);

	std::vector<std::vector<std::vector<double>>>* getGrid();
	std::vector<std::vector<double>> MonteCarloSample(std::size_t numSamples);

private:
	unsigned int m_n;
	unsigned int m_l;
	unsigned int m_m;
	double m_physicalDimension;
	double m_a0;
	unsigned int m_numPointsPerDimension;
	double m_stepSize;
	std::vector<std::vector<std::vector<double>>>* m_grid;

	void NormalizeWaveFunction();
	double RadialComponent(double r);
	std::complex<double> SphericalHarmonic(double theta, double phi);
	void GenerateGrid();
};

double round(double x, int decimalPlaces);