
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
        std::cout << "GLFW : FAILED TO CREATE A WINDOW" << std::endl;
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
        std::cout << " GLAD : FAILED TO INITIALIZE" << std::endl;
        return -1;
    }

    // configure global states

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    // load imgui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
    Shader ModelShader("Shaders/modelVS.glsl", "Shaders/modelFS.glsl");
    Shader SkyboxShader("Shaders/skyboxVS.glsl", "Shaders/skyboxFS.glsl");

    Model ourModel("Assets/ModelsSource/backpack/backpack.obj");

    VAO skyboxVAO;
    skyboxVAO.Bind();
    VBO skyboxVBO(SkyboxVertices, sizeof(SkyboxVertices));
    
    skyboxVAO.LinkVBO(skyboxVBO, 0);
    skyboxVAO.Unbind();
    skyboxVBO.Unbind();


    unsigned int CubemapTexture = MaterialManager::LoadCubemap(SkyboxFaces, false);    

    SkyboxShader.Run();
    SkyboxShader.SetInt("skybox", 0);

    // main render loop

    while (!glfwWindowShouldClose(window))
    {
        float CurrentFrame = static_cast<float>(glfwGetTime());
        DeltaTime = CurrentFrame - LastFrame;
        FrameCount++;

        if (DeltaTime >= 1.0 / 60.0)
        {
            FPS = std::to_string((1.0 / DeltaTime) * FrameCount);
            MS = std::to_string((DeltaTime / FrameCount) * 1000);
            Metrics = "FPS: " + FPS + " MS: " + MS;

            LastFrame = CurrentFrame;
            FrameCount = 0;

            ProcessInput(window); // TODO: replace with SDL
        }

        
        if (MouseLockEnabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Metrics");
        ImGui::Text(Metrics.c_str());
        ImGui::End();

        //rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ModelShader.Run();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(GlobalCamera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = GlobalCamera.GetViewMatrix();
        ModelShader.SetMat4("projection", projection);
        ModelShader.SetMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ModelShader.SetMat4("model", model);
        ourModel.Draw(ModelShader);

        // draw the skybox after everything else
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        SkyboxShader.Run();
        view = glm::mat4(glm::mat3(GlobalCamera.GetViewMatrix())); // remove translation from the view matrix
        SkyboxShader.SetMat4("view", view);
        SkyboxShader.SetMat4("projection", projection);
        // skybox cube
        skyboxVAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default


        // render the imgui elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(FORWARD, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(BACKWARD, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(LEFT, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(RIGHT, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(UP, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        GlobalCamera.ProcessMovement(DOWN, DeltaTime);
}

void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        MouseLockEnabled = !MouseLockEnabled;
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

