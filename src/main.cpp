#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <string>

#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Controls.hpp"
#include "FrameBuffer.hpp"
#include "GlobalFunctions.hpp"
#include "render_geom/CoordinateAxis/CoordinateAxis.hpp"
#include "render_geom/Sphere/Sphere.hpp"
#include "render_geom/Circle/Circle.hpp"
#include "render_geom/Hopf/Hopf.hpp"
#include "render_geom/Points/Points.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/color_space.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#define PI 3.14159265358979323846

int fullscreenWidth = 1920;
int fullscreenHeight = 1080;

int windowedWidth = 1920;
int windowedHeight = 1080;

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
    bool drawCoordinateAxis = true;
<<<<<<< HEAD
    int n = 3;
    int l = 2;
    int m = 2;
    float escCooldown = 0.5f;
    double lastKeyPressTime = 0;
=======
    bool drawCircle = true;
    bool drawAsPoints = false;
    bool hideUi = false;
    bool fullscreen = false;
    bool vsync = true;
    float escCooldown = 0.5f;
    float pointSize = 10.0f;
    double lastKeyPressTime = 0;
    int numPoints = 100;
    int numGreatCircles = 1;
    std::vector<float> rotationXs(numGreatCircles, 0.0f);
    std::vector<float> rotationYs(numGreatCircles, 0.0f);
    std::vector<float> rotationZs(numGreatCircles, 0.0f);
    std::string mode = "GreatCircle";
>>>>>>> a8e6566ecf49eea1651fd1e600cbde840ec69ccf

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//set the major version of OpenGL to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	//set the minor version of OpenGL to 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//set the OpenGL profile to core

    /* Create a windowed mode window and its OpenGL context */
<<<<<<< HEAD
    window = glfwCreateWindow(windowWidth, windowHeight, ";-; this is so fuckign painful", NULL, NULL);
=======
    window = glfwCreateWindow(windowedWidth, windowedHeight, "Hopf Fibration", NULL, NULL);
>>>>>>> a8e6566ecf49eea1651fd1e600cbde840ec69ccf
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
<<<<<<< HEAD
    GLCall(glViewport(0, 0, windowWidth, windowHeight));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));
    GLCall(glClearColor(0.529f, 0.828f, 0.952f, 1.0f));
=======
    GLCall(glViewport(0, 0, windowedWidth, windowedHeight));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));
>>>>>>> a8e6566ecf49eea1651fd1e600cbde840ec69ccf
    //GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

    // IMGUI INITIALIZATION
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();
<<<<<<< HEAD

=======
>>>>>>> a8e6566ecf49eea1651fd1e600cbde840ec69ccf
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
        Shader shader2("res/shaders/Points.shader"); //create a shader

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        Renderer renderer; //create a renderer

        glm::vec3 translationA(0, -1000.0f, 0);

        float nearPlaneDistance = 0.1;
<<<<<<< HEAD
        Camera camera(glm::radians(90.0f), (float)windowWidth / (float)windowHeight, nearPlaneDistance, 50000.0f, window);
=======
        Camera camera(45.0f, (float)windowedWidth/ (float)windowedHeight, nearPlaneDistance, 50000.0f, window, true);
>>>>>>> a8e6566ecf49eea1651fd1e600cbde840ec69ccf
        camera.SetPosition(glm::vec3(0, 50.0f, 0.0));

        Controls controls(window);
        camera.BindControls(&controls);

        glfwSetWindowUserPointer(window, &camera);
        glfwSetCursorPosCallback(window, mouse_callback);

<<<<<<< HEAD
        int bc = 50;
        Sphere* sphere = GenerateHydrogenAtom(bc, n, l, m, size);
        
        // MOVEMENT SETTINGS

        float fov = 45.0f;
        float speed = 2.5f;
        float sensitivity = 0.1f;

        int tn = n;
        int tl = l;
        int tm = m;
=======
        Camera camera1(45.0f, (float)200 / (float)200, nearPlaneDistance, 100.0f, window, false);
        camera1.SetPosition(glm::vec3(30.0f, 30.0f, 30.0f));
        glm::vec3 cameraPos = glm::vec3(30.0f, 30.0f, 30.0f);
        glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f); // Origin
        glm::vec3 forwardVector = glm::normalize(targetPos - cameraPos);
        camera1.SetFront(forwardVector);

        std::vector<std::vector<std::vector<double>>> points;
        points.push_back(GenerateGreatCircle(rotationXs[0], rotationYs[0], rotationZs[0], numPoints));
        
        std::vector<Hopf> hopfs;
        hopfs.push_back(Hopf(&(points[0]), drawAsPoints, pointSize));

        std::vector<Points> pointsDrawers;
        pointsDrawers.push_back(Points(points[0], 10.0f));

        Axis axis(10000.0f);

        Sphere sphere(15.0f, 50);

        FrameBuffer fbo = FrameBuffer();

        fbo.AttachTexture();
        
        // MOVEMENT SETTINGS

        float fov = 45.0f;
        float speed = 2.5f;
        float sensitivity = 0.1f;
