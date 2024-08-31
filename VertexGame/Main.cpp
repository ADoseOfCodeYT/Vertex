
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

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WindowTitle, NULL, NULL);
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
    glfwSetKeyCallback(window, KeyCallBack);
    if (MouseLockEnabled)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    // load the open gl function pointers

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // load imgui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
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
        
        if (MouseLockEnabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if (WireframeEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("wow a window");
        ImGui::Text("idk what to do with it");
        ImGui::End();

        lightingShader.Run();
        lightingShader.SetVec3("viewPos", GlobalCamera.Position);
        lightingShader.SetFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // directional light
        lightingShader.SetVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.SetVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.SetVec3("pointLights[0].position", PointLightPositions[0]);
        lightingShader.SetVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.SetVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.SetVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.SetFloat("pointLights[0].constant", 1.0f);
        lightingShader.SetFloat("pointLights[0].linear", 0.09f);
        lightingShader.SetFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.SetVec3("pointLights[1].position", PointLightPositions[1]);
        lightingShader.SetVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.SetVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.SetVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.SetFloat("pointLights[1].constant", 1.0f);
        lightingShader.SetFloat("pointLights[1].linear", 0.09f);
        lightingShader.SetFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.SetVec3("pointLights[2].position", PointLightPositions[2]);
        lightingShader.SetVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.SetVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.SetVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.SetFloat("pointLights[2].constant", 1.0f);
        lightingShader.SetFloat("pointLights[2].linear", 0.09f);
        lightingShader.SetFloat("pointLights[2].quadratic", 0.032f);
        // spotLight
        lightingShader.SetVec3("spotLight.position", GlobalCamera.Position);
        lightingShader.SetVec3("spotLight.direction", GlobalCamera.Front);
        lightingShader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.SetFloat("spotLight.constant", 1.0f);
        lightingShader.SetFloat("spotLight.linear", 0.09f);
        lightingShader.SetFloat("spotLight.quadratic", 0.032f);
        lightingShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(GlobalCamera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = GlobalCamera.GetViewMatrix();
        lightingShader.SetMat4("projection", projection);
        lightingShader.SetMat4("view", view);

        // world transformation
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

        // also draw the lamp object(s)
        lightCubeShader.Run();
        lightCubeShader.SetMat4("projection", projection);
        lightCubeShader.SetMat4("view", view);

        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, PointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // render the imgui elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all buffers
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !IsCtrlDown)
        GlobalCamera.ProcessMovement(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(RIGHT, deltaTime);
}

void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        MouseLockEnabled = !MouseLockEnabled;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS && IsCtrlDown)
    {
        WireframeEnabled = !WireframeEnabled;
    }
    if (key == GLFW_KEY_LEFT_CONTROL) IsCtrlDown = action == GLFW_PRESS;

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
    }
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

