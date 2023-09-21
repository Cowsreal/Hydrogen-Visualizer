#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.hpp"

Shader::Shader(const std::string& filename)
	: m_FilePath(filename), m_RendererID(0)
{

    ShaderProgramSource source = ParseShader(m_FilePath); //get the shader 
    m_RendererID = CreateShader(source.vertexSource, source.fragmentSource); //create a shader
    GLCall(glUseProgram(m_RendererID));
}
Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID)); //delete the shader
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);    //open the file

    enum class shaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;   //create a string to store the line
    std::stringstream stringStream[2];  //create a string stream to store the shader code
    shaderType type = shaderType::NONE; //create a variable to store the type of shader
    while (getline(stream, line))  //while there is a line to get
    {
        if (line.find("#shader") != std::string::npos) //if the line contains the word "shader"
        {
            if (line.find("vertex") != std::string::npos) //if the line contains the word "vertex"
            {
                type = shaderType::VERTEX;  //set mode to vertex
            }
            else if (line.find("fragment") != std::string::npos) //if the line contains the word "fragment"
            {
                type = shaderType::FRAGMENT;    //set mode to fragment
            }
        }
        else
        {
            stringStream[(int)type] << line << '\n'; //add the line to the string stream
        }
    }
    return { stringStream[0].str(), stringStream[1].str() }; //return the shader code
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();   //get the source code of the shader
    GLCall(glShaderSource(id, 1, &src, nullptr));   //set the source code of the shader
    GLCall(glCompileShader(id));    //compile the shader

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result)); //get the result of the compilation
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));   //get the length of the error message
        char* message = (char*)alloca(length * sizeof(char));  //allocate memory for the error message
        GLCall(glGetShaderInfoLog(id, length, &length, message));  //get the error message
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader, " << m_FilePath << std::endl; //print the error message
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id)); //delete the shader
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // create a shader program
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));

    GLint program_linked;

    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &program_linked));
    std::cout << "Program link status: " << program_linked << ", " << m_FilePath << std::endl;
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        GLCall(glGetProgramInfoLog(program, 1024, &log_length, message));
        std::cout << "Failed to link program, " << m_FilePath << std::endl;
        std::cout << message << std::endl;
    }

    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID)); //use the shader
}
void Shader::Unbind() const
{
    GLCall(glUseProgram(0)); //unbind the shader
}

//Set uniforms
void Shader::SetUniform1i(const std::string& name, int v0)
{
	GLCall(glUniform1i(GetUniformLocation(name), v0)); //set the uniform
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	GLCall(glUniform1f(GetUniformLocation(name), v0)); //set the uniform

}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2)); //set the uniform
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3)); //set the uniform)
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0])); //set the uniform
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name]; //return the location of the uniform if it is already in the cache
    }
    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str())); //get the location of the uniform
    if (location == -1)
    {
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl; //print a warning if the uniform doesn't exist"
    }
    m_UniformLocationCache[name] = location; //add the uniform to the 
    return location;
}