cmake_minimum_required(VERSION 3.10)
project(Hopf)
add_definitions(-DGLEW_STATIC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(SOURCES
    src/Camera.cpp
    src/Controls.cpp
    src/IndexBuffer.cpp
    src/main.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp
    src/FrameBuffer.cpp
    src/GlobalFunctions.cpp
    src/vendor/imgui/imgui.cpp
    src/vendor/imgui/imgui_demo.cpp
    src/vendor/imgui/imgui_draw.cpp
    src/vendor/imgui/imgui_impl_glfw_gl3.cpp
    src/vendor/stb_image/stb_image.cpp
    src/render_geom/CoordinateAxis/CoordinateAxis.cpp
    src/render_geom/Sphere/Sphere.cpp
    src/render_geom/Circle/Circle.cpp
    src/render_geom/Hopf/Hopf.cpp
    src/render_geom/Points/Points.cpp
)

set(INCLUDE_DIRS
    dependencies/include
    src/vendor
    src/render_geom/CoordinateAxis
    src/render_geom/Sphere
)

set(LIB_DIRS
    dependencies/lib
)

set(LIBRARIES
    glew32s
    glfw3
    opengl32
    gsl
    gslcblas
)

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE ${INCLUDE_DIRS})

target_link_directories(main PRIVATE ${LIB_DIRS})

target_link_libraries(main PRIVATE ${LIBRARIES})

set_property(TARGET main PROPERTY CXX_STANDARD 11)