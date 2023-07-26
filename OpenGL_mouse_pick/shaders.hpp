#include<iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//---------------------------Offscreen Rendering for face picking using  mouse--------------------------------------------------------------+ 

const char* OffScreenVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec3 normal;
 
    out vec3 outColor;
  
    uniform mat4 modelMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 projectionMatrix;


    void main()
    {
        gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
        outColor = aColor;
    }
)";


const char* OffScreenFragmentShaderSource = R"(

    #version 330 core
    in vec3 outColor;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(outColor, 1.0);
    }
)";

//-------------------------------------------------------Phongs Lighting algorithm ------------------------------------------------------------------------------------+

//--------------------------------------Onscreen rendering Vertex Shader with Lights -----------------------------------------------------------------------------+


const char* OnScreenVertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;


out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragLightDir;
out vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosition;

uniform vec3 selected_color;


void main()
{
    // Transform vertex position and normal to world space
    vec4 worldPosition = modelMatrix * vec4(position, 1.0);
    vec3 worldNormal = mat3(transpose(inverse(modelMatrix))) * normal;

    // Compute the light direction
    fragLightDir = normalize(lightPosition - worldPosition.xyz);

    // Pass data to the fragment shader
    fragNormal = worldNormal;
    fragPosition = worldPosition.xyz;
    
     vertexColor = vec3(0.5f, 0.5f , 0.5f );
     if(selected_color == color)
       {
          vertexColor = vec3(1.0f, 1.0f , 1.0f );
       }
    //vertexColor = color;

    // Transform the vertex position to clip space
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}
)";

//------------------------------------------On Screen rendering Fragment Shader----------------------------------------------------------------+

const char* OnScreenFragmentShaderSource = R"(

#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 fragLightDir;
in vec3 vertexColor;

out vec4 fragColor;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;



void main()
{
    // Compute the normal and light direction in fragment space
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(fragLightDir);

    // Compute the diffuse color
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = lightDiffuse * materialDiffuse * diffuseFactor;

    // Compute the specular color
    vec3 viewDir = normalize(-fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specularColor = lightSpecular * materialSpecular * specularFactor;

    // Compute the final color (ambient + diffuse + specular)
    vec3 ambientColor = lightAmbient * materialAmbient;
    vec3 finalColor = ambientColor + diffuseColor + specularColor;
    finalColor = finalColor * vertexColor;
 
    
    vec3 haloColor = vec3(1.0f , 1.0f , 1.0f);

    float haloRadius = 5;
   
    float distance = length(fragPosition);
    
    float haloFactor = 1.0 - smoothstep(haloRadius - 0.1, haloRadius + 0.1, distance);

    finalColor = finalColor * haloColor;
   
    fragColor = vec4(finalColor, haloFactor);
}

)";

//--------------------------------------------------------------------------------------------------------------------------------+




//-------------------------------Manage , Compile Shaders---------------------------------------------------------------------------------------------------+

GLuint  OnScreenVertexShader , OnScreenFragmentShader  , OffScreenVertexShader ,  OffScreenFragmentShader ;

GLuint  OnScreenShaderProgram , OffScreenShaderProgram  ;

