//TODO: 
//      Create sphere renderer
//      Create PDF definition for hydrogen atom wave function
//      Create a Monte Carlo based position sampler
//      Create a sphere at each sampled position
//      Render the spheres
//      Create a color mapper for regions of constant probability

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <unordered_map>

#include "Renderer.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Controls.hpp"
#include "render_geom/CoordinateAxis/CoordinateAxis.hpp"
#include "render_geom/Sphere/Sphere.hpp"

#include "QM/Hydrogen.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

const unsigned int windowWidth = 1920;
const unsigned int windowHeight = 1080;

float CalculateWaveHeight(float xamp, float zamp, float x, float z, float currentTime)
{
	float y = 0.0f;

	// Calculate the wave height at the given position

		y = sinf(0.5 * (xamp * x + zamp * z + 10.0 * currentTime)) * 3.0;

	return y;
}

glm::vec4 GetInterpolatedColor(double pdf, double minPDF, double maxPDF)
{
    // Define the colors of the rainbow spectrum
    glm::vec4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 orange = { 1.0f, 0.5f, 0.0f, 1.0f };
    glm::vec4 yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };
    glm::vec4 indigo = { 0.4f, 0.0f, 0.8f, 1.0f };
    glm::vec4 violet = { 0.7f, 0.0f, 1.0f, 1.0f };

    // Normalize the pdf value to be between 0.0 and 1.0 based on observed min and max pdf values
    double normalizedPDF = (pdf - minPDF) / (maxPDF - minPDF);

    glm::vec4 startColor, endColor;

    if (normalizedPDF < 1.0 / 6.0) {
        startColor = red;
        endColor = orange;
        normalizedPDF *= 6;
    }
    else if (normalizedPDF < 2.0 / 6.0) {
        startColor = orange;
        endColor = yellow;
        normalizedPDF = (normalizedPDF - 1.0 / 6.0) * 6;
    }
    else if (normalizedPDF < 3.0 / 6.0) {
        startColor = yellow;
        endColor = green;
        normalizedPDF = (normalizedPDF - 2.0 / 6.0) * 6;
    }
    else if (normalizedPDF < 4.0 / 6.0) {
        startColor = green;
        endColor = blue;
        normalizedPDF = (normalizedPDF - 3.0 / 6.0) * 6;
    }
    else if (normalizedPDF < 5.0 / 6.0) {
        startColor = blue;
        endColor = indigo;
        normalizedPDF = (normalizedPDF - 4.0 / 6.0) * 6;
    }
    else {
        startColor = indigo;
        endColor = violet;
        normalizedPDF = (normalizedPDF - 5.0 / 6.0) * 6;
    }

    // Interpolate between the current pair of colors based on the updated normalizedPDF
    return startColor * (1.0f - (float)normalizedPDF) + endColor * (float)normalizedPDF;
}


float GetInterpolatedSize(double pdf, double minPDF, double maxPDF)
{
	double size1 = 0.25;
	double size2 = 1.75;

	// Normalize the pdf value to be between 0.0 and 1.0 based on your observed min and max pdf values
	double normalizedPDF = (pdf - minPDF) / (maxPDF - minPDF);

	// Interpolate between size1 and size2 based on normalizedPDF
	return size1 * (1.0 - normalizedPDF) * 2 + size2 * normalizedPDF;
}