>>>>>>> a8e6566ecf49eea1651fd1e600cbde840ec69ccf
        // RENDERING LOOP
        while (!glfwWindowShouldClose(window))
        {
            float time = glfwGetTime(); // Get the current time in seconds

            glm::mat4 viewMatrix = camera.GetViewMatrix();
            glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
            /* Render here */

            ImGui_ImplGlfwGL3_NewFrame();
<<<<<<< HEAD
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
=======
            if(menu)
            {
                GLCall(glClearColor(0.529f, 0.828f, 0.952f, 1.0f));
                renderer.Clear();

                ImGui::SetNextWindowSize(ImVec2(500, 500));
                ImGui::SetNextWindowPos(ImVec2(960 - 250, 540 - 250)); // Position: x=50, y=50

                ImGui::Begin("Application Controls");

                if(ImGui::Checkbox("Vsync", &vsync))
                {
                    SetVsync(vsync);
                }
                
                ImGui::Text("Movement Controls");
                ImGui::SliderFloat("FOV", &fov, 0.0f, 120.0f);
                ImGui::SliderFloat("Flying Speed", &speed, 0.0f, 50.0f);
                ImGui::SliderFloat("Mouse Sensitivity", &sensitivity, 0.0f, 2.0f);

                ImGui::Text("ESC - Show Menu");
                ImGui::Text("Left Alt - Toggle Mouse");
                ImGui::Text("F1 - Hide UI");
                ImGui::Text("F11 - Toggle Fullscreen");

                if (ImGui::Button("Exit Application"))
                {
					glfwSetWindowShouldClose(window, true);
				}
                ImGui::End();
            }
            else if(render)
            {
                if(!hideUi)
                {
                    ImGui::SetNextWindowSize(ImVec2(400, 900));
                    ImGui::SetNextWindowPos(ImVec2(20, 120)); // Position: x=50, y=50
                    ImGui::Begin("Object Controls");
                    ImGui::Checkbox("Ground", &drawGround);
                    ImGui::Checkbox("Coordinate Axis", &drawCoordinateAxis);
                    ImGui::Checkbox("Circle", &drawCircle);
                    bool nChanged = ImGui::SliderInt("Fibers", &numPoints, 1, 300);
                    if(mode == "GreatCircle")
                    {
                        for(int i = 0; i < numGreatCircles; i++)
                        {
                            std::string label = "Great Circle " + std::to_string(i + 1);
                            ImGui::Text(label.c_str());

                            std::string xLabel = "X Rotation##" + std::to_string(i);
                            std::string yLabel = "Y Rotation##" + std::to_string(i);
                            std::string zLabel = "Z Rotation##" + std::to_string(i);

                            bool xChanged = ImGui::SliderFloat(xLabel.c_str(), &rotationXs[i], -3.14159f, 3.14159f);
                            bool yChanged = ImGui::SliderFloat(yLabel.c_str(), &rotationYs[i], -3.14159f, 3.14159f);
                            bool zChanged = ImGui::SliderFloat(zLabel.c_str(), &rotationZs[i], -3.14159f, 3.14159f);

                            if(xChanged || yChanged || zChanged || nChanged)
                            {
                                points[i] = GenerateGreatCircle(rotationXs[i], rotationYs[i], rotationZs[i], numPoints);
                                pointsDrawers[i].UpdatePoints(&(points[i]));
                                hopfs[i].UpdateCircles(&(points[i]));
                            }
                        }
                        if(ImGui::Button("Reset Rotations"))
                        {
                            for(int i = 0; i < numGreatCircles; i++)
                            {
                                rotationXs[i] = 0.0f;
                                rotationYs[i] = 0.0f;
                                rotationZs[i] = 0.0f;
                                points[i] = GenerateGreatCircle(rotationXs[i], rotationYs[i], rotationZs[i], numPoints);
                                pointsDrawers[i].UpdatePoints(&(points[i]));
                                hopfs[i].UpdateCircles(&(points[i]));
                            }
                        }
                        if(ImGui::SliderInt("Great Circles", &numGreatCircles, 1, 10))
                        {
                            if(numGreatCircles > hopfs.size())
                            {
                                for(int i = hopfs.size(); i < numGreatCircles; i++)
                                {
                                    rotationXs.push_back(0.0f);
                                    rotationYs.push_back(0.0f);
                                    rotationZs.push_back(0.0f);
                                    points.push_back(GenerateGreatCircle(rotationXs[i], rotationYs[i], rotationZs[i], numPoints));
                                    hopfs.push_back(Hopf(&(points[i]), drawAsPoints, pointSize));
                                    pointsDrawers.push_back(Points(points[i], 10.0f));
                                }
                            }
                            else
                            {
                                for(int i = hopfs.size(); i > numGreatCircles; i--)
                                {
                                    rotationXs.pop_back();
                                    rotationYs.pop_back();
                                    rotationZs.pop_back();
                                    points.pop_back();
                                    hopfs.pop_back();
                                    pointsDrawers.pop_back();
                                }
                            }
                        }
                    }
                    if(ImGui::Checkbox("Draw as Points", &drawAsPoints))
                    {
                        for(int i = 0; i < numGreatCircles; i++)
                        {
                            hopfs[i].SetDrawAsPoints(drawAsPoints);
                        }
                    }
                    if(drawAsPoints)
                    {
                        if(ImGui::SliderFloat("Point Size", &pointSize, 1.0f, 20.0f))
                        {
                            for(int i = 0; i < numGreatCircles; i++)
                            {
                                hopfs[i].ChangePointSize(pointSize);
                            }
                        }
                    }
                    ImGui::End();

                    // STATUS WINDOW

                    ImGui::SetNextWindowPos(ImVec2(20, 20));
                    ImGui::SetNextWindowSize(ImVec2(400, 80));
                    ImGui::Begin("Status:");
                    ImGui::Text("Camera Position: %.3f, %.3f, %.3f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    ImGui::Text("Time: %.3fs", time);
                    ImGui::End();

                    // S2 Sphere Preview Window

                    fbo.Bind();
                    {
                        GLCall(glClearColor(0.0f, 0.0, 0.0, 0.5f));
                        renderer.Clear();
                        shader.Bind();
                        glm::mat4 projectionMatrix = camera1.GetProjectionMatrix();
                        glm::mat4 viewMatrix = camera1.GetViewMatrix();
                        glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                        glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                        shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); //set the uniform
                        shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                        sphere.Draw();
                        shader2.Bind();
                        shader2.SetUniformMat4f("u_MVP", mvp);
                        for(int i = 0; i < numGreatCircles; i++)
                        {
                            pointsDrawers[i].Draw();
                        }
                    }
                    fbo.Unbind();
                    ImGui::SetNextWindowSize(ImVec2(300, 300));
                    ImGui::SetNextWindowPos(ImVec2(1600, 20)); // Position: x=50, y=50
                    ImGui::Begin("Rendered Image");
                    ImGui::Image((void*)(intptr_t)fbo.GetTextureID(), ImVec2(300, 300));
                    ImGui::End();
                }
                
                GLCall(glClearColor(0.529f, 0.828f, 0.952f, 1.0f));
                renderer.Clear();
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
                if(drawCircle)
                {
                    shader.Bind();
                    glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                    glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                    shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                    for(int i = 0; i < numGreatCircles; i++)
                    {
                        hopfs[i].Draw(&shader);
                    }
                }

                // CAMERA CONTROLS
                camera.SetFOV(fov);
                camera.setSpeed(speed);
                camera.setSensitivity(sensitivity);

                camera.ProcessControls();

                if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
                {
                    if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
                    {
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    }
                    else
                    {
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    }
                    lastKeyPressTime = time;
                }
                if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
                {
                    hideUi = !hideUi;
                    lastKeyPressTime = time;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
            {
                ChangeStates(menu, render);
                if(menu)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                lastKeyPressTime = time;
            }

            if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
            {
                fullscreen = !fullscreen; // Toggle the fullscreen flag
                if (fullscreen) {
                    // Store the window size to restore later
                    glfwGetWindowSize(window, &fullscreenWidth, &fullscreenHeight);

                    // Get the primary monitor's resolution
                    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

                    // Switch to fullscreen
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
                    glViewport(0, 0, mode->width, mode->height);
                    SetVsync(vsync);
                } 
                else 
                {
                    // Switch to windowed mode
                    glfwSetWindowMonitor(window, nullptr, 100, 100, windowedWidth, windowedHeight, GLFW_DONT_CARE);
                    glViewport(0, 0, windowedWidth, windowedHeight);
                    SetVsync(vsync);
                }
                lastKeyPressTime = time;
            }
>>>>>>> a8e6566ecf49eea1651fd1e600cbde840ec69ccf

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