#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



//-------------------------------------------------Screen Resolution---------------------------------------------------------------------------+

const GLuint WIDTH = 800, HEIGHT = 600;

//+------------------------------Globals for Peripheral inputs inputs--------------------------------------------------------------------------+


float mouse_x  = 0.0f , mouse_y = 0.0f;

float translate_mouse_x = 0.0f , translate_mouse_y = 0.0f ;

float rotate_mouse_x = 0.0f  , rotate_mouse_y = 0.0f;

float scroll_offset = 0.0f;

float up_key = 0.0f , down_key = 0.0f , left_key = 0.0f , right_key = 0.0f;

float w_key = 0.0f , a_key = 0.0f , s_key = 0.0f , d_key = 0.0f;


//-----------------------------------------------Camera Params------------------------------------------------------------------+

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//----------------------------------Reading Geometry from STL file Triangle definition-------------------------------------------+

typedef unsigned long long TRIANGLE_ID;
typedef GLfloat POINT;
typedef GLfloat COORD;
typedef GLfloat COLOR;
typedef COORD VERTEX[3];
typedef COORD NORMAL[3];


class Triangle {

public:

    NORMAL normal;
    VERTEX verticies[3];
    VERTEX vertex1;
    VERTEX vertex2;
    VERTEX vertex3;
    COLOR  vertex1_color;
    COLOR  vertex2_color;
    COLOR  vertex3_color;


};

class Geometry {

public:
std::vector<Triangle> triangles;

Geometry(const std::string& filename = "NOT_SET")
{
  this->readSTLFile(filename);
}
   
    std::vector<Triangle> readSTLFile(const std::string& filename) {
        if (filename == "NOT_SET")
            return this->triangles;

        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return this->triangles;
        }
      
         std::string line;
         std::string token;

    // Check if the file is in binary format
    bool isBinary = false;
    std::getline(file, line);
    if (line.find("solid") == std::string::npos) {
        isBinary = true;
        //std::cout << "input file is binary" << '\n';
        file.close();
        file.open(filename, std::ios::binary);
    }
    else {
        file.seekg(0); // Return to the beginning of the file for reading in ASCII format
    }

    if (isBinary) {
        file.seekg(80, std::ios::beg);

        // Read the number of triangles (4 bytes)
        uint32_t numTriangles;
        file.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));

        // Reserve space for triangles
        this->triangles.reserve(numTriangles);

        // Read each triangle
        for (uint32_t i = 0; i < numTriangles; ++i) {
            Triangle triangle;

            // Read the normal vector (12 bytes)
            file.read(reinterpret_cast<char*>(&triangle.normal), sizeof(triangle.normal));

            // Read the vertices (36 bytes)
            file.read(reinterpret_cast<char*>(&triangle.vertex1), sizeof(triangle.vertex1));
            file.read(reinterpret_cast<char*>(&triangle.vertex2), sizeof(triangle.vertex2));
            file.read(reinterpret_cast<char*>(&triangle.vertex3), sizeof(triangle.vertex3));

            // Ignore attribute byte count (2 bytes)
            file.seekg(2, std::ios::cur);
            //triangle.color = 
            this->triangles.push_back(triangle);
        }


        
    }
     
    else {
        std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Regular expression patterns
        std::regex facetPattern(R"(facet normal ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+))");
        std::regex vertexPattern(R"(vertex ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+))");

        std::smatch match;
        std::string::const_iterator searchStart(fileContents.cbegin());
        while (std::regex_search(searchStart, fileContents.cend(), match, facetPattern)) {
            Triangle triangle;

            // Extract the normal vector
            triangle.normal[0] = std::stof(match[1]);
            triangle.normal[1] = std::stof(match[2]);
            triangle.normal[2] = std::stof(match[3]);

            // Search for vertex coordinates within the current facet
            std::string::const_iterator vertexSearchStart(match.suffix().first);
            for (int i = 0; i < 3; ++i) {
                if (!std::regex_search(vertexSearchStart, fileContents.cend(), match, vertexPattern))
                    break;

                // Extract vertex coordinates
                triangle.vertex1[i] = std::stof(match[1]);
                triangle.vertex2[i] = std::stof(match[2]);
                triangle.vertex3[i] = std::stof(match[3]);

                vertexSearchStart = match.suffix().first;
            }

            this->triangles.push_back(triangle);
            searchStart = match.suffix().first;
        }

       
    }

  

file.close();
return this->triangles;

}
 


