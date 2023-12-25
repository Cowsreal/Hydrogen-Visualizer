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

Sphere* GenerateHydrogenAtom(int dim, int n, int l, int m, double size)
{
    // GENERATE HYDROGEN ATOM

    // PDF TOLERANCE, i.e., the minimum PDF value to render
    double tolerance = 0.0001;

    Hydrogen hydrogen(n, l, m, dim, size);

    // SAMPLE GRID POSITIONS SO WE DONT HAVE TO RENDER ALL OF THEM
    std::vector<std::vector<double>> samples = hydrogen.MonteCarloSample(100000);

    double sumOfSquares = 0.0;

    // NORMALIZE PDF
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

    std::vector<SphereInstance> sphereInstances;
    float spacing = 5.0f;

    for (int i = 0; i < samples.size(); i++)
    {
        double currPdf = samples[i][3];
        if (currPdf > tolerance)
        {
            // GENERATE POSITION, COLOR, SIZE VALUES FOR EACH SPHERE IN THE INSTANCE
            maxPDF = std::max(maxPDF, currPdf);
            minPDF = std::min(minPDF, currPdf);
            SphereInstance currInstance;
            currInstance.position[0] = samples[i][0] * 1e11 * spacing;
            currInstance.position[1] = samples[i][1] * 1e11 * spacing;
            currInstance.position[2] = samples[i][2] * 1e11 * spacing;
            glm::vec4 color = GetInterpolatedColor(currPdf, minPDF, maxPDF);
            currInstance.color[0] = (float)color.r;
            currInstance.color[1] = (float)color.g;
            currInstance.color[2] = (float)color.b;
            currInstance.color[3] = (float)color.a;
            currInstance.radius = GetInterpolatedSize(currPdf, minPDF, maxPDF) * size / 10.0;
            sphereInstances.push_back(currInstance);
        }
    }

    std::cout << "Finished" << std::endl;

    Sphere* sphere = new Sphere(sphereInstances);

    return sphere;
}

void ChangeStates(bool &s1, bool&s2)
{
    s1 = !s1;
	s2 = !s2;
}

Sphere* ConfirmHydrogenStates(int tn, int tl, int tm, int bc, float size)
{
    return GenerateHydrogenAtom(bc, tn, tl, tm, double(size));
}

