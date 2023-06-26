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
#include <omp.h>


//-------------------------------------------------Screen Resolution---------------------------------------------------------------------------+

const GLuint WIDTH = 1920/2, HEIGHT = 1080/2;

//+------------------------------Globals for Peripheral inputs inputs--------------------------------------------------------------------------+


float mouse_x  = 0.0f , mouse_y = 0.0f;

float ndcX , ndcY ;

float initialMouseX = 0.0f ,  initialMouseY = 0.0f;

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
typedef GLfloat COLOUR_ATTRIBUTE;

typedef COORD  VERTEX[3];
typedef COORD  NORMAL[3];
typedef COLOUR_ATTRIBUTE COLOR[3] ;

class Triangle {

public:

    TRIANGLE_ID ID;
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
std::vector<COORD> VERTEX_ARRAY_GL ;
uint64_t NUM_TRIANGLES = 0 ;

Geometry(const std::string& filename = "NOT_SET")
{
  this->readSTLFile(filename);
  this->NUM_TRIANGLES = this->triangles.size();
 // std::cout << "Num triangles =" << this->NUM_TRIANGLES;
  this->LOAD_GL_VERTEX_ARRAY();
}
   

GLfloat generateColor(int triangleID) {



   GLfloat color ;
   if(triangleID%2 == 0) color = 1.0f;
   else if(triangleID%3 == 0) color = 0.30f;
   else if(triangleID%4 == 0) color = 0.10f;
   else if(triangleID%5 == 0) color = 0.40f;
   else if(triangleID%17 == 0) color = 0.10f;
   else color = 0.50f;


   return color;
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
            triangle.ID = numTriangles;
            std::scientific;

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

        std::regex facetPattern("facet normal ([-+]?[0-9]*\\.?[0-9]+(?:e[-+]?[0-9]+)?) ([-+]?[0-9]*\\.?[0-9]+(?:e[-+]?[0-9]+)?) ([-+]?[0-9]*\\.?[0-9]+(?:e[-+]?[0-9]+)?)");
        std::regex vertexPattern("vertex ([-+]?[0-9]*\\.?[0-9]+(?:e[-+]?[0-9]+)?) ([-+]?[0-9]*\\.?[0-9]+(?:e[-+]?[0-9]+)?) ([-+]?[0-9]*\\.?[0-9]+(?:e[-+]?[0-9]+)?)");
       // std::regex facetPattern(R"(facet normal ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+))");
       // std::regex vertexPattern(R"(vertex ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+))");

        std::smatch match;
        std::string::const_iterator searchStart(fileContents.cbegin());
        uint32_t numTriangles = 0;
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
            numTriangles += 1; 
            triangle.ID = numTriangles;
            this->triangles.push_back(triangle);
            searchStart = match.suffix().first;
        }

       
    }

  

file.close();
return this->triangles;

}
 

std::vector<COORD> LOAD_GL_VERTEX_ARRAY()
{
   
 //  std::cout << "loading vertices for vertex array \n" ;

   uint32_t id = 0;
   for(auto triangle : this->triangles)
       {
            float x = 0.0f;
          
            for(auto coord : triangle.vertex1)        this->VERTEX_ARRAY_GL.push_back(coord); 

            this->VERTEX_ARRAY_GL.push_back(1.0f); 
            this->VERTEX_ARRAY_GL.push_back(0.0f); 
            this->VERTEX_ARRAY_GL.push_back(0.0f); 


            for(auto coord :  triangle.vertex2)       this->VERTEX_ARRAY_GL.push_back(coord); 
         
            this->VERTEX_ARRAY_GL.push_back(0.0f); 
            this->VERTEX_ARRAY_GL.push_back(1.0f); 
            this->VERTEX_ARRAY_GL.push_back(0.0f);

            for(auto coord :  triangle.vertex3)        this->VERTEX_ARRAY_GL.push_back(coord); 
            this->VERTEX_ARRAY_GL.push_back(0.0f); 
            this->VERTEX_ARRAY_GL.push_back(1.0f); 
            this->VERTEX_ARRAY_GL.push_back(1.0f);    
           

            ++id;
        }    
    
     return this->VERTEX_ARRAY_GL;         

}

};



//--------------------------------------Vertices-----------------------------------------------------------------------------------+

/*

std::vector<GLfloat> GeometryVertices = {
    // Positions         // Colors

    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

     -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

     0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f

};

*/

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

}


//-----------------------------------------Perform Ray Triangle intersection-------------------------------------------------+

