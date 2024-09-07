#include <glad/glad.h>
#include <GLFW/glfw3.h>

// glm math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "stb_image.h"
#include "Camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);  
void generateTexture(std::string filename, unsigned int& textureName, bool alpha);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLFWwindow* window;

int main()
{

    // Start up stuff
    // -----------------------------
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // glfw window creation
        // --------------------
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    }
    // -----------------------------

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader colorShader("colors.vs", "colors.fs");
    Shader lightingShader("light.vs", "light.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
   // ------------------------------------------------------------------
    float vertices[] = {

    // 3x1 position, 2x1 texture, 3x1 normals
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,     0.0f, 0.0f, -1.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,     0.0f, 0.0f, 1.0f,  
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     0.0f, 0.0f, 1.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,

       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     1.0f, 0.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     0.0f, - 1.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     0.0f, -1.0f, 0.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,     0.0f, 1.0f, 0.0f
    };
    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    }; 

    unsigned int cubeVAO, lightVAO, VBO;

    // VERTEX ARRAY OBJECT  : Dictates data retrieval/usage
    // VERTEX BUFFER OBJECT : Holds vertices
    
    // ask openGL for vertex arrays and the vertex buffer
    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);

    // bind the vertex buffer and populate with data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    glBindVertexArray(lightVAO); // bind so you can configure
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // WIREFRAME MODE
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   

    unsigned int texture1, texture2;
     //Texture creation
    generateTexture("math.jpg", texture1, false);
    generateTexture("huh.jpg", texture2, false);
    colorShader.use();
    colorShader.setInt("texture1", 0);
    colorShader.setInt("texture2", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
         // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        colorShader.use();
        colorShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        colorShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        colorShader.setVec3("viewPos", camera.Position);
        colorShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        colorShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        colorShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        colorShader.setFloat("material.shininess", 32.0f);
        colorShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        colorShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
        colorShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        colorShader.setVec3("light.ambient", ambientColor);
        colorShader.setVec3("light.diffuse", diffuseColor);

        // change the light's position values over time (can be done anywhere in the render loop actually, but try to do it at least before using the light source positions)
        lightPos.x = 9.0f + sin(glfwGetTime()) * 7.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

        colorShader.setVec3("lightPos", lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        colorShader.setMat4("projection", projection);
        colorShader.setMat4("view", view);

        glBindVertexArray(cubeVAO);

        // world transformation
        glm::mat4 model;

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 7; j++) {
                model = glm::mat4(1.0f);
                model = translate(model, glm::vec3((j + 1) * 2.5f, (i + 1) * 2.5f, 0));  // Add spacing between cubes
                float angle = glfwGetTime() * glm::radians(15.0f);
                model = rotate(model, angle, glm::vec3(1.0f, (float) i, 0.0f));
                colorShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            
        }
        // also draw the lamp object
        lightingShader.use();

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightingShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Given a filename, a texture name, and t/f flag for alpha, 
// initializes tecture name = texture(filename);
// -------------------------------------------------
void generateTexture(std::string filename, unsigned int& textureName, bool alpha) {
    glGenTextures(1, &textureName);
    glBindTexture(GL_TEXTURE_2D, textureName);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load((filename).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        if (!alpha) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}