#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void createCircle(float vertices[], int segments, float radius, float cx, float cy, float r, float g, float b);
void updateCircleColor(float vertices[], int segments, float r, float g, float b);
void hueToRGB(float hue, float &r, float &g, float &b);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const int segments = 100;
const float radius = 0.15f;
const int vertexCountPerCircle = segments * 3;
const int floatsPerCircle = vertexCountPerCircle * 6;

const int maxCircles = 2000;              // room for this many circles, total
float vertices[maxCircles * floatsPerCircle];
int numCircles = 0;                      // how many circles have been placed so far

// each circle's original color and the moment (in seconds) it was placed
float baseR[maxCircles];
float baseG[maxCircles];
float baseB[maxCircles];
double spawnTime[maxCircles];
const float fadeDuration = 4.0f;         

// hue of the NEXT circle's color, in degrees around the color wheel (0-360)
float nextHue = 0.0f;

unsigned int VBO;
// bool mouseWasPressed = false;

const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    // as we only have a single shader, we could also just activate our shader once beforehand if we want to 
    glUseProgram(shaderProgram);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

     
        if (numCircles > 0)
        {
            double now = glfwGetTime();
            for (int i = 0; i < numCircles; i++)
            {
                float elapsed = (float)(now - spawnTime[i]);
                float t = elapsed / fadeDuration;
                if (t > 1.0f) t = 1.0f;

                float r = baseR[i] - t;
                float g = baseG[i] - t;
                float b = baseB[i] - t;

                float *slot = vertices + i * floatsPerCircle;
                updateCircleColor(slot, segments, r, g, b);
            }

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        }

        // render every circle placed so far
        if (numCircles > 0)
        {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, numCircles * vertexCountPerCircle);
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
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void hueToRGB(float hue, float &r, float &g, float &b)
{
    float huePrime = hue / 60.0f;                   
    float x = 1.0f - fabsf(fmodf(huePrime, 2.0f) - 1.0f); 

    if (huePrime < 1.0f)
    { 
        r = 1.0f; g = x;    b = 0.0f; 
    }
    else if (huePrime < 2.0f) 
    { 
        r = x;    g = 1.0f; b = 0.0f; 
    }
    else if (huePrime < 3.0f) 
    { 
        r = 0.0f; g = 1.0f; b = x;   
    }
    else if (huePrime < 4.0f) 
    { 
        r = 0.0f; g = x;    b = 1.0f; 
    }
    else if (huePrime < 5.0f) 
    { 
        r = x;    g = 0.0f; b = 1.0f; 
    }
    else                      
    { 
        r = 1.0f; g = 0.0f; b = x;    
    }
}

void createCircle(float vertices[], int segments, float radius, float cx, float cy, float r, float g, float b) {
    int index = 0;

    for (int i = 0; i < segments; i++)
    {
        float theta1 = 2.0f * 3.1415926f * float(i) / float(segments);
		float theta2 = 2.0f * 3.1415926f * float(i + 1) / float(segments);

        //center
        vertices[index++] = cx; // x
        vertices[index++] = cy; // y
        vertices[index++] = 0.0f; // z
        vertices[index++] = r;
        vertices[index++] = g;
        vertices[index++] = b;


        vertices[index++] = cx + radius * cosf(theta1);
        vertices[index++] = cy + radius * sinf(theta1);
        vertices[index++] = 0.0f;
        vertices[index++] = r;
        vertices[index++] = g;
        vertices[index++] = b;

        vertices[index++] = cx + radius * cosf(theta2);
        vertices[index++] = cy + radius * sinf(theta2);
        vertices[index++] = 0.0f;
        vertices[index++] = r;
        vertices[index++] = g;
        vertices[index++] = b;
    }
}

void updateCircleColor(float vertices[], int segments, float r, float g, float b)
{
    int index = 0;

    for (int i = 0; i < segments; i++)
    {
        index += 3;
        vertices[index++] = r;
        vertices[index++] = g;
        vertices[index++] = b;

        index += 3;
        vertices[index++] = r;
        vertices[index++] = g;
        vertices[index++] = b;

        index += 3;
        vertices[index++] = r;
        vertices[index++] = g;
        vertices[index++] = b;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool mouseIsPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    
    if (mouseIsPressed && numCircles < maxCircles)  //!mouseWasPressed&
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float ndcX = (float)(xpos / SCR_WIDTH) * 2.0f - 1.0f;
        float ndcY = 1.0f - (float)(ypos / SCR_HEIGHT) * 2.0f;


        float *slot = vertices + numCircles * floatsPerCircle;
        float r, g, b;
        hueToRGB(nextHue, r, g, b);
        createCircle(slot, segments, radius, ndcX, ndcY, r, g, b);

        baseR[numCircles] = r;
        baseG[numCircles] = g;
        baseB[numCircles] = b;
        spawnTime[numCircles] = glfwGetTime();
        numCircles++;

        nextHue += 30.0f;
        if (nextHue >= 360.0f) nextHue -= 360.0f;
    }
    // mouseWasPressed = mouseIsPressed;  
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}