# Hydrogen Visualizer
 
## About this Program

This is a visualization of the hydrogen wave function with different options of the $(n, l, m)$ quantum numbers. This project is written in C++ and rendered in OpenGL.

[Video Demo (Youtube)](https://youtu.be/CzYODLT_00g)

## Theory

In quantum mechanics, the wave function $\psi$ of a quantum system can be described by the Schrödinger equation. 

$$i\hbar\frac{\partial}{\partial t}\lvert \psi \rangle = \hat{H} \lvert \psi \rangle$$

where $i$ is the imaginary unit, $\hbar$ is the reduced Planck's constant, $t$ is time, and $\hat{H}$ is the hamiltonian operator, a linear transformation on vectors in a Hilbert Space. My program visualizes the probability distribution of the electron in the hydrogen atom, plotting via color and size of each sphere, of the norm square of the wave function. The analytic solution of the Schrödinger equation for the hydrogen atom is then given by a radial part $R_{n, l}(r)$ and an angular component $Y_{l}^m(\theta, \phi)$.

$$\psi_{n, l, m}(r, \theta, \phi) = R_{n, l}(r) \cdot Y_{l}^m(\theta, \phi)$$

The full solution for $R_{n, l}(r)$ and $Y_{l}^m(\theta, \phi)$ can be easily found online.

## Usage

1. Clone this repository

2. From the root of the repository, 
    ```
    xcopy res build\Release\res /E /I
    mkdir build
    cd build
    cmake -G "Visual Studio 17 2022" -A Win32 ..
    ```

3. Navigate to /build and build the project using release build either in Visual Studio or in terminal via MSBuild.

4. Run build/release/main.exe

## Future Work

* Fix scaling sizes of atom
* Find a better way to adaptively scale visualization particle sizes better
* "Loading" a new atomic configuration is slow and cumbersome to compute, so find a way to parallelize computation, or run in background without freezing the program momentarily.
* Clean up code

## Acknowledgements

This project uses OpenGL abstraction classes from "The Cherno"'s [tutorial series](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2). This [Wikipedia](https://en.wikipedia.org/wiki/Hydrogen_atom) article on the subject was also very helpful.