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
typedef COLOUR_ATTRIBUTE COLOR[3] ;

struct Triangle {

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
//--------------------------------ASCII STL FILE READER-----------------------------------------------------------------------------------------------------------------+ 

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
 

//-----------------------------LOAD INTO OPENGL VERTEX BUFFER-----------------------------------------------+

std::vector<COORD> LOAD_GL_VERTEX_ARRAY()
{
   
 //  std::cout << "loading vertices for OpenGL vertex array \n" ;

   GLuint TriangleID = 1;   
 
   for(auto triangle : this->triangles)
       {
                      
           GLuint R =  (TriangleID & 0b00000000000000000000000011111111); 
           GLuint G =  (TriangleID & 0b00000000000000001111111100000000) >> 8; 
           GLuint B =  (TriangleID & 0b00000000111111110000000000000000) >> 16;

           std::cout << R  << " " << G << " " << B << " " << '\n';

           GLfloat r = float(R)/255.0f;
           GLfloat g = float(G)/255.0f;
           GLfloat b = float(B)/255.0f;
     
           std::cout << r  << " " << g << " " << b << " " << '\n';

         //---------------------------------------------------------------------------------------------------      

           for( auto coordinate : triangle.vertex1 )    this->VERTEX_ARRAY_GL.push_back(coordinate/100); 
            
           this->VERTEX_ARRAY_GL.push_back(r);
           this->VERTEX_ARRAY_GL.push_back(g);
           this->VERTEX_ARRAY_GL.push_back(b);
          
           for( auto norm_dir : triangle.normal )       this->VERTEX_ARRAY_GL.push_back(norm_dir);
    //---------------------------------------------------------------------------------------------------

           for( auto coordinate : triangle.vertex2 )    this->VERTEX_ARRAY_GL.push_back(coordinate/100); 
           
           this->VERTEX_ARRAY_GL.push_back(r);
           this->VERTEX_ARRAY_GL.push_back(g);
           this->VERTEX_ARRAY_GL.push_back(b);

           for( auto norm_dir : triangle.normal )       this->VERTEX_ARRAY_GL.push_back(norm_dir);

    //---------------------------------------------------------------------------------------------------        
       
           for( auto coordinate : triangle.vertex3 )    this->VERTEX_ARRAY_GL.push_back(coordinate/100); 
               
           this->VERTEX_ARRAY_GL.push_back(r);
           this->VERTEX_ARRAY_GL.push_back(g);
           this->VERTEX_ARRAY_GL.push_back(b);

           for( auto norm_dir : triangle.normal )       this->VERTEX_ARRAY_GL.push_back(norm_dir);

    //---------------------------------------------------------------------------------------------------        

           ++TriangleID;       
        }    
    
    std::cout << "Triangle Num = " << TriangleID << "\n";

    return this->VERTEX_ARRAY_GL;         

}

};

}

