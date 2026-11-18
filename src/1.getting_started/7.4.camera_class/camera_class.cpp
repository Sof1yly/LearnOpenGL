#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(6.0f, 3.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -119.7f, -8.5f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("7.4.camera.vs", "7.4.camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {

        -0.5f, -0.5f, -0.5f,  0.255f, 0.338f,
         0.5f, -0.5f, -0.5f,  0.495f, 0.338f,
         0.5f,  0.5f, -0.5f,  0.495f, 0.661f,
         0.5f,  0.5f, -0.5f,  0.495f, 0.661f,
        -0.5f,  0.5f, -0.5f,  0.255f, 0.661f,
        -0.5f, -0.5f, -0.5f,  0.255f, 0.338f,


        -0.5f, -0.5f,  0.5f,  0.005f, 0.338f,
         0.5f, -0.5f,  0.5f,  0.245f, 0.338f,
         0.5f,  0.5f,  0.5f,  0.245f, 0.661f,
         0.5f,  0.5f,  0.5f,  0.245f, 0.661f,
        -0.5f,  0.5f,  0.5f,  0.005f, 0.661f,
        -0.5f, -0.5f,  0.5f,  0.005f, 0.338f,


        -0.5f,  0.5f,  0.5f,  0.745f, 0.661f,
        -0.5f,  0.5f, -0.5f,  0.505f, 0.661f,
        -0.5f, -0.5f, -0.5f,  0.505f, 0.338f,
        -0.5f, -0.5f, -0.5f,  0.505f, 0.338f,
        -0.5f, -0.5f,  0.5f,  0.745f, 0.338f,
        -0.5f,  0.5f,  0.5f,  0.745f, 0.661f,


         0.5f,  0.5f,  0.5f,  0.755f, 0.661f,
         0.5f,  0.5f, -0.5f,  0.995f, 0.661f,
         0.5f, -0.5f, -0.5f,  0.995f, 0.338f,
         0.5f, -0.5f, -0.5f,  0.995f, 0.338f,
         0.5f, -0.5f,  0.5f,  0.755f, 0.338f,
         0.5f,  0.5f,  0.5f,  0.755f, 0.661f,


         -0.5f, -0.5f, -0.5f,  0.255f, 0.328f,
          0.5f, -0.5f, -0.5f,  0.495f, 0.328f,
          0.5f, -0.5f,  0.5f,  0.495f, 0.005f,
          0.5f, -0.5f,  0.5f,  0.495f, 0.005f,
         -0.5f, -0.5f,  0.5f,  0.255f, 0.005f,
         -0.5f, -0.5f, -0.5f,  0.255f, 0.328f,


         -0.5f,  0.5f, -0.5f,  0.255f, 0.995f,
          0.5f,  0.5f, -0.5f,  0.495f, 0.995f,
          0.5f,  0.5f,  0.5f,  0.495f, 0.672f,
          0.5f,  0.5f,  0.5f,  0.495f, 0.672f,
         -0.5f,  0.5f,  0.5f,  0.255f, 0.672f,
         -0.5f,  0.5f, -0.5f,  0.255f, 0.995f
    };
    // world space positions of our cubes
    glm::vec3 grassPositions[450];
    int grassCount = 0;
    for (int x = -10; x <= 10; x++)
    {
        for (int z = -10; z <= 10; z++)
        {
            grassPositions[grassCount++] = glm::vec3(x, -1, z);
        }
    }

    glm::vec3 woodPositions[50];
    int woodCount = 0;
    glm::vec3 plankPositions[20];
    int plankCount = 0;
    for (int x = -3; x <= 3; x++)
    {
        int h = 3 - abs(x);
        for (int z = 0; z < 7; z++)
        {
            woodPositions[woodCount++] = glm::vec3(x, h, z);
            if (z == 0 || z == 6)
            {
                for (int y = 0; y < h; y++)
                {
                    if (z == 6 && x == 0 && y < 2)
                    {
                        continue;
                    }
                    float wallZ = (z == 0) ? 0.5f : 5.5f;
                    plankPositions[plankCount++] = glm::vec3(x, y, wallZ);
                }
            }
        }
    }


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // load textures (all three are the same 4x3 cross-layout atlas)
    // -------------------------------------------------------------
    stbi_set_flip_vertically_on_load(true);
    unsigned int texture1, texture2, texture3;
    int width, height, nrChannels;
    unsigned char* data;

    // grass
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    data = stbi_load(FileSystem::getPath("resources/textures/Grass.jpg").c_str(), &width, &height, &nrChannels, 3);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture Grass.jpg" << std::endl;
    }
    stbi_image_free(data);

  
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    data = stbi_load(FileSystem::getPath("resources/textures/Wood.jpg").c_str(), &width, &height, &nrChannels, 3);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture Wood.jpg" << std::endl;
    }
    stbi_image_free(data);

   
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    data = stbi_load(FileSystem::getPath("resources/textures/WoodPrank.jpg").c_str(), &width, &height, &nrChannels, 3);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture WoodPrank.jpg" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);


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
        glClearColor(0.53f, 0.75f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);


        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // render blocks
        glBindVertexArray(VAO);

        // grass ground
        glBindTexture(GL_TEXTURE_2D, texture1);
        for (int i = 0; i < grassCount; i++)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), grassPositions[i]);
            ourShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // logs (rotated so the round end faces the front)
        glBindTexture(GL_TEXTURE_2D, texture2);
        for (int i = 0; i < woodCount; i++)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), woodPositions[i]);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            ourShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // planks
        glBindTexture(GL_TEXTURE_2D, texture3);
        for (int i = 0; i < plankCount; i++)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), plankPositions[i]);
            ourShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
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
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
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