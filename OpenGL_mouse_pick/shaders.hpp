#include<iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


//--------------------------------------Vertex Shader-----------------------------------------------------------------------------+


const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
   

    out vec3 outColor;

    uniform mat4 mvp;


    void main()
    {

        gl_Position = mvp*vec4(aPos, 1.0);  
           outColor = aColor;
    }
)";

//-------------------------------------------Fragment Shader----------------------------------------------------------------+

const char* fragmentShaderSource = R"(

    #version 330 core
    in vec3 outColor;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(outColor, 1.0);
    }
)";

//--------------------------------------------------------------------------------------------------------------------------------+


//-------------------------------Compile Shaders---------------------------------------------------------------------------------------------------+

GLuint vertexShader , fragmentShader ;

GLuint shaderProgram;

void compile_shaders()
{
   
    //---------------------------- Create vertex shader----------------------------------------------------+
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
    
    shaderProgram = glCreateProgram();
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

}

/*

.......................................................................

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 outColor;

    uniform mat4 mvp;


    void main()
    {
        gl_Position = mvp*vec4(aPos, 1.0);
        outColor = aColor;
    }
)";
.........................................................................

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in float TriangleID;

    out vec3 outColor;

    uniform mat4 mvp;

    void main()
    {        
    
        gl_Position = mvp*vec4(aPos, 1.0);
 
        float R = TriangleID/1640;

        float G = TriangleID/1640;
        
        float B = TriangleID/1640;

        outColor = vec3(R, G , B );
 
    }
)";

.............................................................................

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in uint TriangleID;

    out vec3 outColor;

    uniform mat4 mvp;

    void main()
    {        
    
        gl_Position = mvp*vec4(aPos, 1.0);

        TriangleID = TriangleID;

        GLuint R =  (TriangleID & 0b00000000000000000000000011111111); 
        GLuint G =  (TriangleID & 0b00000000000000001111111100000000) >> 8; 
        GLuint B =  (TriangleID & 0b00000000111111110000000000000000) >> 16;

        GLfloat r = float(R)/255.0f;
        GLfloat g = float(G)/255.0f;
        GLfloat b = float(B)/255.0f;

        outColor = vec3(r, g ,b );
 
    }
)";

.................................................................................

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in float TriangleID;

    out vec3 outColor;

    uniform mat4 mvp;

    void main()
    {        
    
        gl_Position = mvp*vec4(aPos, 1.0);
     
        int x  = int(TriangleID); 
        uint y =  uint(x);
        
        uint max = uint(0xff);

        uint r = y & max; 
        uint g = (y  >> 8) & max;
        uint b = (y  >> 16) & max; 
 
        float R = float(r)/max;
        float G = float(g)/max;
        float B = float(b)/max;


        outColor = vec3(R, G , B );
 
    }
)";

#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

out vec3 FragPosition;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPosition = vec3(model * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * vec4(FragPosition, 1.0);
}


#version 330 core

in vec3 FragPosition;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    vec3 lightDirection = normalize(lightPosition - FragPosition);
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(Normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDirection = normalize(-FragPosition);
    vec3 reflectDirection = reflect(-lightDirection, Normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}



*/
