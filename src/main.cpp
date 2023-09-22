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
#include "glm/gtx/color_space.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

const unsigned int windowWidth = 1920;
const unsigned int windowHeight = 1080;

glm::vec4 GetInterpolatedColor(double pdf, double minPDF, double maxPDF)
{
    double exponent = 0.5;
    double adjustedPDF = std::pow(pdf - minPDF, exponent);
    double adjustedMinPDF = 0.0;
    double adjustedMaxPDF = std::pow(maxPDF - minPDF, exponent);

    double normalizedPDF = (adjustedPDF - adjustedMinPDF) / (adjustedMaxPDF - adjustedMinPDF);

    // Define the colors of the rainbow spectrum in HSV
    glm::vec3 red = glm::rgbColor(glm::vec3(0.0f, 1.0f, 1.0f));
    glm::vec3 orange = glm::rgbColor(glm::vec3(30.0f, 1.0f, 1.0f));
    glm::vec3 yellow = glm::rgbColor(glm::vec3(60.0f, 1.0f, 1.0f));
    glm::vec3 green = glm::rgbColor(glm::vec3(120.0f, 1.0f, 1.0f));
    glm::vec3 blue = glm::rgbColor(glm::vec3(240.0f, 1.0f, 1.0f));
    glm::vec3 violet = glm::rgbColor(glm::vec3(270.0f, 1.0f, 1.0f));

    glm::vec3 startColor, endColor;

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
        normalizedPDF = (normalizedPDF - 1.0 / 6.0) * 6;
    }
    else if (normalizedPDF < 4.0 / 6.0) {
        startColor = green;
        endColor = blue;
        normalizedPDF = (normalizedPDF - 1.0 / 6.0) * 6;
    }
    else if (normalizedPDF < 5.0 / 6.0) {
        startColor = blue;
        endColor = violet;
        normalizedPDF = (normalizedPDF - 1.0 / 6.0) * 6;
    }
    else {
        startColor = violet;
        endColor = red;
        normalizedPDF = (normalizedPDF - 1.0 / 6.0) * 6;
    }

    glm::vec3 interpolatedColor = startColor * (1.0f - (float)normalizedPDF) + endColor * (float)normalizedPDF;
    return glm::vec4(interpolatedColor, 1.0f); // Assuming alpha is 1.0
}

float GetInterpolatedSize(double pdf, double minPDF, double maxPDF)
{
	double size1 = 0.25;
	double size2 = 1.75;

	//Normalize the pdf value to be between 0.0 and 1.0 based on your observed min and max pdf values
	double normalizedPDF = (pdf - minPDF) / (maxPDF - minPDF);

	//Interpolate between size1 and size2 based on normalizedPDF
	return size1 * (1.0 - normalizedPDF) * 2 + size2 * normalizedPDF;
}

int main()
{
    bool drawGround = false;
    bool drawHydrogen = true;
    bool drawCoordinateAxis = true;
 
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

        VertexBufferLayout layout;  //create a vertex buffer layout
        layout.Push<float>(3);  //push a float with 2 elements to the layout
        va.AddBuffer(vb, layout, false);   //add the vertex buffer and the layout to the vertex array
        IndexBuffer ib(indices, 6); //create an index buffer

        Shader shader("res/shaders/Basic.shader"); //create a shader
        Shader shader2("res/shaders/Sinusoid.shader"); //create a shader
        Shader shader3("res/shaders/test.shader"); //create a shader
        Shader shader4("res/shaders/test2.shader"); //create a shader
        Shader sphereShader("res/shaders/Sphere.shader"); //create a shader

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        Renderer renderer; //create a renderer

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translationA(0, -50.0f, 0);

        Camera camera(glm::radians(90.0f), (float)windowWidth / (float)windowHeight, 0.1f, 5000.0f, window);
        camera.SetPosition(glm::vec3(0, 50.0f, 0.0));

        Controls controls(window);
        camera.BindControls(&controls);

        Axis axis(5000.0f);

        int dim = 50;
        double tolerance = 0.0001;

        Hydrogen hydrogen(3, 2, 2, dim);
        //std::vector<std::vector<std::vector<double>>>* pdf = hydrogen.getGrid();
        std::vector<std::vector<double>> samples = hydrogen.MonteCarloSample(100000);

        double sumOfSquares = 0.0;

        for (int i = 0; i < samples.size(); i++)
        {
		    sumOfSquares += samples[i][3] * samples[i][3];
        }
        sumOfSquares = sqrt(sumOfSquares);
        for (int i = 0; i < samples.size(); i++)
        {
			samples[i][3] /= sumOfSquares;
		}
        double maxPDF = std::numeric_limits<double>::lowest();
        double minPDF = std::numeric_limits<double>::max();
        //Let's calculate a dynamic tolernace variable

        std::vector<SphereInstance> sphereInstances;
        float spacing = 5.0f;

        for (int i = 0; i < samples.size(); i++)
        {
            double currPdf = samples[i][3];
            if (currPdf > tolerance)
            {
                maxPDF = std::max(maxPDF, currPdf);
                minPDF = std::min(minPDF, currPdf);
                SphereInstance currInstance;
                currInstance.position[0] = samples[i][0] * 1e11 * spacing;
                currInstance.position[1] = samples[i][1] * 1e11 * spacing;
                currInstance.position[2] = samples[i][2] * 1e11 * spacing;
                glm::vec4 color = GetInterpolatedColor(currPdf, minPDF, maxPDF);
                currInstance.color[0] = (float) color.r;
                currInstance.color[1] = (float) color.g;
                currInstance.color[2] = (float) color.b;
                currInstance.color[3] = (float) color.a;
                currInstance.radius = GetInterpolatedSize(currPdf, minPDF, maxPDF) * 5.0;

                sphereInstances.push_back(currInstance);
			}
		}

        Sphere sphere = Sphere(sphereInstances);
        float fov = 4000.0f;
        float speed = 0.3f;
        float sensitivity = 0.1f;

        glViewport(0, 0, windowWidth, windowHeight);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        GLCall(glClearColor(0.529f, 0.828f, 0.952f, 1.0f));

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            float time = glfwGetTime(); // Get the current time in seconds

            glm::mat4 viewMatrix = camera.GetViewMatrix();
            glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
            /* Render here */
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();
            if(drawCoordinateAxis)
            {   //Coordinate axis
                shader.Bind();
                glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); //set the uniform
                glLineWidth(3.0f);
                axis.Draw();
            }
            if(drawGround)
            {   //Green plane
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.SetUniform4f("u_Color", 0.482f, 0.62f, 0.451f, 1.0f); //set the uniform
                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                renderer.Draw(va, ib, shader);
            }
            if(drawHydrogen)
            {
                sphereShader.Bind();
                glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                sphereShader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                sphere.Draw();
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
                ImGui::Checkbox("Hydrogen", &drawHydrogen);
                ImGui::Checkbox("Ground", &drawGround);
                ImGui::Checkbox("Coordinate Axis", &drawCoordinateAxis);
                if (drawHydrogen)
                {
                    ImGui::SliderFloat("Spacing", &spacing, 0.01f, 100.0f);
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