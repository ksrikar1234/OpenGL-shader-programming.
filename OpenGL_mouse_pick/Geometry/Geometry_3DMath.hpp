#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include<cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//-----------------------------------------------Camera Params------------------------------------------------------------------+

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view;
glm::mat4 model;
glm::mat4 projection;

//------------------------------------------------------------------------------------------------------------------------------+


inline void setup_transformations()

{
    float time = glfwGetTime();

    float set_axis_x =  10*((2.0*rotate_mouse_x/SCR_WIDTH)-1.0f) , set_axis_y = 10*(1.0f-(2.0*rotate_mouse_y/SCR_HEIGHT));

    auto X = (2.0f * std::abs(translate_mouse_x )) / SCR_WIDTH - 1.0f  , Y = 1.0f - (2.0f * std::abs(translate_mouse_y)) / SCR_HEIGHT;

    //-----------------------------Translate + Rotate  + Zoom functionality---------------------------------+
    //-------------------------X-axis-----------------Y-axis----------------Z-axis--------------------------+

    cameraPos = glm::vec3(-right_key + left_key - X, - up_key + down_key - Y, 20.0f - scroll_offset*6); //  use (-scroll_offset + 2.0f) for zooming but distorts mouse ray  so dont use it (Remember this)
    
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    view = glm::rotate(view, (-set_axis_y + w_key - s_key ), glm::vec3(1.0f, 0.0f, 0.0f));

    view = glm::rotate(view, (-set_axis_x + d_key - a_key ), glm::vec3(0.0f, 1.0f, 0.0f));

    view = glm::rotate(view, ( l_key - k_key ), glm::vec3(0.0f, 0.0f, 1.0f));

    //-----------------------------------functionality-----------------------------------------------+

    model = glm::mat4(1.0f);
        
    projection = glm::perspective(glm::radians(45.0f ), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f) ; // (Remember this)
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

inline glm::vec3 createMouseRay(float mouseX , float mouseY ,  const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix,
                    glm::vec3& rayOrigin, glm::vec3& rayDirection , uint SCR_WIDTH , uint SCR_HEIGHT)
{
    // Get the mouse cursor position in screen coordinates
    //float mouseX = mouse_x, mouseY = mouse_y;
    
    // Get the window dimensions

    // Convert screen coordinates to normalized device coordinates (NDC)
    float ndcX = (2.0f * mouseX) / SCR_WIDTH - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / SCR_HEIGHT;

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