int main(void)
{
    bool blinn = false;
    const int numRows = 100;
    const int numCols = 100;
    const float planeSizeX = 1000.0f;
    const float planeSizeZ = 1000.0f;
    const float cellWidth = planeSizeX / (numCols - 1);
    const float cellDepth = planeSizeZ / (numRows - 1);

    // Create an array to hold vertex positions
    float vertices[numRows * numCols * 3];

    // Calculate heights and update vertex positions

    const int numIndices = (numRows - 1) * (numCols - 1) * 6;

    // Step 3: Allocate memory for indices
    unsigned int indices2[numIndices];

    // Step 4: Generate indices
    int index = 0;
    for (int i = 0; i < numRows - 1; i++) {
        for (int j = 0; j < numCols - 1; j++) {
            // Define indices for the two triangles of each grid cell
            int topLeft = i * numCols + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * numCols + j;
            int bottomRight = bottomLeft + 1;

            // Triangle 1
            indices2[index++] = topLeft;
            indices2[index++] = topRight;
            indices2[index++] = bottomLeft;

            // Triangle 2
            indices2[index++] = topRight;
            indices2[index++] = bottomRight;
            indices2[index++] = bottomLeft;
        }
    }


    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//set the major version of OpenGL to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	//set the minor version of OpenGL to 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//set the OpenGL profile to core

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, ";-; this is so painful", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;
    {

        float positions[] = {
            -2000.0f,  -20.0f, -2000.0f,
             2000.0f,  -20.0f, -2000.0f,
             2000.0f,  -20.0f,  2000.0f,
            -2000.0f,  -20.0f,  2000.0f,
        };


        unsigned int indices[] = {
            0, 1, 2,        //Triangle 1
            2, 3, 0 	    //Triangle 2
        };
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va; //create a vertex array
        VertexBuffer vb(positions, 4 * 3 * sizeof(float)); //create a vertex buffer

        IndexBuffer ib2(indices2, numIndices);
        VertexBufferLayout layout;  //create a vertex buffer layout
        layout.Push<float>(3);  //push a float with 2 elements to the layout
        va.AddBuffer(vb, layout);   //add the vertex buffer and the layout to the vertex array
        IndexBuffer ib(indices, 6); //create an index buffer

        Shader shader("res/shaders/Basic.shader"); //create a shader
        Shader shader2("res/shaders/Sinusoid.shader"); //create a shader
        Shader shader3("res/shaders/test.shader"); //create a shader
        Shader shader4("res/shaders/test2.shader"); //create a shader


        //COORDINATE AXIS

        //GLCall(glUseProgram(1));
        //std::cout << "LOL" << std::endl;

        //shader.Bind(); //bind the shader

        //Texture texture("res/textures/img.png"); //create a texture)
        //texture.Bind();
        //shader.SetUniform1i("u_Texture", 0); //set the uniform
        Sphere sphere = Sphere(100.0f);

        std::cout << "Constructed sphere" << std::endl;
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        Renderer renderer; //create a renderer

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translationA(0, -50.0f, 0);
        glm::vec3 translationB(0, 5.0f, 0.0f);
        glm::vec3 rotationB(0, 1, 0);

        Camera camera(glm::radians(90.0f), (float)windowWidth / (float)windowHeight, 0.1f, 5000.0f, window);
        camera.SetPosition(glm::vec3(0, 50.0f, 0.0));

        Controls controls(window);
        camera.BindControls(&controls);

        Axis axis(5000.0f);

        int dim = 50;
        double tolerance = 0.007;
        bool drawHydrogen = true;

        Hydrogen hydrogen(2, 1, 1, dim);
        std::vector<std::vector<std::vector<double>>>* pdf = hydrogen.getGrid();

        for (int i = 0; i < dim; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                for (int k = 0; k < dim; k++)
                {
                    if (i == j && j == k)
                    {
                        //std::cout << (*pdf)[124][124][i] << ", " << std::endl;
                    }
				}
			}
        }

        double minPDF = 99999.0;
        double maxPDF = -99999.0;

        //Let's calculate a dynamic tolernace variable

        std::unordered_map<float, Sphere*> SphereMap;
        for(int i = 0; i < dim; i++)
        {
            for(int j = 0; j < dim; j++)
            {
                for(int k = 0; k < dim; k++)
                {
                    double currPdf = (*pdf)[i][j][k] * 2.5;
                    //std::cout << currPdf << std::endl;
                    if(currPdf > tolerance)
					{
                        if (currPdf > maxPDF)
                        {
                            maxPDF = currPdf;
                        }
                        else if (currPdf < minPDF)
                        {
                            minPDF = currPdf;
                        }
                        if (SphereMap.find(currPdf) == SphereMap.end())     //Precompute all spheres
                        {
							SphereMap[currPdf] = new Sphere(GetInterpolatedSize(currPdf, minPDF, maxPDF));
                        }
					}
                }
            }
        }
        float fov = 4000.0f;
        float speed = 0.3f;
        float sensitivity = 0.1f;
        float waveLength = 2.0f;
        float phaseShift = 0.5f;
        float amplitude = 0.5f;
        float norm = 0.1f;
        float xamp = 1.0f;
        float zamp = 1.0f;
        glm::vec3 lightPos(0.0f, 50.0f, 0.0f);
        glm::vec3 lightColor(1.0f, 0.8549f, 0.7255f);
        /* Loop until the user closes the window */
        glViewport(0, 0, windowWidth, windowHeight);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        GLCall(glClearColor(0.529f, 0.808f, 0.922f, 1.0f));


        while (!glfwWindowShouldClose(window))
        {
            float time = glfwGetTime(); // Get the current time in seconds
            // Define the properties of the rainbow effect

            glm::mat4 viewMatrix = camera.GetViewMatrix();
            glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
            /* Render here */
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();
            
            {   //Coordinate axis
                shader.Bind();
                glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); //set the uniform
                glLineWidth(3.0f);
                axis.Draw();
            }
            /*
            {   //Sphere
                glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                shader.SetUniform4f("u_Color", 0.0f, 1.0f, 1.0f, 1.0f); //set the uniform
                sphere.Draw();
            }
            */
            {   //Green plane
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.SetUniform4f("u_Color", 0.482f, 0.62f, 0.451f, 1.0f); //set the uniform
                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                renderer.Draw(va, ib, shader);
            }
            {   //Walls
                for (int i = 0; i < numRows; i++) {
                    for (int j = 0; j < numCols; j++) {
                        float x = j * cellWidth;
                        float z = i * cellDepth;
                        float y = CalculateWaveHeight(xamp, zamp, x, z, time); // Calculate wave height for this vertex
                        vertices[(i * numCols + j) * 3 + 0] = x;
                        vertices[(i * numCols + j) * 3 + 1] = y;
                        vertices[(i * numCols + j) * 3 + 2] = z;
                    }
                }
                VertexArray va2 = VertexArray();
                VertexBuffer vb2(vertices, numRows * numCols * 3 * sizeof(float));
                VertexBufferLayout layout;
                layout.Push<float>(3);
                va2.AddBuffer(vb2, layout);
                if (blinn)
                {
                    shader2.Bind();
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB); //create a model matrix
                    glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                    shader2.SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z); //set the uniform
                    shader2.SetUniform3f("u_LightColor", lightColor.x, lightColor.y, lightColor.z); //set the uniform
                    shader2.SetUniform3f("u_ViewPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z); //set the uniform
                    shader2.SetUniform1f("u_Wavelength", waveLength); //set the uniform 
                    shader2.SetUniform1f("u_Time", time); //set the uniform
                    shader2.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    renderer.Draw(va2, ib2, shader2);
                }
                else
                {
                    // Calculate minZ and maxZ based on the actual z-coordinates of the vertices
                    float minZ = 1e6; // Initialize to a large value
                    float maxZ = -1e6; // Initialize to a small value
                    // Loop through the vertices to find minZ and maxZ
                    for (int i = 0; i < numRows; i++) {
                        for (int j = 0; j < numCols; j++) {
                            float z = vertices[(i * numCols + j) * 3 + 2];
                            minZ = std::min(minZ, z);
                            maxZ = std::max(maxZ, z);
                        }
                    }
                    
                    shader3.Bind();//rotate along x axis
                    glm::mat4 model = glm::rotate(glm::translate(glm::mat4(1.0f), translationB), glm::radians(-90.0f), glm::vec3(1, 0, 0)); //create a model matrix
                    glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                    shader3.SetUniform1f("u_Wavelength", waveLength); //set the uniform 
                    shader3.SetUniform1f("u_PhaseShift", phaseShift); //set the uniform 
                    shader3.SetUniform1f("u_Amplitude", amplitude); //set the uniform 
                    shader3.SetUniform1f("u_Time", time); //set the uniform
                    shader3.SetUniform1f("u_Norm", norm); //set the uniform
                    shader3.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    shader3.SetUniform3f("iResolution", windowWidth, windowHeight, 1.0f);
                    shader3.SetUniform1f("iTime", time);
                    renderer.Draw(va2, ib2, shader3);

                    glm::mat4 model2 = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
                    mvp = projectionMatrix * viewMatrix * model2;
                    shader4.Bind();
                    shader4.SetUniformMat4f("u_MVP", mvp); //set the uniform
					shader4.SetUniform3f("iResolution", windowWidth, windowHeight, 1.0f);
                    shader4.SetUniform1f("iTime", time);
                    renderer.Draw(va2, ib2, shader4);

                    shader3.Bind();
                    model = glm::translate(model, glm::vec3(0.0f, -1000.0f, 0.0f));
                    mvp = projectionMatrix * viewMatrix * model;
                    shader3.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    renderer.Draw(va2, ib2, shader3);

                    model2 = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
                    mvp = projectionMatrix * viewMatrix * model2;
                    shader4.Bind();
                    shader4.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    renderer.Draw(va2, ib2, shader4);
                }
            }
            
            if(drawHydrogen)
            {
                for (int i = 0; i < dim; i++)
                {
                    for (int j = 0; j < dim; j++)
                    {
                        for (int k = 0; k < dim; k++)
                        {
                            double currPDF = (*pdf)[i][j][k] * 2.5;
                            //std::cout << "Drawing sphere at " << i << ", " << j << ", " << k << "currPDF = " << currPDF<< std::endl;
                            if (currPDF > tolerance)
                            {
                                glm::vec4 color = GetInterpolatedColor(currPDF, minPDF, maxPDF);
                                shader.Bind();
                                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 5.0f, j * 5.0f, k * 5.0f)); //create a model matrix
                                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                                shader.SetUniform4f("u_Color", color.x, color.y, color.z, color.w); //set the uniform
                                SphereMap[currPDF]->Draw();
                            }
                        }
                    }
                }
            }
            
            {
                ImGui::Begin("Application Controls");
                ImGui::SliderFloat("FOV", &fov, 0.0f, 10000.0f);
                ImGui::SliderFloat("Flying Speed", &speed, 0.0f, 5.0f);
                ImGui::SliderFloat("Mouse Sensitivity", &sensitivity, 0.0f, 5.0f);
                ImGui::Text("Camera Position: %.3f, %.3f, %.3f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();

                ImGui::Begin("Object Controls");
                ImGui::Checkbox("Blinn", &blinn);
                ImGui::Checkbox("Hydrogen", &drawHydrogen);
                ImGui::SliderFloat3("Translation A", &translationA.x, -960.f, 960.0f);
                ImGui::SliderFloat3("Translation B", &translationB.x, -960.f, 960.0f);
                ImGui::SliderFloat("X Amplitude", &xamp, 0.0f, 100.0f);
                ImGui::SliderFloat("Z Amplitude", &zamp, 0.0f, 100.0f);
                ImGui::SliderFloat("Wave Length", &waveLength, 0.001f, 50.0f);
                if(blinn)
                {
                    ImGui::SliderFloat3("Light Position", &lightPos.x, -960.f, 960.0f);
                    ImGui::SliderFloat3("Light Color", &lightColor.x, 0.0f, 1.0f);
                }
                else
                {
                    ImGui::SliderFloat("Phase Shift", &phaseShift, 0.001f, 50.0f);
                    ImGui::SliderFloat("Amplitude", &amplitude, 0.001f, 5.0f);
                    ImGui::SliderFloat("Norm", &norm, 0.001f, 5.0f);
                }
            }
            camera.SetFOV(glm::radians(fov));
            camera.setSpeed(speed);
            camera.setSensitivity(sensitivity);

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
            camera.ProcessControls();
        }
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}