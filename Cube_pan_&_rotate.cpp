#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<vector>


const GLuint WIDTH = 800, HEIGHT = 600;


//+------------------------------Globals for Peripheral inputs inputs--------------------------------------------------------------------------+

double mouse_x  = 0.0 , mouse_y = 0.0;

float translate_mouse_x = 0.0 , translate_mouse_y = 0.0 ;

float scroll_offset = 0.0;

float up_key = 0.0f , down_key = 0.0f , left_key = 0.0f , right_key = 0.0f;



//---------------------------------------------------------------------------------------------------------------------------------+

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

//--------------------------------------------------Peripheral input handling---------------------------------------------------+

void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Print mouse position to console
    //std::cout << "Mouse position: " << xpos << ", " << ypos << std::endl;
    
    
    int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    int leftButtonState  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    
    if (rightButtonState == GLFW_PRESS) 
       {
       // std::cout << "Right mouse button clicked! Rotation mode active " << '\n';
        mouse_x = xpos;
        mouse_y = ypos;
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
        
        float set_axis_x =  10*((2.0*mouse_x/WIDTH)-1.0f) , set_axis_y = 10*(1.0f-(2.0*mouse_y/HEIGHT));
        
        // glm::mat4 view = glm::translate(glm::mat4(0.1f), glm::vec3((translate_mouse_x*2/WIDTH)-1.0f, (1.0f-(2.0f*translate_mouse_y/HEIGHT)), -3.0f));

        cameraPos = glm::vec3(-right_key+left_key, - up_key + down_key, -scroll_offset+2.0f);
         
        //--------------------------Translate + Zoom functionality-----------------------------------------------+
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        view = glm::translate(view, glm::vec3(4*((translate_mouse_x*2.0f/WIDTH)-1.0f), 4*(1.0f-(2.0f*translate_mouse_y/HEIGHT)), -3.0f)); 
 
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), -set_axis_y, glm::vec3(1.0f, 0.0f, 0.0f));
        
        model = glm::rotate(model, -set_axis_x, glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        
    //  glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f)); 
      
    //-----------------------------------------------------------------------------------------------------------+
       
        for(unsigned short i = 0 ; i < 4 ; ++i) 
            {
               for(unsigned short j = 0 ; j < 4 ; ++j) ;// std::cout << model[i][j] << ',';

              // std::cout << '\n';
            }


        //------------------------Pass transformation matrices to the shader--------------------------------+
                
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //---------------------------------Draw cube-------------------------------------------------------+

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //------------------------------------Draw Cube 2--------------------------------------------------+

        //  modelLoc = glGetUniformLocation(shaderProgram, "model");

        //  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

        //  glDrawArrays(GL_TRIANGLES, 0, 36);


        //------------------------------Unbind the VAO------------------------------------------------------+

        glBindVertexArray(0);

        glm::mat4 mvp = projection * view * model;

        /*
        for (size_t i = 0; i < cubeIndices.size(); i += 3){

            GLuint triangleID = i / 3;
            glm::vec3 v0 = glm::make_vec3(&cubeVertices[cubeIndices[i] * 3]);
            glm::vec3 v1 = glm::make_vec3(&cubeVertices[cubeIndices[i + 1] * 3]);
            glm::vec3 v2 = glm::make_vec3(&cubeVertices[cubeIndices[i + 2] * 3]);

            if (isTriangleVisible(mvp, v0, v1, v2))
                std::cout << "Triangle " << triangleID << " is visible." << std::endl;
        
        }
        */
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

