

#ifndef SHADERMANAGER
#define SHADERMANAGER
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Math.h"

// REMEBER: TO DEBUG SHADER RELATED ERRORS, CHECK THE CONSOLE IN DEBUG MODE
class ShaderManager
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    ShaderManager(const char* vertexPath, const char* fragmentPath);
    
    // activate the shader
    void Run();

    // utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec2(const std::string& name, float x, float y) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w) const;
    void SetMat2(const std::string& name, const glm::mat2& mat) const;
    void SetMat3(const std::string& name, const glm::mat3& mat) const;
    void SetMat4(const std::string& name, const glm::mat4& mat) const;


private:
    void CheckForCompileErrors(unsigned int shader, std::string type);   
};
#endif