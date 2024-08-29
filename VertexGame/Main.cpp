
#include "Main.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

#ifdef _DEBUG
int main() 
{
#else
int WinMain()
{
#endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "No more scoobert :(", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallBack); // for resizing window
    glfwSetCursorPosCallback(window, MouseCallBack);
    glfwSetScrollCallback(window, ScrollCallBack);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load the open gl function pointers

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    ShaderManager lightingShader("Shaders/lightingVS.glsl", "Shaders/lightingFS.glsl");
    ShaderManager lightCubeShader("Shaders/cubeVS.glsl", "Shaders/cubeFS.glsl");

    //buffers
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO; // VBO stays the same for now
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = MaterialManager::LoadTexture("TexturesSource/container2_diffuse.png");
    unsigned int specularMap = MaterialManager::LoadTexture("TexturesSource/container2_specular.png");

    lightingShader.Run();
    lightingShader.SetInt("material.diffuse", 0);
    lightingShader.SetInt("material.specular", 1);

    glEnable(GL_DEPTH_TEST);

    // main render loop

    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window); // TODO: replace with SDL

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.Run();

        lightingShader.SetVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        lightingShader.SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        lightingShader.SetVec3("material.specular", 0.5f, 0.5f, 0.5f);
        lightingShader.SetFloat("material.shininess", 32.0f);

        lightingShader.SetVec3("light.position", GlobalCamera.Position);
        lightingShader.SetVec3("light.direction", GlobalCamera.Front);
        lightingShader.SetFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.SetFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        lightingShader.SetVec3("viewPos", GlobalCamera.Position);

        lightingShader.SetVec3("light.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.SetVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.SetFloat("light.constant", 1.0f);
        lightingShader.SetFloat("light.linear", 0.09f);
        lightingShader.SetFloat("light.quadratic", 0.032f);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(GlobalCamera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, GlobalCamera.NearPlane, 100.0f);
        lightingShader.SetMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = GlobalCamera.GetViewMatrix();
        lightingShader.SetMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.SetMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.SetMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        /*lightCubeShader.Run();
        lightCubeShader.SetMat4("projection", projection);
        lightCubeShader.SetMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        lightCubeShader.SetMat4("model", model);*/

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all buffers
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        GlobalCamera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        GlobalCamera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        GlobalCamera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        GlobalCamera.ProcessKeyboard(RIGHT, deltaTime);
}

void MouseCallBack(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    GlobalCamera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
    GlobalCamera.ProcessMouseScroll(static_cast<float>(yoffset));
}