std::vector<COORD> VERTEX_ARRAY_GL ;

std::vector<COORD> LOAD_GL_VERTEX_ARRAY()
{
   
   std::cout << "loading vertices for vertex array \n" ;

   for(auto triangle : this->triangles)
       {
            for(auto coord :  triangle.vertex1) this->VERTEX_ARRAY_GL.push_back(coord); 
            for(auto coord :  triangle.vertex2) this->VERTEX_ARRAY_GL.push_back(coord); 
            for(auto coord :  triangle.vertex3) this->VERTEX_ARRAY_GL.push_back(coord);                          
       }    

     return this->VERTEX_ARRAY_GL;         

}

};


//--------------------------------------Vertices-----------------------------------------------------------------------------------+

std::vector<GLfloat> cubeVertices = {
    // Positions         // Colors
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

     -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

     0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f

};

//--------------------------------------Vertex Shader-----------------------------------------------------------------------------+

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 ourColor;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";
//-------------------------------------------Fragment Shader----------------------------------------------------------------+
const char* fragmentShaderSource = R"(

    #version 330 core
    in vec3 ourColor;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
)";


//---------------------------------------------check which triangles are present on the projection plane----------------------+


bool rayTriangleIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                          const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                          glm::vec3& intersectionPoint)
{
    const float epsilon = 0.0001f;

    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;

    glm::vec3 P = glm::cross(rayDirection, e2);
    float det = glm::dot(e1, P);

    if (std::abs(det) < epsilon)
        return false;

    float invDet = 1.0f / det;

    glm::vec3 T = rayOrigin - v0;

    float u = glm::dot(T, P) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 Q = glm::cross(T, e1);

    float v = glm::dot(rayDirection, Q) * invDet;
    if (v < 0.0f || v > 1.0f)
        return false;

    float t = glm::dot(e2, Q) * invDet;
    if (t < 0.0f)
        return false;

    if (u + v > 1.0f)
        return false;

    intersectionPoint = rayOrigin + t * rayDirection;
    return true;
}



glm::vec3 createMouseRay(GLFWwindow* window, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix,
                    glm::vec3& rayOrigin, glm::vec3& rayDirection)
{
    // Get the mouse cursor position in screen coordinates
    //float mouseX = mouse_x, mouseY = mouse_y;
     double mouseX ,mouseY;
     glfwGetCursorPos(window, &mouseX, &mouseY);
    
    // Get the window dimensions
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Convert screen coordinates to normalized device coordinates (NDC)
    float ndcX = (2.0f * mouseX) / WIDTH - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / HEIGHT;

    // Convert NDC to view-space coordinates
    glm::vec4 clipSpacePos = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
    glm::vec4 viewSpacePos = inverseProjection * clipSpacePos;
    //viewSpacePos.z = -1.0f;  // Set the z-component to -1 to make it perpendicular to the viewing plane
    
    viewSpacePos /= viewSpacePos.w;

    // Convert view-space coordinates to world-space coordinates
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec4 worldSpacePos = inverseView * viewSpacePos;
    worldSpacePos /= worldSpacePos.w;

    // Set the ray origin and direction
    rayOrigin = glm::vec3(inverseView[3]);    
    rayDirection = glm::normalize(glm::vec3(worldSpacePos) - rayOrigin);

    return glm::vec3(worldSpacePos);
}




glm::vec3 createPerpendicularMouseRay(GLFWwindow* window, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix,
                                glm::vec3& rayOrigin, glm::vec3& rayDirection)
{
   // Get the mouse cursor position in screen coordinates
    
    //float mouseX = mouse_x, mouseY = mouse_y;
    
    double mouseX ,mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Get the window dimensions
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Convert screen coordinates to normalized device coordinates (NDC)
    float ndcX = (2.0f * mouseX) / WIDTH - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / HEIGHT;

    // Create a ray direction that is perpendicular to the viewing plane
    glm::vec4 rayClipSpacePos = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
    glm::vec4 rayViewSpacePos = inverseProjection * rayClipSpacePos;
    rayViewSpacePos.z = -1.0f;  
    rayViewSpacePos.w = 0.0f;
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec4 rayWorldSpacePos = inverseView * rayViewSpacePos;
    rayWorldSpacePos /= rayWorldSpacePos.w;

    // Set the ray origin and direction
    rayOrigin = glm::vec3(inverseView[3]);
    rayDirection = glm::normalize(glm::vec3(rayWorldSpacePos) - rayOrigin);
    
    return glm::vec3(rayWorldSpacePos);

}


