#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

typedef unsigned long long TRIANGLE_ID;
typedef GLfloat POINT;
typedef GLfloat COORD;
typedef GLfloat COLOR;
typedef COORD VERTEX[3];
typedef COORD NORMAL[3];


class Triangle {

public:

    NORMAL normal;
    VERTEX verticies[3];
    VERTEX vertex1;
    VERTEX vertex2;
    VERTEX vertex3;
    COLOR  vertex1_color;
    COLOR  vertex2_color;
    COLOR  vertex3_color;


};



class Geometry {

public:
std::vector<Triangle> triangles;

Geometry(const std::string& filename = "NOT_SET")
{
  this->readSTLFile(filename);
}
   
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
        this->triangles.reserve(numTriangles);

        // Read each triangle
        for (uint32_t i = 0; i < numTriangles; ++i) {
            Triangle triangle;

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
        std::regex facetPattern(R"(facet normal ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+))");
        std::regex vertexPattern(R"(vertex ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+) ([-+]?[0-9]*\.?[0-9]+))");

        std::smatch match;
        std::string::const_iterator searchStart(fileContents.cbegin());
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

            this->triangles.push_back(triangle);
            searchStart = match.suffix().first;
        }

       
    }

  

file.close();
return this->triangles;

}
 

std::vector<COORD> VERTEX_ARRAY_GL ;

std::vector<COORD> LOAD_GL_VERTEX_ARRAY()
{
   
   std::cout << "loading vertices for vertex array \n" ;

   for(auto triangle : this->triangles)
       {
            for(auto coord :  triangle.vertex1) this->VERTEX_ARRAY_GL.push_back(coord); 
            for(auto coord :  triangle.vertex2) this->VERTEX_ARRAY_GL.push_back(coord); 
            for(auto coord :  triangle.vertex3) this->VERTEX_ARRAY_GL.push_back(coord);                          
       }    

     return this->VERTEX_ARRAY_GL;         

}

};

int main() {
    std::string filename = "/home/ubuntu-testing/Cube_b.stl"; // Replace with your STL file path

    class Geometry geometry;
    
    geometry.readSTLFile(filename);    

    // Print the extracted data
    for (auto triangle : geometry.triangles) {
        std::cout << "Normal  : " << triangle.normal[0]  << " " << triangle.normal[1]  << " " << triangle.normal[2]  << std::endl;
        std::cout << "Vertex 1: " << triangle.vertex1[0] << " " << triangle.vertex1[1] << " " << triangle.vertex1[2] << std::endl;
        std::cout << "Vertex 2: " << triangle.vertex2[0] << " " << triangle.vertex2[1] << " " << triangle.vertex2[2] << std::endl;
        std::cout << "Vertex 3: " << triangle.vertex3[0] << " " << triangle.vertex3[1] << " " << triangle.vertex3[2] << std::endl;
        std::cout << std::endl;
    }

  geometry.LOAD_GL_VERTEX_ARRAY();
  
   for(auto coord : geometry.VERTEX_ARRAY_GL)
       std::cout << "coord  : "  << coord << '\n';

    return 0;
}





