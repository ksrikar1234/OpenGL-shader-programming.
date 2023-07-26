#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


//-----------------------------------GLFW & GLEW Initiation----------------------------------------------------------------------+

 

      //------------------------------------Screen Resolution-----------------------------------------------------+
     
         GLFWwindow* window;
         const GLuint SCR_WIDTH = 800*1.8, SCR_HEIGHT = 600*1.8;

      //-------------------glfw window creation , checks , destruction Blah blah blah-----------------------------+

        inline void framebuffer_size_callback(GLFWwindow* window, int SCR_WIDTH, int SCR_HEIGHT)
        {
           glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        }
        inline void processInput(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        }
        inline short int check_glfwInit() 
        {
            if (!glfwInit()) {std::cerr << "Failed to initialize GLFW" << '\n';     return -1;  }
            else {std::cout << "Successfully initialised GLFW" << '\n'; return  0;  } 
        } 
       inline short int check_glfw_window(GLFWwindow* window) 
       {
           if (!window) { std::cerr << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; } 
           else {std::cout << "Successfully created GLFW window" << std::endl ; return 0; }
       }    
      
     //--------------------------Create a GLFW window-------------------------------------------------------------+
     
      inline void create_window() 
       {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);   // Enable this for Fixing WINDOW SIZE
           window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangle Picking with lighting effects", NULL, NULL);
            glfwSetWindowSizeLimits(window, SCR_WIDTH, SCR_HEIGHT, SCR_WIDTH, SCR_HEIGHT); // MIN MAX of WINDOW SIZE
           check_glfw_window(window);
           glfwMakeContextCurrent(window);
           glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);      
       }
    
     //--------------------------------------------------------------------------------------------------+

     // -----------------------Set OpenGL version and profile to 3.3 core--------------------------------+

      inline void set_opengl_version()
       {
         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


       }

      inline short int  check_glewInit() 
       {
         if (glewInit() != GLEW_OK)  {std::cerr << "Failed to initialize GLEW" << std::endl; glfwTerminate(); return -1; }
         else { std::cout << "Successfully initialised GLEW \n" ; return 0;}
       }


//---------------------------------------------------------------------------------------------------------------------------------------------+
//+------------------------------Globals for Peripheral inputs inputs--------------------------------------------------------------------------+

float mouse_x  = 0.0f , mouse_y = 0.0f;

float ndcX , ndcY ;

float initialMouseX = 0.0f ,  initialMouseY = 0.0f;

int rightButtonState , leftButtonState;  

float translate_mouse_x = 0.0f , translate_mouse_y = 0.0f ;

float rotate_mouse_x = 0.0f  , rotate_mouse_y = 0.0f;

float scroll_offset = 0.0f;

float up_key = 0.0f , down_key = 0.0f , left_key = 0.0f , right_key = 0.0f;

float w_key = 0.0f , a_key = 0.0f , s_key = 0.0f , d_key = 0.0f , k_key = 0.0f , l_key = 0.0f; // For camera rotation movement [ +-x , +-y , +-z ]

float y_key = 0.0f , h_key = 0.0f , g_key = 0.0f , j_key = 0.0f  , o_key = 0.0f , p_key = 0.0f; // For light source movement 

bool space_key = false;
 
//--------------------------------------------------Peripheral input handling--------------------------------------------------------------+


inline void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Print mouse position to console
    //std::cout << "Mouse position: " << xpos << ", " << ypos << std::endl;
    
    mouse_x = xpos;
    mouse_y = ypos;
   
    bool initial_rightClick = true;
    bool initial_leftClick = true;

    // float ndcX = (2.0f * mouse_x) / SCR_WIDTH - 1.0f;
    // float ndcY = 1.0f - (2.0f * mouse_y) / SCR_HEIGHT;

    rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    leftButtonState  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    

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
               rotate_mouse_x += mouse_x - initialMouseX;
               rotate_mouse_y += mouse_y - initialMouseY;
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
            initial_leftClick =  false;
            translate_mouse_x =  initialMouseX;
            translate_mouse_y =  initialMouseY;
          }

          else 
           {   
               translate_mouse_x += mouse_x - initialMouseX;
               translate_mouse_y += mouse_y - initialMouseY;
           }
       }

        else if(leftButtonState  == GLFW_RELEASE) 
          {

            initialMouseX = translate_mouse_x;
            initialMouseY = translate_mouse_y;
            initial_leftClick  = true;
    
          }       
}

//-------------------------SCROLL INPUT HANDLING -------------------------------------------------+

inline void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    // Handle scroll input here
    // xOffset and yOffset represent the scroll offset values
    // xOffset indicates horizontal scrolling (e.g., trackpad swipe left/right)
    // yOffset indicates vertical scrolling (e.g., mouse wheel up/down)
    // Usage : Zoom functionality . Print the scroll offsets


    scroll_offset += float(yOffset)/10 ;

    std::cout << "Scroll: xOffset = " << xOffset << ", yOffset = " << scroll_offset << std::endl;
}

//-----------------------Keyboard Input------------------------------------------------------------+

inline void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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

        std::cout << "W key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_A && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle A arrow key press
        // Example: Move the camera or perform some action
         
         a_key += 0.05f;

        std::cout << "A key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_S && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle S arrow key press
        // Example: Move the camera or perform some action
         
         s_key += 0.05f;

        std::cout << "S key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_D && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle D key press
        // Example: Move the camera or perform some action
         
         d_key += 0.05f;

        std::cout << "D key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_K && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle K key press
        // Example: Move the camera or perform some action
         
         k_key += 0.05f;

        std::cout << " K key pressed" << std::endl;
    }
       else if (key == GLFW_KEY_L && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the camera or perform some action
         
         l_key += 0.05f;

        std::cout << " L key pressed" << std::endl;
    } 
    else if (key == GLFW_KEY_H && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the light 
         
         y_key += 0.8f;

        std::cout << " Y key pressed" << std::endl;
    } 

     else if (key == GLFW_KEY_Y && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the camera or perform some action
         
         h_key += 0.8f;

        std::cout << " H key pressed" << std::endl;
    } 
        else if (key == GLFW_KEY_G && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the light
         
         g_key += 0.8f;

        std::cout << " H key pressed" << std::endl;
    } 
        else if (key == GLFW_KEY_J && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the light
         
         j_key += 0.8f;

        std::cout << " H key pressed" << std::endl;
    } 
 
        else if (key == GLFW_KEY_O && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the light
         
         o_key += 0.8f;

        std::cout << " O key pressed" << std::endl;
    } 

        else if (key == GLFW_KEY_P && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the light
         
         p_key += 0.8f;

        std::cout << " p key pressed" << std::endl;
    } 

 else if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT  ||  action == GLFW_PRESS)) {
        // Handle L key press
        // Example: Move the light
         
         space_key = true;

        std::cout << " space key pressed" << std::endl;
    }

    else if (key == GLFW_KEY_SPACE && (action == GLFW_RELEASE) )
    {
        // Handle  key press
        // Example: Move the light
         
         space_key = false;

        std::cout << " space key released" << std::endl;
    }

}


inline void set_up_peripheral_inputs()
{
      glfwSetCursorPosCallback(window, mousePositionCallback);
      glfwSetScrollCallback(window, scrollCallback);
      glfwSetKeyCallback(window, keyCallback);
}