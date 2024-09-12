#include "include/Shapes.h"



/// <summary>
/// Returns a pointer to the vertices which describe a unit cube centered 
/// about the origin, the vertices are layed out as a 3x1 positional vector,
/// a 3x1 normal vector, and a 2x1 texture coordinate vector. 
/// </summary>
/// 
std::vector<float> Shapes::getCube() {
	// we want to return a set of vertices describing a unit cube as well as the tecture coordinates and the surface normals

	std::vector<float> vertices{ 
        // positions          // normals           // texture coords
          -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
           0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
           0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
           0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
          -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
          -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, };

    return vertices;
}