#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Cube vertices and indices
std::vector<float> cubeVertices = {
    // Positions
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.5f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
};

std::vector<unsigned int> cubeIndices = {
    // Front face
    0, 1, 2,
    2, 3, 0,
    // Back face
    4, 5, 6,
    6, 7, 4,
    // Left face
    4, 7, 3,
    3, 0, 4,
    // Right face
    1, 2, 6,
    6, 5, 1,
    // Top face
    3, 2, 6,
    6, 7, 3,
    // Bottom face
    0, 1, 5,
    5, 4, 0
};

bool isPointInTriangle(const glm::vec3& point, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    glm::vec3 edge0 = v1 - v0;
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v0 - v2;

    glm::vec3 c0 = point - v0;
    glm::vec3 c1 = point - v1;
    glm::vec3 c2 = point - v2;

    glm::vec3 normal = glm::cross(edge0, edge1);

    return glm::dot(glm::cross(edge0, c0), normal) >= 0 &&
           glm::dot(glm::cross(edge1, c1), normal) >= 0 &&
           glm::dot(glm::cross(edge2, c2), normal) >= 0;
}

void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Print mouse position to console
    std::cout << "Mouse position: " << xpos << ", " << ypos << '\n';
 
       float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
       float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;
      std::cout << "Mouse position: " << x << ", " << y << '\n';
 /*   
    
    int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    // Check if the right mouse button is pressed
     // {
    //    std::cout << "Right mouse button clicked at position: (" << x << ", " << y << ")" << std::endl; }
    
    
    if (rightButtonState == GLFW_PRESS) 
       {
        std::cout << "Right mouse button clicked! Rotation mode active " << '\n';
        mouse_x = xpos;
        mouse_y = ypos;
       }
     
     if (leftButtonState == GLFW_PRESS) 
       {
        std::cout << "Right mouse button clicked! translation mode active " << '\n';
        translate_mouse_x = xpos;
        translate_mouse_y = ypos;
       }
         
   */    
       
       
}





void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Convert mouse coordinates to normalized device coordinates
        float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;

        // Calculate the ray in clip space
        glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

        // Calculate the ray in view space
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 rayView = glm::inverse(projection) * rayClip;
        rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);

        // Calculate the ray in world space
        glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayView));

        // Check for intersection with each face of the cube
        float tMin = std::numeric_limits<float>::max();
        int selectedFace = -1;
        glm::vec3 cameraPosition(0.0f, 0.0f, 3.0f); // Camera position in world space
        for (int i = 0; i < 6; ++i)
        {
            glm::vec3 v0 = glm::vec3(cubeVertices[cubeIndices[i * 6] * 6], cubeVertices[cubeIndices[i * 6] * 6 + 1], cubeVertices[cubeIndices[i * 6] * 6 + 2]);
            glm::vec3 v1 = glm::vec3(cubeVertices[cubeIndices[i * 6 + 1] * 6], cubeVertices[cubeIndices[i * 6 + 1] * 6 + 1], cubeVertices[cubeIndices[i * 6 + 1] * 6 + 2]);
            glm::vec3 v2 = glm::vec3(cubeVertices[cubeIndices[i * 6 + 2] * 6], cubeVertices[cubeIndices[i * 6 + 2] * 6 + 1], cubeVertices[cubeIndices[i * 6 + 2] * 6 + 2]);
            glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
            float denominator = glm::dot(rayWorld, normal);
            if (denominator != 0.0f)
            {
                float t = glm::dot(v0 - cameraPosition, normal) / denominator;
                if (t > 0.0f && t < tMin)
                {
                    glm::vec3 intersectionPoint = cameraPosition + t * rayWorld;
                    if (isPointInTriangle(intersectionPoint, v0, v1, v2))
                    {
                        tMin = t;
                        selectedFace = i;
                    }
                }
            }
        }

        if (selectedFace != -1)
        {
            std::cout << "Selected face: " << selectedFace << std::endl;
        }
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cube Selection", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
    glfwSetCursorPosCallback(window, mousePositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);


    // Create vertex buffer object and vertex array object
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), cubeIndices.data(), GL_STATIC_DRAW);

    // Specify the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Create shader program
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aColor;
        out vec3 color;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        void main()
        {
            gl_Position = projection * view * model * vec4(aPosition, 1.0);
            color = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 color;
        out vec4 fragColor;
        void main()
        {
            fragColor = vec4(color, 1.0);
        }
    )";

    GLuint vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glUseProgram(shaderProgram);
        
         float time = glfwGetTime();
         glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 0.0f, 1.0f));
         glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
         glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);




        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