int main()
{
    float size = 50.0;
    bool render = false;
    bool menu = true;
    bool drawGround = false;
    bool drawHydrogen = true;
    bool drawCoordinateAxis = true;
    int n = 3;
    int l = 2;
    int m = 2;
    float escCooldown = 0.5f;
    double lastKeyPressTime = 0;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//set the major version of OpenGL to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	//set the minor version of OpenGL to 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//set the OpenGL profile to core

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, ";-; this is so fuckign painful", NULL, NULL);
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

    //INITIALIZATION OPTIONS

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_FRONT));
    GLCall(glFrontFace(GL_CCW));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glViewport(0, 0, windowWidth, windowHeight));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));
    GLCall(glClearColor(0.529f, 0.828f, 0.952f, 1.0f));
    //GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

    // IMGUI INITIALIZATION
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    {
        float positions[] = {
            -10000.0f,  0.0f, -10000.0f,
             10000.0f,  0.0f, -10000.0f,
             10000.0f,  0.0f,  10000.0f,
            -10000.0f,  0.0f,  10000.0f,
        };

        unsigned int indices[] = {
            0, 1, 2,        //Triangle 1
            2, 3, 0 	    //Triangle 2
        };
        
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

        glm::vec3 translationA(0, -1000.0f, 0);

        float nearPlaneDistance = 0.1;
        Camera camera(glm::radians(90.0f), (float)windowWidth / (float)windowHeight, nearPlaneDistance, 50000.0f, window);
        camera.SetPosition(glm::vec3(0, 50.0f, 0.0));

        Controls controls(window);
        camera.BindControls(&controls);

        Axis axis(5000.0f);

        int bc = 50;
        Sphere* sphere = GenerateHydrogenAtom(bc, n, l, m, size);
        
        // MOVEMENT SETTINGS

        float fov = 45.0f;
        float speed = 2.5f;
        float sensitivity = 0.1f;

        int tn = n;
        int tl = l;
        int tm = m;
        // RENDERING LOOP
        while (!glfwWindowShouldClose(window))
        {
            float time = glfwGetTime(); // Get the current time in seconds

            glm::mat4 viewMatrix = camera.GetViewMatrix();
            glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
            /* Render here */
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();
            if (menu)
            {
                ImGui::SetNextWindowSize(ImVec2(500, 500));
                ImGui::SetNextWindowPos(ImVec2(960, 540)); // Position: x=50, y=50
                ImGui::Begin("Menu");
                if (ImGui::Button("Render Hydrogen Atom")) 
                {
                    ChangeStates(menu, render); // Call your function when the button is pressed.
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                ImGui::SliderInt("n", &tn, 1, 50);
                // Adjust the range of l based on n (0 to n - 1)
                if (tl >= tn)
                {
                    tl = tn - 1; // Keep l within the valid range when n changes
                }
                ImGui::SliderInt("l", &tl, 0, tn - 1);
                // Adjust the range of m based on l (-l to l)
                if (tm < -tl)
                {
                    tm = -tl; // Adjust m to stay within the new range
                }
                if (tm > tl)
                {
                    tm = tl;
                }
                ImGui::SliderInt("m", &tm, -tl, tl);
                ImGui::SliderInt("Ball Count", &bc, 10, 200);
                ImGui::SliderFloat("Size", &size, 0.0, 1000.0);

                // Create confirm button
                if (ImGui::Button("Confirm Atom")) 
                {
                    delete sphere;
                    sphere = ConfirmHydrogenStates(tn, tl, tm, bc, size); // Call the Confirm function when the button is pressed
                }

                if (ImGui::Button("Exit"))
                {
					glfwSetWindowShouldClose(window, true);
				}

				ImGui::End();
                ImGui::Begin("Application Controls");
                ImGui::SliderFloat("FOV", &fov, 0.0f, 120.0f);
                ImGui::SliderFloat("Flying Speed", &speed, 0.0f, 50.0f);
                ImGui::SliderFloat("Mouse Sensitivity", &sensitivity, 0.0f, 2.0f);
                ImGui::Text("Camera Position: %.3f, %.3f, %.3f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Text("Time: %.3fs", time);
                ImGui::End();

                ImGui::Begin("Object Controls");
                ImGui::Checkbox("Hydrogen", &drawHydrogen);
                ImGui::Checkbox("Ground", &drawGround);
                ImGui::Checkbox("Coordinate Axis", &drawCoordinateAxis);
                ImGui::End();
                ImGui::Render();
                ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
                {
                    // Toggle the visibility
                    ChangeStates(menu, render);
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    lastKeyPressTime = time;
                }
            }
            if (render)
            {
                if (drawCoordinateAxis)
                {   //Coordinate axis
                    shader.Bind();
                    glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                    glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                    shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); //set the uniform
                    glLineWidth(3.0f);
                    axis.Draw();
                }
                if (drawGround)
                {   //Green plane
                    shader.Bind();
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA); //create a model matrix
                    glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                    shader.SetUniform4f("u_Color", 0.482f, 0.62f, 0.451f, 1.0f); //set the uniform
                    shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    renderer.Draw(va, ib, shader);
                }
                if (drawHydrogen)
                {
                    sphereShader.Bind();
                    glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                    glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                    sphereShader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    sphere->Draw();
                }
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
                {
                    lastKeyPressTime = time;
                    // Toggle the visibility
                    ChangeStates(render, menu);
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }

                ImGui::SetNextWindowSize(ImVec2(400, 80));
                ImGui::Begin("Status:");
                ImGui::Text("Camera Position: %.3f, %.3f, %.3f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Text("Time: %.3fs", time);
                ImGui::End();

                // CAMERA CONTROLS
                camera.SetFOV(fov);
                camera.setSpeed(speed);
                camera.setSensitivity(sensitivity);

                ImGui::Render();
                ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
                camera.ProcessControls();
            }
            

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}