//--------------------------------------------------Peripheral input handling---------------------------------------------------+

void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Print mouse position to console
    //std::cout << "Mouse position: " << xpos << ", " << ypos << std::endl;
    
    mouse_x = xpos;
    mouse_y = ypos;
    
    int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    int leftButtonState  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    

    if (rightButtonState == GLFW_PRESS) 
       {
       // std::cout << "Right mouse button clicked! Rotation mode active " << '\n';
        rotate_mouse_x = xpos;
        rotate_mouse_y = ypos;
       }
     
     if (leftButtonState == GLFW_PRESS) 
       {
      //  std::cout << "Left mouse button clicked! translation mode active " << '\n';
        translate_mouse_x = xpos;
        translate_mouse_y = ypos;
       }
       
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    // Handle scroll input here
    // xOffset and yOffset represent the scroll offset values
    // xOffset indicates horizontal scrolling (e.g., trackpad swipe left/right)
    // yOffset indicates vertical scrolling (e.g., mouse wheel up/down)

    // Example: Print the scroll offsets

    scroll_offset += float(yOffset)/10 ;

    std::cout << "Scroll: xOffset = " << xOffset << ", yOffset = " << scroll_offset << std::endl;
    


}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Check if the key is one of the arrow keys
    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        // Handle left arrow key press
        // Example: Move the camera or perform some action
        
        left_key += 0.1;

        std::cout << "Left arrow key pressed" << std::endl;
    } else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle right arrow key press
        // Example: Move the camera or perform some action
        
        right_key += 0.1;
      
        std::cout << "Right arrow key pressed" << std::endl;
    } else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle up arrow key press
        // Example: Move the camera or perform some action
        
        up_key += 0.1;

        std::cout << "Up arrow key pressed" << std::endl;
    } else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle down arrow key press
        // Example: Move the camera or perform some action
         
         down_key += 0.1f;

        std::cout << "Down arrow key pressed" << std::endl;
    }

    else if (key == GLFW_KEY_W && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle down arrow key press
        // Example: Move the camera or perform some action
         
         w_key += 0.05f;

        std::cout << "Down arrow key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_A && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle down arrow key spress
        // Example: Move the camera or perform some action
         
         a_key += 0.05f;

        std::cout << "Down arrow key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_S && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle down arrow key press
        // Example: Move the camera or perform some action
         
         s_key += 0.05f;

        std::cout << "Down arrow key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_D && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle down arrow key press
        // Example: Move the camera or perform some action
         
         d_key += 0.05f;

        std::cout << "Down arrow key pressed" << std::endl;
    }

}




//--------------------------------glfw window creation , checks , destruction Blah blah blah-------------------------------------+

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

inline void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

inline short int check_glfwInit() {
      if (!glfwInit()) {std::cerr << "Failed to initialize GLFW" << '\n';     return -1;  }
      else             {std::cout << "Successfully initialised GLFW" << '\n'; return  0;  } 
      } 

inline short int check_glfw_window(auto window) {
    if (!window) { std::cerr << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; } 
    else {std::cout << "Successfully created window" ; }
}    
//----------------------------------------------------------------------------------------------------------------------------+

