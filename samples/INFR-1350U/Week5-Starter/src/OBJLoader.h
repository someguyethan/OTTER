#include "MeshBuilder.h"
#include "MeshFactory.h"

class OBJLoader
{
public:
	static bool LoadOBJFile(const std::string& filename,
							std::vector<glm::vec3> &outPos, 
							std::vector<glm::vec2> &outUV, 
							std::vector<glm::vec3> &outNorm);

protected:
	OBJLoader() = default;
	~OBJLoader() = default;
};
