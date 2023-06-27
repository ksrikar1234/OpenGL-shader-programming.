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



typedef uint64_t TRIANGLE_ID;
typedef GLfloat POINT;
typedef GLfloat COORD;
typedef GLfloat COLOUR_ATTRIBUTE;

typedef COORD  VERTEX[3];
typedef COORD  NORMAL[3];
typedef COLOUR_ATTRIBUTE COLOR[4] ;

class Triangle {

public:

    TRIANGLE_ID ID;
    NORMAL normal;
    VERTEX vertex1;
    VERTEX vertex2;
    VERTEX vertex3;
    COLOR  vertex1_color;
    COLOR  vertex2_color;
    COLOR  vertex3_color;
};

namespace Geometry {

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

//----------------------------------Reading Geometry from STL file Triangle definition-------------------------------------------+

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
        //this->triangles.reserve(numTriangles);

        // Read each triangle
        for (uint64_t i = 0; i < numTriangles; ++i) {
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
        uint64_t numTriangles = 0;
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
          
            for(auto coord : triangle.vertex1)        this->VERTEX_ARRAY_GL.push_back(coord); 

          
            this->VERTEX_ARRAY_GL.push_back(1.0f); 
            this->VERTEX_ARRAY_GL.push_back(0.0f); 
            this->VERTEX_ARRAY_GL.push_back(0.0f); 


            for(auto coord :  triangle.vertex2)       this->VERTEX_ARRAY_GL.push_back(coord); 
         
            this->VERTEX_ARRAY_GL.push_back(0.0f); 
            this->VERTEX_ARRAY_GL.push_back(1.0f); 
            this->VERTEX_ARRAY_GL.push_back(0.0f);

            for(auto coord :  triangle.vertex3)        this->VERTEX_ARRAY_GL.push_back(coord); 

            this->VERTEX_ARRAY_GL.push_back(1.0f); 
            this->VERTEX_ARRAY_GL.push_back(0.0f); 
            this->VERTEX_ARRAY_GL.push_back(1.0f);    

            ++id;
        }    
    
     return this->VERTEX_ARRAY_GL;         

}

};

}


//-----------------------------------------Perform Ray Triangle intersection Mollers intersection -------------------------------------------------+


inline bool rayTriangleIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
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

//---------------------------------------------check which triangles are present on the projection plane----------------------+

inline bool isTriangleVisible(const glm::mat4& mvp, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
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

//-----------------------------------Generate Mouse Ray ---------------------------------------------------------------+

inline glm::vec3 createMouseRay(GLfloat mouseX , GLfloat mouseY ,  const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix,
                    glm::vec3& rayOrigin, glm::vec3& rayDirection , GLuint WIDTH , GLuint HEIGHT)
{
    // Get the mouse cursor position in screen coordinates
    //float mouseX = mouse_x, mouseY = mouse_y;
    
    // Get the window dimensions

    // Convert screen coordinates to normalized device coordinates (NDC)
    float ndcX = (2.0f * mouseX) / WIDTH - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / HEIGHT;

    // Convert NDC to view-space coordinates
    glm::vec4 clipSpacePos = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
    glm::vec4 viewSpacePos = inverseProjection * clipSpacePos;
    //viewSpacePos.z = -1.0f;  // Set the z-component to -1 to make it perpendicular to the viewing plane
    
  //  viewSpacePos /= viewSpacePos.w;

    // Convert view-space coordinates to world-space coordinates
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec4 worldSpacePos = inverseView * viewSpacePos;
  //  worldSpacePos /= worldSpacePos.w;

    // Set the ray origin and direction
    rayOrigin = glm::vec3(inverseView[3]);    
    rayDirection = glm::normalize(glm::vec3(worldSpacePos) - rayOrigin);

    return glm::vec3(worldSpacePos);
}

