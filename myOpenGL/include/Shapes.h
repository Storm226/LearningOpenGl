
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};


class Shapes {
	
public:

	static std::vector<float> getCube();

	static std::vector<Vertex> getSquarePyramid();

};