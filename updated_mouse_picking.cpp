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



//--------------------------------------Vertices-----------------------------------------------------------------------------------+

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

     if(rightButtonState = GLFW_RELEASE) 
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

        if(leftButtonState  = GLFW_RELEASE) 
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

        float time = glfwGetTime();
        
        float mouse_input = float((mouse_x/WIDTH + mouse_y/HEIGHT)*3.14);
        
        float set_axis_x =  10*((2.0*rotate_mouse_x/WIDTH)-1.0f) , set_axis_y = 10*(1.0f-(2.0*rotate_mouse_y/HEIGHT));
        
        //glm::mat4 view = glm::translate(glm::mat4(0.1f), glm::vec3((translate_mouse_x*2/WIDTH)-1.0f, (1.0f-(2.0f*translate_mouse_y/HEIGHT)), -3.0f));

        //-----------------------------------Translate + Zoom functionality-------------------------------------------------------------+
        //-------------------------X-axis-----------------Y-axis----------------Z-axis--------------------------------------------------+

        ndcX = (2.0f * mouse_x) / WIDTH - 1.0f  ; ndcY = 1.0f - (2.0f * mouse_y) / HEIGHT;

        auto X = (2.0f * translate_mouse_x) / WIDTH - 1.0f  , Y = 1.0f - (2.0f * translate_mouse_y) / HEIGHT;


        cameraPos = glm::vec3(-right_key + left_key - X, - up_key + down_key - Y,  2.0f); //  use (-scroll_offset + 2.0f) for zooming
    
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
 
        //-------------------------------------------Rotate functionality---------------------------------------------------------------+

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (-set_axis_y + w_key - s_key), glm::vec3(1.0f, 0.0f, 0.0f));
        
        model = glm::rotate(model, (-set_axis_x + d_key - a_key), glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f + scroll_offset*2), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        
 
       //--------------------------------Implement Mouse_rayClip_intersection-------------------------------------------------------------+
       
        glm::mat4 mvp = projection * view * model;     
        glm::vec3 intersectionPoint;
    
       // #pragma omp parallel for 
        for (size_t i = 0; i < GeometryVertices.size(); i += 18){

            GLuint triangleID = i / 18;
          
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
            rayOrigin[2] =  10.0f;

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
                
             if(!intersection_started)   
            {
                 intersection_started = true;
                 Nearest_Z_coord = intersectionPoint[2];
            }
            else
              {              
                if(intersectionPoint[2] <= Nearest_Z_coord)   {  
                   std::cout << "\nTriangle " << triangleID << " intersected at : ("  <<  intersectionPoint[0] << " , " << intersectionPoint[1]  << " , "<< intersectionPoint[2] << ")"<<  std::endl; }
 
                   Nearest_Z_coord = intersectionPoint[2];
             
             }

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
        glDrawArrays(GL_POINTS ,36,37);

              
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