int main()
{
    //-------------------------Initialize GLFW---------------------------------------------------------+
     check_glfwInit();
    // -----------------------Set OpenGL version and profile to 3.3 core--------------------------------+

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //--------------------------Create a GLFW window----------------------------------------------------+

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Cube Rotation", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);    
 
   //----------------------Peripherals input handling--------------------------------------------------+

   // glfwSetCursorPosCallback(window, initialPositionCallback);

      glfwSetCursorPosCallback(window, mousePositionCallback);
      glfwSetScrollCallback(window, scrollCallback);
      glfwSetKeyCallback(window, keyCallback);

   //------------------------Initialize GLEW----------------------------------------------------------+
   
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    //---------------------------- Create vertex shader----------------------------------------------------+
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    //-------------------------- Check for vertex shader compile errors-------------------------------------+
   
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    //-------------------------------Create fragment shader-----------------------------------------------+

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    //-----------------------Check for fragment shader compile errors-------------------------------------+

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    //---------------------------Create shader program-----------------------------------------------------+
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //---------------------------Check for shader program link errors--------------------------------------+
   
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    //-------------------Delete shaders as they're linked into the program now and no longer needed--------+

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //-------------------Create vertex buffer object (VBO) and vertex array object (VAO)-------------------+
    
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    //--------Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attribute(s)---+
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(GLfloat), &cubeVertices[0], GL_STATIC_DRAW);

    //---------------------Position attribute--------------------------------------------------------------+
   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    //------------------------Color attribute--------------------------------------------------------------+
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //---------------------Unbind VBO and VAO---------------------------------------------------------------+
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);    

    //-------------------------Enable depth testing---------------------------------------------------------+
   
    glEnable(GL_DEPTH_TEST);

    //----------------------fps counter variables-----------------------------------------------------------+

    double lastTime = glfwGetTime();
    int frameCount = 0;
    int fps = 0;

    //---------------------------------Render loop---------------------------------------------------------+
   
    while (!glfwWindowShouldClose(window))
    {
        //------------------------------Input handling-----------------------------------------------------+
        
        processInput(window);

        //-----------------------------Clear the screen----------------------------------------------------+

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //--------------------------Activate shader program------------------------------------------------+
        
        glUseProgram(shaderProgram);

        //---------------------------Create transformations------------------------------------------------+

        float time = glfwGetTime();
        
        float mouse_input = float((mouse_x/WIDTH + mouse_y/HEIGHT)*3.14);
        
        float set_axis_x =  10*((2.0*rotate_mouse_x/WIDTH)-1.0f) , set_axis_y = 10*(1.0f-(2.0*rotate_mouse_y/HEIGHT));
        
        // glm::mat4 view = glm::translate(glm::mat4(0.1f), glm::vec3((translate_mouse_x*2/WIDTH)-1.0f, (1.0f-(2.0f*translate_mouse_y/HEIGHT)), -3.0f));

        //-----------------------------------Translate + Zoom functionality-------------------------------------------------------------+
        //-------------------------X-axis-----------------Y-axis----------------Z-axis--------------------------------------------------+

        cameraPos = glm::vec3(-right_key+left_key, - up_key + down_key, -scroll_offset + 2.0f);
    
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        view = glm::translate(view, glm::vec3(4*((translate_mouse_x*2.0f/WIDTH)-1.0f), 4*(1.0f-(2.0f*translate_mouse_y/HEIGHT)), -3.0f)); 
 
        //-------------------------------------------Rotate functionality---------------------------------------------------------------+

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (-set_axis_y + w_key - s_key), glm::vec3(1.0f, 0.0f, 0.0f));
        
        model = glm::rotate(model, (-set_axis_x + d_key - a_key), glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        
 
       //--------------------------------Implement Mouse_rayClip_intersection-------------------------------------------------------------+
       
        glm::mat4 mvp = projection * view * model;     
         glm::vec3 intersectionPoint;
        
       // #pragma omp parallel for 
        for (size_t i = 0; i < cubeVertices.size(); i += 18){

            GLuint triangleID = i / 18;
          
            glm::vec3 v0 (cubeVertices[i],cubeVertices[i+1], cubeVertices[i+2] );
            glm::vec3 v1 (cubeVertices[i+6],cubeVertices[i+7], cubeVertices[i+8]);
            glm::vec3 v2 (cubeVertices[i+12],cubeVertices[i+13], cubeVertices[i+14] );

            glm::vec4 new_v0 = mvp * glm::vec4(v0, 1.0);
            glm::vec4 new_v1 = mvp * glm::vec4(v1, 1.0);
            glm::vec4 new_v2 = mvp * glm::vec4(v2, 1.0);
       
            glm::vec3 V0 = glm::vec3(new_v0);  
            glm::vec3 V1 = glm::vec3(new_v1);  
            glm::vec3 V2 = glm::vec3(new_v2);  
                        
            glm::vec3 rayOrigin;
            glm::vec3 rayDirection;

           // glm::mat4 viewMatrix = glm::translate(view, glm::vec3(4*((mouse_x*2.0f/WIDTH)-1.0f), 4*(1.0f-(2.0f*mouse_y/HEIGHT)), -3.0f)); 

            glm::vec3 mouse_ray = createMouseRay(window, projection, view, rayOrigin, rayDirection);
            
           // std::cout << " : " << rayOrigin[0]  << " : " << rayOrigin[1] <<   " : " << rayOrigin[2] << '\n';
           // std::cout << " : " << rayDirection[0]  << " : " << rayDirection[1] <<   " : " << rayDirection[2] << '\n';
              
           //  auto mouse_ray = createPerpendicularMouseRay(window, projection, view, rayOrigin, rayDirection);

            //  std::cout<< '\n' << mouse_ray[0] <<" "<< mouse_ray[1] <<" "<< mouse_ray[2] << '\n';
           
             
            if (rayTriangleIntersect(rayOrigin, rayDirection, V0, V1, V2, intersectionPoint)) {
            // Intersection occurred, use intersectionPoint
            // ...
            std::cout << "Triangle " << triangleID << " intersected at : ("  <<  intersectionPoint[0] << " , " << intersectionPoint[1]  << " , "<< intersectionPoint[2] << ")"<<  std::endl;
             }

        }  

//----------------------------------------------------------------------------------------------------------------+



        //------------------------Pass transformation matrices to the shader-----------------------------+
                
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));        

        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // After creating the shader program and obtaining the uniform location
        
        //  glm::vec3 intersectionPoint = ...; // Calculate the intersection point
       
        glUseProgram(shaderProgram);
       
        GLuint intersectionPointLocation = glGetUniformLocation(shaderProgram, "intersectionPoint");
       
        glUniform3fv(intersectionPointLocation, 1, glm::value_ptr(intersectionPoint));

        //---------------------------------Draw cube-------------------------------------------------------+

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        //------------------------------ MouseRayTrianglePlane intersection point calculation ------------------------------------------------------+

       
        //  std::cout << "mouse_x : " << mouse_x << "\n";

              
        //------------------------------Unbind the VAO------------------------------------------------------+
        
        glBindVertexArray(0);
        
        //------------------------------Swap buffers and poll events----------------------------------------+

        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //-----------------------------Fps counter----------------------------------------------------------+

         double currentTime = glfwGetTime();
         frameCount++;
         if (currentTime - lastTime >= 1.0)
           {
              fps = frameCount;
              frameCount = 0;
              lastTime = currentTime;
           }

         // Print FPS
         // std::cout << "FPS: " << fps << std::endl;

        //--------------------------------------------------------------------------------------------------+
    }

    // Clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}




