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

//namespace gs = Geometry;

//-----------------------------------------------Camera Params------------------------------------------------------------------+

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//------------------------------------------------------------------------------------------------------------------------------+

int main()
{
    //-------------------------Initialize GLFW---------------------------------------------------------+
     check_glfwInit();
    // -----------------------Set OpenGL version and profile to 3.3 core--------------------------------+

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //--------------------------Create a GLFW window----------------------------------------------------+
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow*  window = glfwCreateWindow(960, 540, __FILE__, NULL, NULL);
    //GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ray_intersection", nullptr, nullptr);
    check_glfw_window(window);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glfwSetWindowSizeLimits(window, WIDTH, HEIGHT, WIDTH, HEIGHT);  
 
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

    std::string filename = "/home/ubuntu-testing/CUBE.stl";
    
    // std::string filename = "/home/ubuntu-testing/GridPro.v8.0/doc/WS/Tutorials/Advanced/Tutorial_2_DLR_F6/fuselage.stl"; // Replace with your STL file path
    
    Geometry::Geometry geometry(filename);
    
    auto GeometryVertices = geometry.VERTEX_ARRAY_GL;

     for(uint32_t i = 0 ; i <  GeometryVertices.size() ; i+=6) {
       
       float Normalise_param = 1.0f;
       GeometryVertices[i]   = (GeometryVertices[i]/Normalise_param)   -0.5;
       GeometryVertices[i+1] = (GeometryVertices[i+1]/Normalise_param) -0.5;
       GeometryVertices[i+2] = (GeometryVertices[i+2]/Normalise_param) -0.5;   }

       std::cout << "STL num triangles = " << GeometryVertices.size()/2 << '\n';

    //-------------------Create vertex buffer object (VBO) and vertex array object (VAO)-------------------+
    
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    //--------Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attribute(s)---+
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

    /* 
    GLuint PBO;
    glGenBuffers(1, &PBO);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO);
    glBufferData(GL_PIXEL_PACK_BUFFER, WIDTH * HEIGHT * 4, nullptr, GL_STREAM_READ);
    */

    //-------------------------Enable depth testing---------------------------------------------------------+
   
    glEnable(GL_DEPTH_TEST);

    //----------------------fps counter variables-----------------------------------------------------------+

    double lastTime = glfwGetTime();
    int frameCount = 0;
    int fps = 0;
    
    //----------------------For calculating first & nearest intersection------------------------------------+
   
    bool intersection_started = false;
    GLfloat Nearest_Z_coord = 0.0f ;

    auto pixelData = [](int mouseX, int mouseY, GLubyte* pixelBuffer) {
       // Assuming your pixel format is RGBA and you're retrieving 1 pixel at a time
       

       int pixelIndex = ((mouse_y +50 )* 960 + (mouse_x-100))*4 ; // Adjust according to your screen width and pixel format
       GLubyte r = pixelBuffer[pixelIndex];
       GLubyte g = pixelBuffer[pixelIndex + 1];
       GLubyte b = pixelBuffer[pixelIndex + 2];
       GLubyte a = pixelBuffer[pixelIndex + 3];

       uint32_t i ;

       //for(auto k : pixelBuffer) i += 4; 
  
    // Do something with the pixel data
    std::cout << "Pixel at (" << mouse_x  << ", " << mouse_y << "): RGBA(" << static_cast<int>(r) << ", " << static_cast<int>(g) << ", " << static_cast<int>(b) << ", " << static_cast<int>(a) << ")" << std::endl;
};



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

       // glm::mat4 model = glm::rotate(glm::mat4(1.0f), (-set_axis_y + w_key - s_key ), glm::vec3(1.0f, 0.0f, 0.0f));     

       // model = glm::rotate(model, (-set_axis_x + d_key - a_key), glm::vec3(0.0f, 1.0f, 0.0f));
        
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

        glDrawArrays(GL_TRIANGLES, 0, GeometryVertices.size()/6);
       
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

        GLubyte* PixelData = new GLubyte[WIDTH * HEIGHT * 4];;
        glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, PixelData);
    
        //GLubyte* PixelData = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);  
        
        size_t bufferSize = strlen(reinterpret_cast<const char*>(PixelData)) + 1;

        std::cout << "NUM PIXELS = " << bufferSize << "\n" ;

        pixelData(mouse_x, mouse_y, PixelData);

        delete[] PixelData;

        PixelData = nullptr;
        //glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        //glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    
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

    // Clean up resources
  //  glDeleteBuffers(1, &PBO);
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
  //glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryVertices), GeometryVertices, GL_STATIC_DRAW);