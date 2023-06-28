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


*/
