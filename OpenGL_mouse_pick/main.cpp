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
#include "shaders.hpp"


#include "Geometry/Geometry_Data_handling.hpp"
#include "Geometry/Geometry_3DMath.hpp"


//------------------------------------------------------------------------------------------------------------------------------+

int main()
{

   //----------------------------LOAD & SET UP geometry ----------------------------------------------------------+

    std::string filename = "/home/ubuntu-testing/fuselage.stl";
    
    // std::string filename = "/home/ubuntu-testing/GridPro.v8.0/doc/WS/Tutorials/Advanced/Tutorial_2_DLR_F6/fuselage.stl"; // Replace with your STL file path
    
    Geometry::Geometry geometry(filename);

    std::cout << "Number of triangles = " << geometry.VERTEX_ARRAY_GL.size()/27 << "\n";

    //--------------------------------Initialize GLFW----------------------------------------------------------------------------+
      check_glfwInit();
    //--------------------------------Set OpenGL Profile-------------------------------------------------------------------------+
      set_opengl_version();
    //--------------------------------Create a GLFW window-----------------------------------------------------------------------+  
      create_window();  
    //--------------------------------Peripherals input handling-----------------------------------------------------------------+
      set_up_peripheral_inputs();
    //--------------------------------Initialize GLEW----------------------------------------------------------------------------+
      check_glewInit();
    //--------------------------------Manage, Compile, link & Delete shaders-----------------------------------------------------+
      manage_shaders();
    //--------------------------------Create vertex buffer object (VBO) and vertex array object (VAO)----------------------------+   
       GLuint VBO, VAO;
       glGenBuffers(1, &VBO);
       glGenVertexArrays(1, &VAO);
    //--------------------------------Bind VAO first, then bind and set vertex buffer(s) & then configure vertex attribute(s)----+ 

       glBindVertexArray(VAO);
       glBindBuffer(GL_ARRAY_BUFFER, VBO);
       glBufferData(GL_ARRAY_BUFFER, geometry.VERTEX_ARRAY_GL.size() * sizeof(GLfloat), &geometry.VERTEX_ARRAY_GL[0], GL_STATIC_DRAW);

    //-------------------------------Position attribute--------------------------------------------------------------------------+
   
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat) , (void*)0);
       glEnableVertexAttribArray(0);

    //------------------------color attribute-------------------------------------------------------------------------------------+
    
       glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat) , (void*)(3 * sizeof(GLfloat)));
       glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------------------------------------------------+
           
       glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat) , (void*)(6 * sizeof(GLfloat)));
       glEnableVertexAttribArray(2);

    //---------------------Unbind VBO and VAO---------------------------------------------------------------+
    
       glBindBuffer(GL_ARRAY_BUFFER, 0);
       glBindVertexArray(0);    

    //-------------------------Enable depth testing---------------------------------------------------------+
   
       glEnable(GL_DEPTH_TEST);
 
    //----------------------fps counter variables-----------------------------------------------------------+

      double lastTime = glfwGetTime();
      int frameCount = 0;
      int fps = 0;   
      
    //---------------------------------Render loop------------------------------------------------------------+        
    
      while (!glfwWindowShouldClose(window))
            {

                 //------------------------------Input handling------------------------------------------------+
         
                  processInput(window);

                //-----------------------------Clear the screen----------------------------------------------------+

                   glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
                   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                //--------------------------Activate shader program------------------------------------------------+
        
                   glUseProgram(OnScreenShaderProgram);

               //---------------------------Create transformations------------------------------------------------+
               //---------------------------Translate + Rotate + Zoom functionality-------------------------------+

                   setup_transformations();
       
               //------------------------Pass transformation matrices to the shader-------------------------------+

                  glm::mat4 mvp = projection * view * model; 
                  
                  GLint projectionLoc = glGetUniformLocation(OnScreenShaderProgram, "projectionMatrix");
                  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

                  GLint viewLoc = glGetUniformLocation(OnScreenShaderProgram, "viewMatrix");
                  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

                  GLint modelLoc = glGetUniformLocation(OnScreenShaderProgram, "modelMatrix");
                  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


/*
                  GLint mvpLoc = glGetUniformLocation(OnScreenShaderProgram, "mvp");
                  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
 */  
               //-------------------------------------------------------------------------------------------------+

                 // Get the location of the lightPosition uniform variable
                 GLuint lightPositionLocation = glGetUniformLocation(OnScreenShaderProgram, "lightPosition");

                 // Set the value of the lightPosition uniform variable
                 glm::vec3 lightPosition(10.0f + g_key - j_key , 2.0f + y_key - h_key , 0.0f + o_key - p_key); // movable light source


                 glUniform3f(lightPositionLocation, lightPosition.x, lightPosition.y, lightPosition.z);
                
                 GLuint lightAmbientLocation = glGetUniformLocation(OnScreenShaderProgram, "lightAmbient");
                 GLuint lightDiffuseLocation = glGetUniformLocation(OnScreenShaderProgram, "lightDiffuse");
                 GLuint lightSpecularLocation = glGetUniformLocation(OnScreenShaderProgram, "lightSpecularLocation");

                 GLuint materialAmbientLocation = glGetUniformLocation(OnScreenShaderProgram, "materialAmbient");
                 GLuint materialDiffuseLocation = glGetUniformLocation(OnScreenShaderProgram, "materialDiffuse");
                 GLuint materialSpecularLocation = glGetUniformLocation(OnScreenShaderProgram, "materialSpecular");
                 GLuint materialShininessLocation = glGetUniformLocation(OnScreenShaderProgram, "materialShininess");

                  // Set the lighting properties
                  glUniform3f(lightAmbientLocation, 0.5f, 0.5f, 0.5f);
                  glUniform3f(lightDiffuseLocation, 0.8f, 0.8f, 0.8f);
                  glUniform3f(lightSpecularLocation, 1.0f, 1.0f, 1.0f);

                 // Set the material properties
                  glUniform3f(materialAmbientLocation, 0.2f, 0.2f, 0.2f);
                  glUniform3f(materialDiffuseLocation, 0.8f, 0.8f, 0.8f);
                  glUniform3f(materialSpecularLocation, 1.0f, 1.0f, 1.0f);
                  glUniform1f(materialShininessLocation, 64.0f);

               //---------------------------------Draw call-------------------------------------------------------+
       
                  glBindVertexArray(VAO);
                  glDrawArrays(GL_TRIANGLES, 0, geometry.VERTEX_ARRAY_GL.size()/6);       
        
              //------------------------------Unbind the VAO------------------------------------------------------+  

               //  glBindVertexArray(0);

              //------------------------------Swap buffers and poll events----------------------------------------+
               
                 glfwSwapBuffers(window);
                 glfwPollEvents();
       
              //--------------------------------Retrive Frame Buffers---------------------------------------------+

                 GLint viewport[4];
                 GLubyte PixelData[4];
 
                 glGetIntegerv(GL_VIEWPORT , viewport); 
                 glReadPixels(mouse_x, viewport[3] - mouse_y -1 , 1 , 1, GL_RGBA, GL_UNSIGNED_BYTE, PixelData);


                 GLubyte r = PixelData[0];
                 GLubyte g = PixelData[1];
                 GLubyte b = PixelData[2];
                 GLubyte a = PixelData[3];
  

                 // Do something with the pixel data

                 GLuint Selected_Triangle_ID = 0;

                 Selected_Triangle_ID = ((Selected_Triangle_ID + b) << 8); 
                 Selected_Triangle_ID = ((Selected_Triangle_ID + g) << 8);
                 Selected_Triangle_ID =  (Selected_Triangle_ID + r) ;

                 std::cout << "Pixel at (" << mouse_x  << ", " << mouse_y << "): RGBA(" << static_cast<unsigned int>(r) << ", " << static_cast<unsigned int>(g) << ", " << static_cast<unsigned int>(b) << ", " << static_cast<unsigned int>(a) << ")" << std::endl;
       
                 std::cout << "Selected Triangle ID = " << Selected_Triangle_ID  << "\n"; 


                // std::cout << "selected_color_vec = (" << R <<", "<< G << ", " << B << ")" << '\n'; 
                 
                 //----------------Send The Necessary Uniform Variables to shader--------------------------------+
               /* 
                    GLint uniformLocation = glGetUniformLocation(shaderProgram, "selected_color_vec");

                    auto _ID = Selected_Triangle_ID;
                    GLuint R =  (_ID & 0b00000000000000000000000011111111); 
                    GLuint G =  (_ID & 0b00000000000000001111111100000000) >> 8; 
                    GLuint B =  (_ID & 0b00000000111111110000000000000000) >> 16;

                   // Set the uniform value

                  GLfloat red = float(R)/255.0f;
                  GLfloat green = float(G)/255.0f;
                  GLfloat blue = float(B)/255.0f;
              
                  glUniform3f(uniformLocation, red, green, blue);  
              */

                //-----------------------------Fps counter------------------------------------------------------+


                  glUseProgram(0);

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
    glDeleteProgram(OnScreenShaderProgram);
    glDeleteProgram(OffScreenShaderProgram);
    glfwTerminate();
    return 0;
}


