//*
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "ParticleGenerator.h"

#include <iostream>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
void RenderModelAndSkyBox(Camera camera, Shader modelShader, Model ourModel, Shader lampShader, Shader skyboxShader, Shader frameBufferShader, unsigned int VAOs[3], unsigned int textures[2]);
void RenderSceneForDepthMap(Shader shader, Model model_1, glm::vec3 model_Pos1, Model model_2, glm::vec3 model_Pos2, unsigned int VAO);
void CheckGLError(int idx);
void renderQuad();
void renderSmallQuad(float size = 1, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const unsigned int SKYBOX_WIDTH = 1280; // ��̬������ͼ��С
bool hdrOpen = true;
bool hdrKeyPressed = false;
float exposure = 1.0f;

float lightMoveTime = 0.0f;
bool lightMoving = true;
bool lightMoveKeyPressed = false;

// camera
Camera camera(glm::vec3(-3.51f, -0.45f, 6.24f), glm::vec3(0.0f, 1.0f, 0.0f), -46.9f, -1.8f);
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int currentFrameInt = 0;

// lighting
glm::vec3 lightPos(1.2f, 0.6f, 2.0f);
float radius = 2.0f;

// pos
glm::vec3 reflectCubePos(2.0f, -1.3f, 0.0f);
glm::vec3 modelPos(-1.0f, -2.55f, 0.0f);

// statistics
int fps = 0;

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader shader("..\\LearnOpenGL_Res\\ZXShaders\\cubeMapWithLighting.zxshader");
    Shader skyboxShader("..\\LearnOpenGL_Res\\Shaders\\skyBox.vs", "..\\LearnOpenGL_Res\\Shaders\\skyBox.fs");
    Shader modelShader("..\\LearnOpenGL_Res\\Shaders\\cubeMapReflectAndLight.vs", "..\\LearnOpenGL_Res\\Shaders\\cubeMapReflectAndLight.fs");
    Shader lampShader("..\\LearnOpenGL_Res\\Shaders\\lamp.vs", "..\\LearnOpenGL_Res\\Shaders\\lamp.fs");
    Shader textureShader("..\\LearnOpenGL_Res\\ZXShaders\\simpleTexture.zxshader");
    Shader pointShadowShader("..\\LearnOpenGL_Res\\ZXShaders\\pointShadow.zxshader");
    Shader shadowDepthShader("..\\LearnOpenGL_Res\\ZXShaders\\pointShadowDepth.zxshader");
    Shader hdrShader("..\\LearnOpenGL_Res\\ZXShaders\\HDR.zxshader");
    Shader filterBrightAreaShader("..\\LearnOpenGL_Res\\ZXShaders\\filterBrightArea.zxshader");
    Shader blurShader("..\\LearnOpenGL_Res\\ZXShaders\\gaussianBlur.zxshader");
    Shader mixShader("..\\LearnOpenGL_Res\\ZXShaders\\mixTexture.zxshader");
    Shader particleShader("..\\LearnOpenGL_Res\\ZXShaders\\particle.zxshader");
    Shader gBufferShader("..\\LearnOpenGL_Res\\ZXShaders\\gBuffer.zxshader");
    // Shader gBufferShader("..\\LearnOpenGL_Res\\Shaders\\SSAOGeometry.vs", "..\\LearnOpenGL_Res\\Shaders\\SSAOGeometry.fs");
    Shader colorBufferShader("..\\LearnOpenGL_Res\\ZXShaders\\showColorBuffer.zxshader");
    Shader greyColorBufferShader("..\\LearnOpenGL_Res\\ZXShaders\\showGreyColorBuffer.zxshader");
    Shader ssaoShader("..\\LearnOpenGL_Res\\ZXShaders\\SSAO.zxshader");
    Shader ssaoBlurShader("..\\LearnOpenGL_Res\\ZXShaders\\SSAOBlur.zxshader");

    // load models
    // -----------
    Model ourModel("..\\LearnOpenGL_Res\\Models\\nanosuit_reflection\\nanosuit.obj");
    Model lamborghini("..\\LearnOpenGL_Res\\Models\\Lamborghini\\huracan in 2.8.obj");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    float planeVertices[] = {
        // positions          // normals          // texture Coords
         5.0f, -2.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
        -5.0f, -2.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -5.0f, -2.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,

         5.0f, -2.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
        -5.0f, -2.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
         5.0f, -2.5f, -5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f
    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // load textures
    // -------------
    vector<std::string> faces
    {
        "..\\LearnOpenGL_Res\\Textures\\skybox3\\corona_rt2.png", // ��
        "..\\LearnOpenGL_Res\\Textures\\skybox3\\corona_lf2.png", // ��
        "..\\LearnOpenGL_Res\\Textures\\skybox3\\corona_dn2.png", // �ϣ�ע�������õ���պ����·���һ�£�����ǰ�����ҵ�ͼ��ת��180�ȵ�
        "..\\LearnOpenGL_Res\\Textures\\skybox3\\corona_up2.png", // ��
        "..\\LearnOpenGL_Res\\Textures\\skybox3\\corona_ft2.png", // ǰ
        "..\\LearnOpenGL_Res\\Textures\\skybox3\\corona_bk2.png", // ��
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    unsigned int floorTexture = loadTexture("..\\LearnOpenGL_Res\\Textures\\wall.jpg");
    unsigned int particleTexture = loadTexture("..\\LearnOpenGL_Res\\Textures\\particle.png");

    // Particle System
    // ---------------
    ParticleGenerator* Particles = new ParticleGenerator(particleShader, particleTexture, 500, camera.Position);

    // ����һ���߾��ȸ�������֡����������HDR(High Dynamic Range)��Ⱦ
    // -------------------------------------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    // create floating point color buffer
    unsigned int hdrColorBuffer;
    glGenTextures(1, &hdrColorBuffer);
    glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ����һ�����ڶ�̬����ӳ�䣨Dynamic Environment Mapping����FBO
    // ------------------------------------------------------------
    unsigned int dynamicEnvMapFBO;
    glGenFramebuffers(1, &dynamicEnvMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, dynamicEnvMapFBO);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SKYBOX_WIDTH, SKYBOX_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SKYBOX_WIDTH, SKYBOX_WIDTH); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ����һ����Ⱦ���ͼ��FBO������Ⱦ��Ӱ׼������
    // -------------------------------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ����һ��֡�������ڴ�Ŵ�HDR����������ȡ�����ĸ�������ͼ����Ϊ�ǴӸ߾��ȵ�HDR���������ȡ������Ⱦ���������������Ҳ�Ǹ߾��ȵģ�����ᶪʧϸ��
    // ---------------------------------------------------------
    unsigned int brightFBO;
    glGenFramebuffers(1, &brightFBO);
    // ֻ������ɫ���壬�����ò�����Ȳ��ԾͲ�������Ȼ�����
    glBindFramebuffer(GL_FRAMEBUFFER, brightFBO);
    unsigned int brightColorBuffer;
    glGenTextures(1, &brightColorBuffer);
    glBindTexture(GL_TEXTURE_2D, brightColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brightColorBuffer, 0);
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

    // ����һ����������˹ģ�������֡����
    // ---------------------------------
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    // �����ӳ���Ⱦ�� G-Buffer
    // -----------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedo;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int gDepth;
    glGenRenderbuffers(1, &gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // �������ڴ���SSAO��֡����
    // ------------------------
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate sample kernel for SSAO
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // ����һЩShader�Ĺ̶�Uniform
    colorBufferShader.use();
    colorBufferShader.setInt("screenTexture", 0);
    greyColorBufferShader.use();
    greyColorBufferShader.setInt("screenTexture", 0);
    ssaoShader.use();
    ssaoShader.setInt("gPosition", 0);
    ssaoShader.setInt("gNormal", 1);
    ssaoShader.setInt("texNoise", 2);
    ssaoBlurShader.use();
    ssaoBlurShader.setInt("ssaoInput", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // ------------------------------------------------ �߼����� ------------------------------------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (currentFrameInt == int(currentFrame))
        {
            fps += 1;
        }
        else
        {
            cout << "FPS: " << fps << endl;
            currentFrameInt += 1;
            fps = 0;
        }

        // ��̬�����Դλ�ã�����Χ��ģ��תȦ
        if (lightMoving)
        {
            lightMoveTime += deltaTime;
        }
        float light_x = glm::sin(lightMoveTime) * radius;
        float light_z = glm::cos(lightMoveTime) * radius;
        lightPos = glm::vec3(light_x, 0.6f, light_z);

        // ��������ϵͳ(����X��Y����0.1��Ϊ�������Ӳ����ĵط��ڹ�Դ�����������λ��)
        Particles->Update(deltaTime, camera.Position, lightPos, glm::vec3(0.0f, 5.0f, 0.0f), 2, glm::vec3(0.01f));

        // input
        // -----
        processInput(window);

        // ------------------------------------------------ ��Ⱦ���� ------------------------------------------------
        // ׼����Ⱦ���CubeMap������
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // �л������ͼ֡���壬��Ⱦ���CubeMap
        // -----------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        shadowDepthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            shadowDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        shadowDepthShader.setFloat("far_plane", far_plane);
        shadowDepthShader.setVec3("lightPos", lightPos);
        RenderSceneForDepthMap(shadowDepthShader, ourModel, modelPos, lamborghini, reflectCubePos, planeVAO);


        // �л����Զ����֡���壬׼����Ⱦ��̬�Ļ���CubeMap
        // -----------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, dynamicEnvMapFBO);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // �½�һ��CubeMap
        unsigned int dynamicCubeMap;
        glGenTextures(1, &dynamicCubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        // ��̬����ӳ��(Dynamic Environment Mapping)���������������WorldUp�������ˣ���Ȼ�Ļ����ɵ�CubeMap�ǵ���
        Camera DEMCamera(reflectCubePos, glm::vec3(0.0f, -1.0f, 0.0f));
        // ����һ��ָ��׼���洢��֡�����������ͼ�����ݣ���Ϊ���õ�GL_RGB����������������*3
        GLubyte* colorBufferData = (GLubyte*)malloc(SKYBOX_WIDTH * SKYBOX_WIDTH * 3);
        // ��ʼ���Զ����֡��������Ⱦ

        unsigned int VAOs[3] = {cubeVAO, skyboxVAO, planeVAO};
        unsigned int textures[3] = {cubemapTexture, floorTexture, depthCubemap};

        // ���������������Ⱦ��һ����
        DEMCamera.Yaw = 180;
        DEMCamera.Pitch = 0;
        DEMCamera.updateCameraVectors();
        RenderModelAndSkyBox(DEMCamera, modelShader, ourModel, lampShader, skyboxShader, pointShadowShader, VAOs, textures);
        CheckGLError(1);
        // ���Դ��ϰѵ�ǰ֡���������Ⱦ�õ�ͼ���ȡ���������Դ�����ڴ棩�������û����������Ӧ�þ��������ֶ����õ�֡���������ɫ������textureColorbuffer
        glReadPixels(0, 0, SKYBOX_WIDTH, SKYBOX_WIDTH, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);
        CheckGLError(2);
        // �л��ض�̬CubeMap����RenderModelAndSkyBox������GL_TEXTURE_CUBE_MAP���л���֮ǰ����պ���
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        // ���õ�һ���棬��ΪCubeMap����ͼ��ʱ��ͼ��ĸߺͿ�һ��Ҫ����ȵģ�����
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, SKYBOX_WIDTH, SKYBOX_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);

        DEMCamera.Yaw = 0;
        DEMCamera.Pitch = 0;
        DEMCamera.updateCameraVectors();
        RenderModelAndSkyBox(DEMCamera, modelShader, ourModel, lampShader, skyboxShader, pointShadowShader, VAOs, textures);
        glReadPixels(0, 0, SKYBOX_WIDTH, SKYBOX_WIDTH, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, SKYBOX_WIDTH, SKYBOX_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);

        DEMCamera.Yaw = 0;
        DEMCamera.Pitch = -90;
        DEMCamera.updateCameraVectors();
        RenderModelAndSkyBox(DEMCamera, modelShader, ourModel, lampShader, skyboxShader, pointShadowShader, VAOs, textures);
        glReadPixels(0, 0, SKYBOX_WIDTH, SKYBOX_WIDTH, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, SKYBOX_WIDTH, SKYBOX_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);

        DEMCamera.Yaw = 0;
        DEMCamera.Pitch = 90;
        DEMCamera.updateCameraVectors();
        RenderModelAndSkyBox(DEMCamera, modelShader, ourModel, lampShader, skyboxShader, pointShadowShader, VAOs, textures);
        glReadPixels(0, 0, SKYBOX_WIDTH, SKYBOX_WIDTH, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, SKYBOX_WIDTH, SKYBOX_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);

        DEMCamera.Yaw = -90;
        DEMCamera.Pitch = 0;
        DEMCamera.updateCameraVectors();
        RenderModelAndSkyBox(DEMCamera, modelShader, ourModel, lampShader, skyboxShader, pointShadowShader, VAOs, textures);
        glReadPixels(0, 0, SKYBOX_WIDTH, SKYBOX_WIDTH, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, SKYBOX_WIDTH, SKYBOX_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);

        DEMCamera.Yaw = 90;
        DEMCamera.Pitch = 0;
        DEMCamera.updateCameraVectors();
        RenderModelAndSkyBox(DEMCamera, modelShader, ourModel, lampShader, skyboxShader, pointShadowShader, VAOs, textures);
        glReadPixels(0, 0, SKYBOX_WIDTH, SKYBOX_WIDTH, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, SKYBOX_WIDTH, SKYBOX_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBufferData);

        CheckGLError(3);
        // malloc���ڴ��ù�֮��һ��һ��һ��Ҫ�ǵ�free������
        std::free(colorBufferData);
        // ����CubeMap�������Ʒ�ʽ
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



        // �任�����ʼ��
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


        // �л����ӳ���Ⱦ֡���壬������д�� G-Buffer
        // -----------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(-1000.0f, -1000.0f, -1000.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        gBufferShader.use();
        gBufferShader.setMat4("view", view);
        gBufferShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPos);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        gBufferShader.setMat4("model", model);
        ourModel.Draw(gBufferShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, reflectCubePos);
        gBufferShader.setMat4("model", model);
        lamborghini.Draw(gBufferShader);

        model = glm::mat4(1.0f);
        gBufferShader.setMat4("model", model);
        gBufferShader.setInt("texture_diffuse1", 0);
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        // �е�SSAO������ȾSSAO��ͼ
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssaoShader.use();
        // Send kernel + rotation 
        for (unsigned int i = 0; i < 64; ++i)
            ssaoShader.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        ssaoShader.setMat4("view", view);
        ssaoShader.setMat4("projection", projection);
        ssaoShader.setVec3("camPos", camera.Position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        renderQuad();

        // ģ��SSAO�����SSAO������
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssaoBlurShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderQuad();


        // �л���HDR֡���壬��ʼ��ȾHDR�µ����ӽǻ���
        // ------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ��Ⱦ����������ֵ�ģ��
        shader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, reflectCubePos);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.Position);
        shader.setInt("skybox", 0);
        shader.setFloat("shininess", 32.0f);
        // ƽ�й�
        shader.setVec3("dirLight.direction", 0.5f, -1.0f, -0.3f);
        shader.setVec3("dirLight.ambient", 0.6f, 0.6f, 0.6f);
        shader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        // ���Դ
        shader.setVec3("pointLight.position", lightPos);
        shader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLight.constant", 1.0f);
        shader.setFloat("pointLight.linear", 0.09);
        shader.setFloat("pointLight.quadratic", 0.032);
        // cubes
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeMap);
        lamborghini.Draw(shader);

        // glGenTextures�����������һֱ���ڴ�����������ѭ���д������������ã�������while�ﶯ̬����������һ��Ҫ�ֶ�delete������һ���ڴ������
        glDeleteTextures(1, &dynamicCubeMap);

        // ��Ⱦ���ص�ģ��
        modelShader.use();
        modelShader.setVec3("camPos", camera.Position);
        modelShader.setFloat("shininess", 32.0f);

        // ƽ�й�
        modelShader.setVec3("dirLight.direction", 0.5f, -1.0f, -0.3f);
        modelShader.setVec3("dirLight.ambient", 0.6f, 0.6f, 0.6f);
        modelShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        // ���Դ
        modelShader.setVec3("pointLight.position", lightPos);
        modelShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("pointLight.constant", 1.0f);
        modelShader.setFloat("pointLight.linear", 0.09);
        modelShader.setFloat("pointLight.quadratic", 0.032);

        // ��������һ��ģ�͵�λ�ú�����
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPos);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        modelShader.setMat4("model", model);
        modelShader.setMat4("view", view);
        modelShader.setMat4("projection", projection);

        // ����������պе�CubeMap����Ϊ��ģ�����Ѿ�����4�������ˣ�����������5
        modelShader.setInt("skybox", 5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        // draw the model
        ourModel.Draw(modelShader);

        // �ѵ��Դ���Ƴ����������������и������գ���ʵ������Ҳ��Ӱ��ģ���ϼ������
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        lampShader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // floor
        glBindVertexArray(planeVAO);
        pointShadowShader.use();
        pointShadowShader.setMat4("model", glm::mat4(1.0f));
        pointShadowShader.setMat4("view", view);
        pointShadowShader.setMat4("projection", projection);
        // set lighting uniforms
        pointShadowShader.setVec3("lightPos", lightPos);
        pointShadowShader.setVec3("viewPos", camera.Position);
        pointShadowShader.setInt("shadows", true); // enable/disable shadows by pressing 'SPACE'
        pointShadowShader.setFloat("far_plane", far_plane);
        pointShadowShader.setInt("diffuseTexture", 0);
        pointShadowShader.setInt("depthMap", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // ������պ�
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setInt("skybox", 0);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // ����������ϵͳ����Ϊ����ϵͳ�а�͸������Ⱦ
        particleShader.use();
        // ֮ǰ������պе�ʱ���view�������ƽ��ɾ���ˣ����ڱ��������
        view = camera.GetViewMatrix();
        particleShader.setMat4("view", view);
        particleShader.setMat4("projection", projection);
        particleShader.setInt("sprite", 0);
        Particles->Draw();

        // �л�����������֡���壬��ȡHDR��ɫ�����еĸ�������
        // -------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, brightFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        filterBrightAreaShader.use();
        filterBrightAreaShader.setInt("colorBuffer", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
        renderQuad();

        // ����ȡ�����ĸ�����������˹ģ������
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 40;
        blurShader.use();
        blurShader.setInt("image", 0);
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? brightColorBuffer : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }

        // ��HDR֡��������Ⱦ�����ӽǻ��棬�͸�˹ģ�������ĸ�������Ҳ���Ƿ���BloomЧ������ϵ�һ��
        // ����Ϊ�˲������ٶ�����һ��������ɫ��ϵ�֡���壬����֮ǰ��˹ģ���е�һ��֡��������Ⱦ��
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        mixShader.use();
        mixShader.setFloat("addTimes", 4);
        mixShader.setInt("originColor", 0);
        mixShader.setInt("addColor", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);// ֮ǰ��˹ģ��������֮��Ľ���������ɫ������
        renderQuad();

        // �л���Ĭ��֡���壬��HDR����������ӽǻ�����Ⱦ��������Ļ���ı����ϣ�����HDRӳ�䵽�����LDR
        // -----------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        hdrShader.setInt("hdrBuffer", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[horizontal]);
        hdrShader.setInt("hdr", hdrOpen);
        hdrShader.setFloat("exposure", exposure);
        renderQuad();

        // ��ȾһЩ���Ի���
        colorBufferShader.use();
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, brightColorBuffer);
        renderSmallQuad(0.2, glm::vec3(-0.75f, 0.75f, 0.0f));

        glBindTexture(GL_TEXTURE_2D, gPosition);
        renderSmallQuad(0.2, glm::vec3(-0.75f, 0.25f, 0.0f));

        glBindTexture(GL_TEXTURE_2D, gNormal);
        renderSmallQuad(0.2, glm::vec3(-0.75f, -0.25f, 0.0f));

        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        renderSmallQuad(0.2, glm::vec3(-0.75f, -0.75f, 0.0f));

        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderSmallQuad(0.2, glm::vec3(0.75f, 0.75f, 0.0f));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVAO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        modelPos.z += deltaTime * 2;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        modelPos.z -= deltaTime * 2;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        modelPos.x += deltaTime * 2;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        modelPos.x -= deltaTime * 2;

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !hdrKeyPressed)
    {
        hdrOpen = !hdrOpen;
        hdrKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
    {
        hdrKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !lightMoveKeyPressed)
    {
        lightMoving = !lightMoving;
        lightMoveKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
    {
        lightMoveKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
            exposure -= 0.1f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        exposure += 0.1f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// ��⵱ǰ�Ƿ��д���
void CheckGLError(int idx)
{
    GLenum error = glGetError();
    if (error == GL_NO_ERROR)
    {
        return;
    }
    else if (error == GL_INVALID_ENUM)
    {
        std::cout << "ZXTest " << idx << " GL_INVALID_ENUM" << std::endl;
    }
    else if (error == GL_INVALID_VALUE)
    {
        std::cout << "ZXTest " << idx << " GL_INVALID_VALUE" << std::endl;
    }
    else if (error == GL_INVALID_OPERATION)
    {
        std::cout << "ZXTest " << idx << " GL_INVALID_OPERATION" << std::endl;
    }
    else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
    {
        std::cout << "ZXTest " << idx << " GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
    }
    else if (error == GL_OUT_OF_MEMORY)
    {
        std::cout << "ZXTest " << idx << " GL_OUT_OF_MEMORY" << std::endl;
    }
    else if (error == GL_STACK_UNDERFLOW)
    {
        std::cout << "ZXTest " << idx << " GL_STACK_UNDERFLOW" << std::endl;
    }
    else if (error == GL_STACK_OVERFLOW)
    {
        std::cout << "ZXTest " << idx << " GL_STACK_OVERFLOW" << std::endl;
    }
    else
    {
        std::cout << "ZXTest " << idx << " OTHER_ERROR" << std::endl;
    }
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // ����ĵ������ᣬ������ͨ�������z����Ϊcubemap��3D������

    return textureID;
}

// ����һ���ض���������ڲ�׽����Ƕȹ۲쵽�ĳ�����Ȼ��������̬������ͼ
void RenderModelAndSkyBox(Camera camera, Shader modelShader, Model ourModel, Shader lampShader, Shader skyboxShader, Shader pointShadowShader, unsigned int VAOs[3], unsigned int textures[2])
{
    glViewport(0, 0, SKYBOX_WIDTH, SKYBOX_WIDTH);

    // render
    // ------
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // �任�����ʼ��
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom * 2), (float)SKYBOX_WIDTH / (float)SKYBOX_WIDTH, 0.1f, 100.0f);

    // ��Ⱦ���ص�ģ��
    modelShader.use();
    modelShader.setVec3("camPos", camera.Position);
    modelShader.setFloat("shininess", 32.0f);

    // ƽ�й�
    modelShader.setVec3("dirLight.direction", 0.5f, -1.0f, -0.3f);
    modelShader.setVec3("dirLight.ambient", 0.6f, 0.6f, 0.6f);
    modelShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
    modelShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    // ���Դ
    modelShader.setVec3("pointLight.position", lightPos);
    modelShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
    modelShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
    modelShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
    modelShader.setFloat("pointLight.constant", 1.0f);
    modelShader.setFloat("pointLight.linear", 0.09);
    modelShader.setFloat("pointLight.quadratic", 0.032);

    // ��������һ��ģ�͵�λ�ú�����
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPos);
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    modelShader.setMat4("model", model);
    modelShader.setMat4("view", view);
    modelShader.setMat4("projection", projection);

    // ����������պе�CubeMap����Ϊ��ģ�����Ѿ�����4�������ˣ�����������4
    modelShader.setInt("skybox", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0]);

    // draw the model
    ourModel.Draw(modelShader);

    // �ѵ��Դ���Ƴ����������������и������գ���ʵ������Ҳ��Ӱ��ģ���ϼ������
    lampShader.use();
    lampShader.setMat4("projection", projection);
    lampShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
    lampShader.setMat4("model", model);
    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // floor
    glBindVertexArray(VAOs[2]);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    pointShadowShader.use();
    pointShadowShader.setMat4("model", glm::mat4(1.0f));
    pointShadowShader.setMat4("view", view);
    pointShadowShader.setMat4("projection", projection);
    // set lighting uniforms
    pointShadowShader.setVec3("lightPos", lightPos);
    pointShadowShader.setVec3("viewPos", camera.Position);
    pointShadowShader.setInt("shadows", true); // enable/disable shadows by pressing 'SPACE'
    pointShadowShader.setFloat("far_plane", 25.0f);
    pointShadowShader.setInt("diffuseTexture", 0);
    pointShadowShader.setInt("depthMap", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures[2]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    CheckGLError(4);
    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setInt("skybox", 0);
    // skybox cube
    glBindVertexArray(VAOs[1]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
    CheckGLError(5);
}

void RenderSceneForDepthMap(Shader shader, Model model_1, glm::vec3 model_Pos1, Model model_2, glm::vec3 model_Pos2, unsigned int VAO)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, model_Pos1);
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setMat4("model", model);
    model_1.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, model_Pos2);
    shader.setMat4("model", model);
    model_2.Draw(shader);

    glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// ����Ļ����Ⱦһ��С��������չʾһЩͼ��
// --------------------------------------
unsigned int smallQuadVAO;
unsigned int smallQuadVBO;
void renderSmallQuad(float size, glm::vec3 offset)
{
    float quadVertices[] = {
        // positions                                             // texture Coords
        -1.0f * size + offset.x,  1.0f * size + offset.y, -0.1f, 0.0f, 1.0f,
        -1.0f * size + offset.x, -1.0f * size + offset.y, -0.1f, 0.0f, 0.0f,
         1.0f * size + offset.x,  1.0f * size + offset.y, -0.1f, 1.0f, 1.0f,
         1.0f * size + offset.x, -1.0f * size + offset.y, -0.1f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &smallQuadVAO);
    glGenBuffers(1, &smallQuadVBO);
    glBindVertexArray(smallQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, smallQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glBindVertexArray(smallQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// ��Ⱦģ��
void RenderModel(Camera camera, Shader shader)
{

}
//*/