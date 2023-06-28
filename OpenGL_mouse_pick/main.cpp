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

#include "OpenGL_helper.hpp"
#include "Geometry.hpp"



//-----------------------------------------------Camera Params------------------------------------------------------------------+

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//------------------------------------------------------------------------------------------------------------------------------+

int main()
{
    std::string filename = "/home/ubuntu-testing/wing.stl";
    
    // std::string filename = "/home/ubuntu-testing/GridPro.v8.0/doc/WS/Tutorials/Advanced/Tutorial_2_DLR_F6/fuselage.stl"; // Replace with your STL file path
    
    Geometry::Geometry geometry(filename);
    
    geometry.VERTEX_ARRAY_GL;

    std::cout << "Number of triangles = " << geometry.VERTEX_ARRAY_GL.size()/18 << "\n";

    //-------------------------Initialize GLFW---------------------------------------------------------+
     check_glfwInit();
    // -----------------------Set OpenGL version and profile to 3.3 core--------------------------------+

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //--------------------------Create a GLFW window----------------------------------------------------+
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
   // GLFWwindow*  window = glfwCreateWindow(960, 540,  "Triangle Picking", NULL, NULL);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ray_intersection", nullptr, nullptr);
    check_glfw_window(window);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    //glfwSetWindowSizeLimits(window, WIDTH, HEIGHT, WIDTH, HEIGHT);  
 
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

   //----------------------------set up geometry ----------------------------------------------------------+




    //-------------------Create vertex buffer object (VBO) and vertex array object (VAO)-------------------+   
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    //--------Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attribute(s)---+
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, geometry.VERTEX_ARRAY_GL.size() * sizeof(GLfloat), &geometry.VERTEX_ARRAY_GL[0], GL_STATIC_DRAW);

    //---------------------Position attribute--------------------------------------------------------------+
   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat) , (void*)0);
    glEnableVertexAttribArray(0);

    //------------------------color attribute--------------------------------------------------------------+
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat) , (void*)(3 * sizeof(GLfloat)));
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
    
    //----------------------For calculating first & nearest intersection------------------------------------+
   
    bool intersection_started = false;
    GLfloat Nearest_Z_coord = 0.0f ;

    //---------------------------------Render loop---------------------------------------------------------+        
    
    while (!glfwWindowShouldClose(window))
    {
        //------------------------------Input handling-----------------------------------------------------+
        
        processInput(window);

        //-----------------------------Clear the screen----------------------------------------------------+

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //--------------------------Activate shader program------------------------------------------------+
        
        glUseProgram(shaderProgram);

        //---------------------------Create transformations------------------------------------------------+

        float gltime = glfwGetTime();
        
        float mouse_input = float((mouse_x/WIDTH + mouse_y/HEIGHT)*3.14);
        
        float set_axis_x =  10*((2.0*rotate_mouse_x/WIDTH)-1.0f) , set_axis_y = 10*(1.0f-(2.0*rotate_mouse_y/HEIGHT));
        
        //glm::mat4 view = glm::translate(glm::mat4(0.1f), glm::vec3((translate_mouse_x*2/WIDTH)-1.0f, (1.0f-(2.0f*translate_mouse_y/HEIGHT)), -3.0f));

        ndcX = (2.0f * mouse_x) / WIDTH - 1.0f  ; ndcY = 1.0f - (2.0f * mouse_y) / HEIGHT;

        auto X = (2.0f * translate_mouse_x ) / WIDTH - 1.0f  , Y = 1.0f - (2.0f * translate_mouse_y) / HEIGHT;

        //-----------------------------------Translate + Zoom functionality-------------------------------------------------------------+
        //-------------------------X-axis-----------------Y-axis----------------Z-axis--------------------------------------------------+

        cameraPos = glm::vec3(-right_key + left_key - X, - up_key + down_key - Y, 20.0f - scroll_offset*6); //  use (-scroll_offset + 2.0f) for zooming but distorts mouse ray  so dont use it (Remember this)
    
        float time = glfwGetTime();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        view = glm::rotate(view, (-set_axis_y + w_key - s_key ), glm::vec3(1.0f, 0.0f, 0.0f));

        view = glm::rotate(view, (-set_axis_x + d_key - a_key), glm::vec3(0.0f, 1.0f, 0.0f));

        view = glm::rotate(view, (-set_axis_x + l_key - k_key ), glm::vec3(0.0f, 0.0f, 1.0f));

        //-------------------------------------------Rotate functionality---------------------------------------------------------------+

        glm::mat4 model = glm::mat4(1.0f);
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f ), (float)WIDTH / (float)HEIGHT, 0.1f, 200.0f) ; // (Remember this)
    
       //--------------------------------Implement Mouse_rayClip_intersection-------------------------------------------------------------+
       
        glm::mat4 mvp = projection * view * model;     
        glm::vec3 intersectionPoint;

       //------------------------Pass transformation matrices to the shader-------------------------------+

        GLint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        glUseProgram(shaderProgram);       
       
       //---------------------------------Draw call-------------------------------------------------------+
       
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, geometry.VERTEX_ARRAY_GL.size()/6);       
       
        //------------------------------Unbind the VAO------------------------------------------------------+        
        glBindVertexArray(0);
        //------------------------------Swap buffers and poll events----------------------------------------+
        glfwSwapBuffers(window);
        glfwPollEvents();
        //--------------------------------Retrive Frame Buffers---------------------------------------------+
        /*
        glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO);
        glReadBuffer(GL_FRONT);
        */

        GLint viewport[4];
        GLubyte PixelData[4];

        glGetIntegerv(GL_VIEWPORT , viewport); 

        glReadPixels(mouse_x, viewport[3] - mouse_y -1 , 1 , 1, GL_RGBA, GL_UNSIGNED_BYTE, PixelData);

        GLubyte r = PixelData[0];
        GLubyte g = PixelData[1];
        GLubyte b = PixelData[2];
        GLubyte a = PixelData[3];
  

       // Do something with the pixel data
       uint32_t Selected_Triangle_ID = 0;

       Selected_Triangle_ID = ((Selected_Triangle_ID + b) << 8); 
       Selected_Triangle_ID = ((Selected_Triangle_ID + g) << 8);
       Selected_Triangle_ID =  (Selected_Triangle_ID + r) ;

       std::cout << "Pixel at (" << mouse_x  << ", " << mouse_y << "): RGBA(" << static_cast<unsigned int>(r) << ", " << static_cast<unsigned int>(g) << ", " << static_cast<unsigned int>(b) << ", " << static_cast<unsigned int>(a) << ")" << std::endl;
       
       std::cout << "Selected Triangle ID = " << Selected_Triangle_ID  << "\n"; 
       
 
    
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
        std::cout << "FPS: " << fps << std::endl;

        //--------------------------------------------------------------------------------------------------+
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}


