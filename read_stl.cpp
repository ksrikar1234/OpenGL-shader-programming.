#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Triangle {
    float normal[3];
    float vertex1[3];
    float vertex2[3];
    float vertex3[3];
};

std::vector<Triangle> readSTLFile(const std::string& filename) {
    std::vector<Triangle> triangles;

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return triangles;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "facet") {
            Triangle triangle;

            // Read the normal vector
            iss >> token; // Skip "normal"
            iss >> triangle.normal[0] >> triangle.normal[1] >> triangle.normal[2];

            // Read the vertices
            for (int i = 0; i < 3; ++i) {
                std::getline(file, line); // Read the line containing "vertex"
                std::istringstream vertexIss(line);
                vertexIss >> token; // Skip "vertex"
                vertexIss >> triangle.vertex1[i] >> triangle.vertex1[i] >> triangle.vertex1[i];
            }

            triangles.push_back(triangle);
        }
    }

    file.close();

    return triangles;
}

int main() {
    std::string filename = "/home/ubuntu-testing/cube.stl"; // Replace with your STL file path

    std::vector<Triangle> triangles = readSTLFile(filename);

    // Print the extracted data
    for (const Triangle& triangle : triangles) {
        std::cout << "Normal: " << triangle.normal[0] << " " << triangle.normal[1] << " " << triangle.normal[2] << std::endl;
        std::cout << "Vertex 1: " << triangle.vertex1[0] << " " << triangle.vertex1[1] << " " << triangle.vertex1[2] << std::endl;
        std::cout << "Vertex 2: " << triangle.vertex2[0] << " " << triangle.vertex2[1] << " " << triangle.vertex2[2] << std::endl;
        std::cout << "Vertex 3: " << triangle.vertex3[0] << " " << triangle.vertex3[1] << " " << triangle.vertex3[2] << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
