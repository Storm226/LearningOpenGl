#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "Shapes.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

int setUp();
void cleanUp();

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
GLFWwindow* window;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Frustum stuff
float nearPlane = 0.01f;
float farPlane = 1000.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main()
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";
   
    // Compile shaders
    // ---------------
    Shader basicColor("Shaders/Basic/colors.vs", "Shaders/Basic/colors.fs");
    Shader basicLight("Shaders/Basic/lights.vs", "Shaders/Basic/lights.fs");


    // We use vertices and indices for index based drawing 
    // ---------------------------------------------------
    std::vector<float> vertices = Shapes::getSquarePyramid();
    std::vector<unsigned int> indices = {
        // Base (two triangles)
        0, 1, 2,
        2, 3, 0,
        // Sides (four triangles)
        0, 1, 4,  // Side 1
        1, 2, 4,  // Side 2
        2, 3, 4,  // Side 3
        3, 0, 4   // Side 4
    };

    // So, we have vertices, now we need to: 
    // 1) put them into a Vertex Buffer object to store the data
    // 2) configure a Vertex Array object to instruct the gpu how to interpret the data
    // 3) Render the data

    unsigned int VBO, VAO, EBO;
    size_t bufferSize = sizeof(float) * vertices.size();

    // Create and populate VBO
    // -----------------------
    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, bufferSize, vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Creates a VAO and then binds the VBO to the VAO
    // params -nBuffers, ptr->buffer
    // -----------------------------------------------
    glCreateVertexArrays(1, &VAO);

    // Here we tell openGL that the VBO is associated with
    // index = bindingIndex = 0 of the VAO
    // params - vaobj, bindingIndex, buffer, offset, stride
    // ----------------------------------------------------
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 8 * sizeof(float));

    // Now, Instruct the api how to use the vertex data
    // in the shader we set the layout variables, that is how
    // the shader knows at (location = 0), lies the position data.
    // ----------------------------------------------------------
    glEnableVertexArrayAttrib(VAO, 0);  // Positions
    glEnableVertexArrayAttrib(VAO, 1);  // Normals
    glEnableVertexArrayAttrib(VAO, 2);  // Texture Coordinates

    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
    
    // params - (vaobj, attribindex, binding index)
    // here we use bindingIndex = 0 for all calls because they all come from the same vao
    // It is also worth noting that these calls aren't strictly neccessary as the code is,
    // and that is because without explicitly defining these options openGL will assume
    // defaults.
    // ----------------------------------------------------------------------------------
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribBinding(VAO, 1, 0);
    glVertexArrayAttribBinding(VAO, 2, 0);


    // Create Element Buffer Object used for indexed drawing
    // -----------------------------------------------------
    glCreateBuffers(1, &EBO);

    // Copy indices into the EBO
    size_t indexBufferSize = sizeof(unsigned int) * indices.size();
    glNamedBufferStorage(EBO, indexBufferSize, indices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Bind the EBO to the VAO
    glVertexArrayElementBuffer(VAO, EBO);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // so now the vbo,vao and ebo should be setup. 

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

        // Don't forget to use the shader program
        basicColor.use();

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
      

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearPlane, farPlane);
       // glm::mat4 view = camera.GetViewMatrix();
        basicColor.setMat4("projection", projection);
     //   basicColor.setMat4("view", view);

        // world transformation
        //glm::mat4 model = glm::mat4(1.0f);
        //basicColor.setMat4("model", model);

        glBindVertexArray(VAO);
        for (int i = 0; i < 25; i++) {

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(i));
            model = glm::translate(model, glm::vec3(i + 5, 1, 0));
            basicColor.setMat4("model", model);


            glm::mat4 view = camera.GetViewMatrix();
            basicColor.setMat4("view", view);


            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        }



        /*glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
        basicColor.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);*/

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUp();
  
    return 0;
}

int setUp() {

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    return 1;
}

void cleanUp() {
    glfwTerminate();
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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

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


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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