inline void manage_shaders()
{

    GLint success;
    GLchar infoLog[512];
   
    //--------------------- Create  vertex shader for Onscreen rendering-------------------------------------+
    
    OnScreenVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(OnScreenVertexShader, 1, &OnScreenVertexShaderSource, nullptr);
    glCompileShader(OnScreenVertexShader);

    //----------------------------Check for vertex shader compile errors-------------------------------------+
   
    
    glGetShaderiv(OnScreenVertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(OnScreenVertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader for Onscreen rendering failed to compile  : \n" << infoLog << std::endl;
    }

    //-------------------------------Create fragment shader for Onscreen rendering-----------------------------------------------+

    OnScreenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(OnScreenFragmentShader, 1, &OnScreenFragmentShaderSource, nullptr);
    glCompileShader(OnScreenFragmentShader);

    //-----------------------Check for fragment shader compile errors-------------------------------------+

    glGetShaderiv(OnScreenFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(OnScreenFragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader for Onscreen rendering failed to compile :\n" << infoLog << std::endl;
    }

    //---------------------------Create shader program-----------------------------------------------------+
    
    OnScreenShaderProgram = glCreateProgram();
    glAttachShader(OnScreenShaderProgram, OnScreenVertexShader);
    glAttachShader(OnScreenShaderProgram, OnScreenFragmentShader);
    glLinkProgram(OnScreenShaderProgram);

    //---------------------------Check for shader program link errors--------------------------------------+
   
    glGetProgramiv(OnScreenShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(OnScreenShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Onscreen Shader program linking failed:\n" << infoLog << std::endl;
    }
   
   //---------------------------Delete shaders as they're linked into the program --------------------------+

     glDeleteShader(OnScreenVertexShader);
     glDeleteShader(OnScreenFragmentShader);


  //---------------------------- Create Vertex shader for OffScreen rendering-------------------------------+
    
    OffScreenVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(OffScreenVertexShader, 1, &OffScreenVertexShaderSource, nullptr);
    glCompileShader(OffScreenVertexShader);

    //-------------------------- Check for vertex shader compile errors-------------------------------------+
    
    glGetShaderiv(OffScreenVertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(OffScreenVertexShader, 512, nullptr, infoLog);
        std::cerr << "OffScreenVertex shader compilation failed:\n" << infoLog << std::endl;
    }

    //-------------------------------Create fragment shader-----------------------------------------------+

    OffScreenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(OffScreenFragmentShader, 1, &OffScreenFragmentShaderSource, nullptr);
    glCompileShader(OffScreenFragmentShader);

    //-----------------------Check for fragment shader compile errors-------------------------------------+

    glGetShaderiv(OffScreenFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(OffScreenFragmentShader, 512, nullptr, infoLog);
        std::cerr << "OffScreenFragment shader compilation failed:\n" << infoLog << std::endl;
    }

    //---------------------------Create shader program-----------------------------------------------------+
    
    OffScreenShaderProgram = glCreateProgram();
    glAttachShader(OffScreenShaderProgram, OffScreenVertexShader);
    glAttachShader(OffScreenShaderProgram, OffScreenFragmentShader);
    glLinkProgram(OffScreenShaderProgram);

    //---------------------------Check for shader program link errors--------------------------------------+
   
    glGetProgramiv(OffScreenShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(OffScreenShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }
   
   //---------------------------Delete shaders as they're linked into the program --------------------------+

     glDeleteShader(OffScreenVertexShader);
     glDeleteShader(OffScreenFragmentShader);
}

/*
..........................Basic Shader .............................................

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


const char* fragmentShaderSource = R"(

    #version 330 core
    in vec3 outColor;
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(outColor, 1.0);
    }
)";

...............Unique Colour Generation using Unique Triangle ID shader program ...........................................

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

.................................Actual logic ............................................

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

..........................vertex code that was supposed to work  .......................................................

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

...........................Multiple Light Sources..............................................
vertex shader code
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragLightDir;
out vec3 fragColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPositions[8]; // Array of light positions

void main()
{
    // Transform vertex position and normal to world space
    vec4 worldPosition = modelMatrix * vec4(position, 1.0);
    vec3 worldNormal = mat3(transpose(inverse(modelMatrix))) * normal;

    // Pass data to the fragment shader
    fragNormal = worldNormal;
    fragPosition = worldPosition.xyz;
    fragColor = color;

    // Compute the light directions for each light source
    for (int i = 0; i < 8; i++) {
        fragLightDir[i] = normalize(lightPositions[i] - worldPosition.xyz);
    }

    // Transform the vertex position to clip space
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}
................................................................................................
fragment shader code
................................................................................................

#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 fragLightDir[8]; // Array of light directions
in vec3 fragColor;

out vec4 fragOutput;

uniform vec3 lightAmbient[8]; // Array of light ambient colors
uniform vec3 lightDiffuse[8]; // Array of light diffuse colors
uniform vec3 lightSpecular[8]; // Array of light specular colors
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(-fragPosition);

    vec3 finalColor = vec3(0.0);

    for (int i = 0; i < 8; i++) {
        vec3 lightDir = normalize(fragLightDir[i]);

        float diffuseFactor = max(dot(normal, lightDir), 0.0);
        vec3 diffuseColor = lightDiffuse[i] * materialDiffuse * diffuseFactor;

        vec3 reflectDir = reflect(-lightDir, normal);
        float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        vec3 specularColor = lightSpecular[i] * materialSpecular * specularFactor;

        vec3 ambientColor = lightAmbient[i] * materialAmbient;
        finalColor += ambientColor + diffuseColor + specularColor;
    }

    // Multiply the final color with the vertex color
    vec3 outputColor = finalColor * fragColor;

    fragOutput = vec4(outputColor, 1.0);
}





//--------------Not tested Alternative shaders----------------------------------------------------------


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


//----------------------------------------------------------------------------------------------

*/