bool rayTriangleIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                          const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                          glm::vec3& intersectionPoint)
{
    const float epsilon = 0.000001f;

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


//--------------------------------------------------Peripheral input handling---------------------------------------------------+

void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Print mouse position to console
    //std::cout << "Mouse position: " << xpos << ", " << ypos << std::endl;
    
    mouse_x = xpos;
    mouse_y = ypos;
   
    bool initial_rightClick = true;
    bool initial_leftClick = true;

    // float ndcX = (2.0f * mouse_x) / WIDTH - 1.0f;
    // float ndcY = 1.0f - (2.0f * mouse_y) / HEIGHT;

    int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    int leftButtonState  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    

    if (rightButtonState == GLFW_PRESS) 
       {
         // std::cout << "Right mouse button clicked! Rotation mode active " << '\n';
        if (initial_rightClick)
          {
            initialMouseX = mouse_x;
            initialMouseY = mouse_y;
            initial_rightClick  = false;
            rotate_mouse_x = initialMouseX;
            rotate_mouse_y = initialMouseY;

          }
        else 
           {   
               rotate_mouse_x = mouse_x - initialMouseX;
               rotate_mouse_y = mouse_y - initialMouseY;
           }
       }

     if(rightButtonState == GLFW_RELEASE) 
       {

           initial_rightClick  = true;

       } 
     
     if (leftButtonState == GLFW_PRESS) 
       {
      //  std::cout << "Left mouse button clicked! translation mode active " << '\n';

        if (initial_leftClick )
          {
            initialMouseX = mouse_x;
            initialMouseY = mouse_y;
            initial_leftClick  = false;
            translate_mouse_x = initialMouseX;
            translate_mouse_y = initialMouseY;

          }

          else 
           {   
               translate_mouse_x = mouse_x - initialMouseX;
               translate_mouse_y = mouse_y - initialMouseY;
           }
       }

        if(leftButtonState  == GLFW_RELEASE) 
          {

             initial_leftClick = true;

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

inline short int check_glfw_window(GLFWwindow* window) {
    if (!window) { std::cerr << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; } 
    else {std::cout << "Successfully created window" ; return 0; }
}    
//----------------------------------------------------------------------------------------------------------------------------+

int main()
{
    std::string filename = "/home/ubuntu-testing/wing.stl";
    // std::string filename = "/home/ubuntu-testing/GridPro.v8.0/doc/WS/Tutorials/Advanced/Tutorial_2_DLR_F6/fuselage.stl"; // Replace with your STL file path

    class Geometry geometry(filename);
    
    auto GeometryVertices = geometry.VERTEX_ARRAY_GL;

     for(uint32_t i = 0 ; i <  GeometryVertices.size() ; i+=6) {
       
       float Normalise_param = 1.0f;
       GeometryVertices[i]   = (GeometryVertices[i]/Normalise_param)  -0.5;
       GeometryVertices[i+1] = (GeometryVertices[i+1]/Normalise_param) -0.5;
       GeometryVertices[i+2] = (GeometryVertices[i+2]/Normalise_param) -0.5;   }

       std::cout << "STL num triangles = " << GeometryVertices.size()/2 << '\n';
/*
    for(uint32_t i = 0 ; i <  GeometryVertices.size() ; i+=6) {
 
        std::cout << "Vertex ID  =" << i/6 << '\n';
        std::cout << "coord  : "  << GeometryVertices[i]  << " " <<  GeometryVertices[i+1] << " "<<  GeometryVertices[i+2] <<  "\n" ;
        std::cout << "color  : "  << GeometryVertices[i+3]  << " " <<  GeometryVertices[i+4] << " "<<  GeometryVertices[i+5] <<  "\n" ;
    }


     GeometryVertices = {
    // Positions         // Colors

    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

     -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

     0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f

};

 std::cout << " Array triangles = " << GeometryVertices.size()/6<< '\n';


 for(uint32_t i = 0 ; i <  GeometryVertices.size() ; i+=6) {

        std::cout << "Vertex ID  =" << i/6 << '\n';
        std::cout << "coord  : "  << GeometryVertices[i]  << " " <<  GeometryVertices[i+1] << " "<<  GeometryVertices[i+2] <<  "\n" ;
        std::cout << "color  : "  << GeometryVertices[i+3]  << " " <<  GeometryVertices[i+4] << " "<<  GeometryVertices[i+5] <<  "\n" ;
    }
 */    
    //-------------------------Initialize GLFW---------------------------------------------------------+
     check_glfwInit();
    // -----------------------Set OpenGL version and profile to 3.3 core--------------------------------+

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //--------------------------Create a GLFW window----------------------------------------------------+

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Cube Rotation", nullptr, nullptr);
    // check_glfw_window(wi)
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
  //glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryVertices), GeometryVertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, GeometryVertices.size() * sizeof(GLfloat), &GeometryVertices[0], GL_STATIC_DRAW);


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
    
    bool intersection_started = false;
    GLfloat Nearest_Z_coord = 0.0f ;
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

        float gltime = glfwGetTime();
        
        float mouse_input = float((mouse_x/WIDTH + mouse_y/HEIGHT)*3.14);
        
        float set_axis_x =  10*((2.0*rotate_mouse_x/WIDTH)-1.0f) , set_axis_y = 10*(1.0f-(2.0*rotate_mouse_y/HEIGHT));
        
        //glm::mat4 view = glm::translate(glm::mat4(0.1f), glm::vec3((translate_mouse_x*2/WIDTH)-1.0f, (1.0f-(2.0f*translate_mouse_y/HEIGHT)), -3.0f));

        //-----------------------------------Translate + Zoom functionality-------------------------------------------------------------+
        //-------------------------X-axis-----------------Y-axis----------------Z-axis--------------------------------------------------+

        ndcX = (2.0f * mouse_x) / WIDTH - 1.0f  ; ndcY = 1.0f - (2.0f * mouse_y) / HEIGHT;

        auto X = (2.0f * translate_mouse_x ) / WIDTH - 1.0f  , Y = 1.0f - (2.0f * translate_mouse_y) / HEIGHT;

        cameraPos = glm::vec3(-right_key + left_key - X, - up_key + down_key - Y, 20.0f+ scroll_offset*6); //  use (-scroll_offset + 2.0f) for zooming but distrort perspective so dont use it (Remember this)
    
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
 
        //-------------------------------------------Rotate functionality---------------------------------------------------------------+

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (-set_axis_y + w_key - s_key ), glm::vec3(1.0f, 0.0f, 0.0f));
        
        model = glm::rotate(model, (-set_axis_x + d_key - a_key), glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f ), (float)WIDTH / (float)HEIGHT, 0.1f, 200.0f) ; // (Remember this)
    
       //--------------------------------Implement Mouse_rayClip_intersection-------------------------------------------------------------+
       
        glm::mat4 mvp = projection * view * model;     
        glm::vec3 intersectionPoint;





    /*    
    auto time = omp_get_wtime();   

     //   #pragma omp parallel for  
        
        for (size_t i = 0; i < GeometryVertices.size(); i += 18){

            
          
            glm::vec3 v0 (GeometryVertices[i],GeometryVertices[i+1], GeometryVertices[i+2] );
            glm::vec3 v1 (GeometryVertices[i+6],GeometryVertices[i+7], GeometryVertices[i+8]);
            glm::vec3 v2 (GeometryVertices[i+12],GeometryVertices[i+13], GeometryVertices[i+14] );

            glm::vec4 new_v0 = mvp * glm::vec4(v0, 1.0);
            glm::vec4 new_v1 = mvp * glm::vec4(v1, 1.0);
            glm::vec4 new_v2 = mvp * glm::vec4(v2, 1.0);
       
            glm::vec3 V0 = glm::vec3(new_v0);  
            glm::vec3 V1 = glm::vec3(new_v1);  
            glm::vec3 V2 = glm::vec3(new_v2);  
             
    //----------------------------For Mouse ray----------------------------------------// 
                     
            glm::vec3 rayOrigin;
            glm::vec3 rayDirection;
            
            rayOrigin[0] = ndcX;
            rayOrigin[1] = ndcY;
            rayOrigin[2] =  5.0f;

            //rayOrigin = cameraPos;
            rayDirection[0] = ndcX;
            rayDirection[1] = ndcY;
            rayDirection[2] = -10.0f;   
            
        //  std::cout << "Ray_Origin -> | " << rayOrigin[0]  << " : " << rayOrigin[1] <<   " : " << rayOrigin[2] << " | \n";
        //  std::cout << "Ray_Direction -> | " << rayDirection[0]  << " : " << rayDirection[1] <<   " : " << rayDirection[2] << " | \n ";
        //  std::cout << "MOuse pointer is at (" << ndcX << " , " << ndcY << " ) \n"  ; 
             
         //   if(isTriangleVisible(mvp , v0 , v1 , v2))    std::cout <<  "Triangle : " << triangleID << " is visible" << "\n";
            

            if (rayTriangleIntersect(rayOrigin, rayDirection, V0, V1, V2, intersectionPoint)) {
            // Intersection occurred, use intersectionPoint
            // ...
                GLuint triangleID = i / 18;
                
             if(!intersection_started)   
            {
                 intersection_started = true;
                 Nearest_Z_coord = intersectionPoint[2];
            }
            else
              {              
                if(intersectionPoint[2] <= Nearest_Z_coord)   {  
                   std::cout << "\nTriangle " << triangleID << " intersected at : ("  <<  intersectionPoint[0] << " , " << intersectionPoint[1]  << " , "<< intersectionPoint[2] << ")"<<  std::endl; 
                 }
                   Nearest_Z_coord = intersectionPoint[2];
             
             }

           }

        }  
    std::cout << "Time taken = " << omp_get_wtime() - time << "\n";   

    */

//----------------------------------------------------------------------------------------------------------------+


        //------------------------Pass transformation matrices to the shader-----------------------------+
                
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        //cameraPos = glm::vec3(-right_key + left_key - X, - up_key + down_key - Y, (-scroll_offset*4 + 2.0f) ); //  use (-scroll_offset + 2.0f) for zooming but distrort perspective so dont use it (Remember this)
    
        //view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));        

        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
       
        glUseProgram(shaderProgram);
       
        GLuint intersectionPointLocation = glGetUniformLocation(shaderProgram, "intersectionPoint");
       
        glUniform3fv(intersectionPointLocation, 1, glm::value_ptr(intersectionPoint));

        //---------------------------------Draw cube-------------------------------------------------------+

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, GeometryVertices.size()/6);
              
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
