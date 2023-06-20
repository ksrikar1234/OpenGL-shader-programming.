#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 a_Position;
    layout (location = 1) in vec3 a_Normal;
    layout (location = 2) in int a_TriangleID;

    out vec3 FragPos;
    out vec3 Normal;
    flat out int TriangleID;

    uniform mat4 u_Model;
    uniform mat4 u_View;
    uniform mat4 u_Projection;

    void main()
    {
        FragPos = vec3(u_Model * vec4(a_Position, 1.0));
        Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
        TriangleID = a_TriangleID;
        gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 FragPos;
    in vec3 Normal;
    flat in int TriangleID;

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(float(TriangleID & 0xFF) / 255.0, 
                         float((TriangleID >> 8) & 0xFF) / 255.0, 
                         float((TriangleID >> 16) & 0xFF) / 255.0, 
                         1.0);
    }
)";


/*

bool isTriangleVisible(const glm::mat4& mvp, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{

    glm::vec4 p0 = mvp * glm::vec4(v0, 1.0f);
    glm::vec4 p1 = mvp * glm::vec4(v1, 1.0f);
    glm::vec4 p2 = mvp * glm::vec4(v2, 1.0f);

    // Check if any vertex is inside the frustum
    if (p0.x >= -p0.w && p0.x <= p0.w && p0.y >= -p0.w && p0.y <= p0.w && p0.z >= -p0.w && p0.z <= p0.w)
        return true;
    if (p1.x >= -p1.w && p1.x <= p1.w && p1.y >= -p1.w && p1.y <= p1.w && p1.z >= -p1.w && p1.z <= p1.w)
        return true;
    if (p2.x >= -p2.w && p2.x <= p2.w && p2.y >= -p2.w && p2.y <= p2.w && p2.z >= -p2.w && p2.z <= p2.w)
        return true;

    // Check if any edge intersects the frustum
    if ((p0.x < -p0.w && p1.x > p1.w) || (p0.x > p0.w && p1.x < -p1.w) ||
        (p0.y < -p0.w && p1.y > p1.w) || (p0.y > p0.w && p1.y < -p1.w) ||
        (p0.z < -p0.w && p1.z > p1.w) || (p0.z > p0.w && p1.z < -p1.w))
        return true;
    if ((p1.x < -p1.w && p2.x > p2.w) || (p1.x > p1.w && p2.x < -p2.w) ||
        (p1.y < -p1.w && p2.y > p2.w) || (p1.y > p1.w && p2.y < -p2.w) ||
        (p1.z < -p1.w && p2.z > p2.w) || (p1.z > p1.w && p2.z < -p2.w))
        return true;
    if ((p2.x < -p2.w && p0.x > p0.w) || (p2.x > p2.w && p0.x < -p0.w) ||
        (p2.y < -p2.w && p0.y > p0.w) || (p2.y > p2.w && p0.y < -p0.w) ||
        (p2.z < -p2.w && p0.z > p0.w) || (p2.z > p2.w && p0.z < -p0.w))
        return true;
   
    return false;


}*/












bool isPointInTriangle(const glm::vec3& point, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    glm::vec3 edge0 = v1 - v0;
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v0 - v2;

    glm::vec3 normal = glm::cross(edge0, edge1);

    // Test against the three edges of the triangle
    glm::vec3 c0 = glm::cross(edge0, point - v0);
    if (glm::dot(normal, c0) < 0.0f)
        return false;

    glm::vec3 c1 = glm::cross(edge1, point - v1);
    if (glm::dot(normal, c1) < 0.0f)
        return false;

    glm::vec3 c2 = glm::cross(edge2, point - v2);
    if (glm::dot(normal, c2) < 0.0f)
        return false;

    return true;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float normalizedX = (2.0f * mouseX) / width - 1.0f;
        float normalizedY = 1.0f - (2.0f * mouseY) / height;

        unsigned char pixel[4];
        glReadPixels((int)mouseX, (int)mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

      
        glm::vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);                
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 inversePV = glm::inverse(projection * view);

        glm::vec4 rayEye = inversePV * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

        float nearestDistance = std::numeric_limits<float>::max();
        glm::vec3 intersectionPoint;

        for (unsigned int i = 0; i < 6; i++)
        {
           
            glm::vec3 v0 (cubeVertices[i],cubeVertices[i+1], cubeVertices[i+2] );
            glm::vec3 v1 (cubeVertices[i+6],cubeVertices[i+7], cubeVertices[i+8]);
            glm::vec3 v2 (cubeVertices[i+12],cubeVertices[i+13], cubeVertices[i+14] );

            glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);

            float denominator = glm::dot(rayWorld, normal);
            if (std::fabs(denominator) < 0.0001f)
                continue;

            float t = glm::dot(v0 - glm::vec3(0.0f, 0.0f, 0.0f), normal) / denominator;
            if (t < 0.0f)
                continue;

            glm::vec3 pointOnPlane = glm::vec3(0.0f, 0.0f, 0.0f) + t * rayWorld;
            if (isPointInTriangle(pointOnPlane, v0, v1, v2))
            {
                if (t < nearestDistance)
                {
                    nearestDistance = t;
                    intersectionPoint = pointOnPlane;
                }
            }
        }

        if (nearestDistance < std::numeric_limits<float>::max())
        {
            std::cout << "Intersection Point: (" << intersectionPoint.x << ", " << intersectionPoint.y << ", " << intersectionPoint.z << ")" << std::endl;
        }
    }

        // Retrieve the triangle ID from the pixel value
        int triangleID = pixel[0] + pixel[1] * 256 + pixel[2] * 256 * 256;

        // Print the triangle ID
        std::cout << "Triangle ID: " << triangleID << std::endl;
}


unsigned int createShaderProgram()
{
    // Compile the vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create the shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete the shaders as they're no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle ID", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the created window the current context
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the mouse button callback function
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Create the vertex array object (VAO) and vertex buffer object (VBO)
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the VAO and VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Set up the vertex data
    // ...

    // Set up the vertex attributes
    // ...

    // Create the shader program
    unsigned int shaderProgram = createShaderProgram();

    // Use the shader program
    glUseProgram(shaderProgram);

    // Set the model, view, and projection matrices in the shader program
    // ...

    // Set the vertex attribute pointers
    // ...

    // Main rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the triangles
        // ...

        // Swap the front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}