/*

1. The function `rayTriangleIntersect` takes the following parameters:
   - `rayOrigin`: The origin point of the ray.
   - `rayDirection`: The direction vector of the ray.
   - `v0`, `v1`, `v2`: The vertices of the triangle in 3D space.
   - `intersectionPoint`: A reference to a `glm::vec3` variable that will store the intersection point if an intersection occurs.

2. `epsilon` is a small value used as a tolerance for floating-point comparisons.

3. The function calculates two edge vectors of the triangle: `e1` and `e2`. These vectors are obtained by subtracting `v0` from `v1` and `v2` respectively.

4. The function computes the cross product of the `rayDirection` and `e2` vectors, storing it in `P`.

5. The dot product of `e1` and `P` is computed and stored in `det`. This value is used to determine if the ray and the triangle are parallel or nearly parallel. If the absolute value of `det` is smaller than `epsilon`, it means the determinant is close to zero and the ray and triangle are almost parallel, so no intersection occurs.

6. The inverse of the determinant, `invDet`, is calculated for later use.

7. The vector from the `rayOrigin` to `v0` is computed and stored in `T`.

8. The function computes the u-parameter by taking the dot product of `T` and `P` and multiplying it by `invDet`. If the resulting value is less than 0 or greater than 1, it means the intersection point lies outside the triangle's boundaries, so no intersection occurs.

9. The function computes the Q vector by taking the cross product of `T` and `e1`.

10. The v-parameter is calculated by taking the dot product of `rayDirection` and `Q` and multiplying it by `invDet`. If the resulting value is less than 0 or greater than 1, it means the intersection point lies outside the triangle's boundaries, so no intersection occurs.

11. The t-parameter is computed by taking the dot product of `e2` and `Q` and multiplying it by `invDet`. If the resulting value is negative, it means the intersection point lies behind the ray origin, so no intersection occurs.

12. If `u + v` is greater than 1, it means the intersection point lies outside the triangle's boundaries, so no intersection occurs.

13. If all the conditions are met, an intersection occurs. The intersection point is calculated by multiplying the ray direction by `t` and adding it to the ray origin. The resulting point is stored in `intersectionPoint`.

14. The function returns `true` to indicate that an intersection occurred and the intersection point is valid.

This code implements the Moller-Trumbore algorithm, a commonly used method for ray-triangle intersection testing. It checks for the intersection between a ray and a triangle in 3D space, taking into account the triangle's vertices and the direction of the ray. If an intersection occurs, the function calculates the intersection point and returns `true`. Otherwise, it returns `false`.

*/

