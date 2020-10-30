#include "OBJLoader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

// Borrowed from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
#pragma region String Trimming

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

#pragma endregion 

bool OBJLoader::LoadOBJFile(const std::string& filename,
							std::vector<glm::vec3> &outPos,
							std::vector<glm::vec2> &outUV,
							std::vector<glm::vec3> &outNorm)
{
	//Vertex values
	std::vector<glm::fvec3> vertex_positions;
	std::vector<glm::fvec2> vertex_uvs;
	std::vector<glm::fvec3> vertex_normals;

	//Face vectors
	std::vector<GLint> vertex_position_indicies;
	std::vector<GLint> vertex_uv_indicies;
	std::vector<GLint> vertex_normal_indicies;

	//Vertex array
	std::vector<VertexPosNormTex> vertices;

	//Temp variables
	glm::vec3 temp_vec3;
	glm::vec2 temp_vec2;
	GLint temp_glint = 0;

	// Open our file in binary mode
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	if (!file) {
		throw std::runtime_error("Failed to open file");
	}

	//MeshBuilder<VertexPosNormTexCol> mesh;
	std::string line;

	// TODO: Load from file
	// Iterate as long as there is content to read
	while (std::getline(file, line)) 
	{
		trim(line);

		//Dealing with comments
		if (line.substr(0, 1) == "#")
		{
			//Ignore this line
		}
		//Vertices
		else if (line.substr(0, 2) == "v ")
		{
			std::istringstream ss = std::istringstream(line.substr(2));
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			vertex_positions.push_back(temp_vec3);
		}
		//Texture Coordinates
		else if (line.substr(0, 2) == "vt")
		{
			std::istringstream ss = std::istringstream(line.substr(3));
			ss >> temp_vec2.x >> temp_vec2.y;
			vertex_uvs.push_back(temp_vec2);
		}
		//Normals
		else if (line.substr(0, 2) == "vn")
		{
			std::istringstream ss = std::istringstream(line.substr(3));
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			vertex_normals.push_back(temp_vec3);
		}
		else if (line.substr(0, 1) == "f")
		{
			std::istringstream ss = std::istringstream(line.substr(2));
			int v, uv, n;
			char a;
			for (int i = 0; i <= 2; i++)
			{
				ss >> v >> a >> uv >> a >> n;
				vertex_position_indicies.push_back(v);
				vertex_uv_indicies.push_back(uv);
				vertex_normal_indicies.push_back(n);
			}
			/*int counter = 0;
			while (ss >> temp_glint)
			{
				//Pushing indicies into correct array positions
				if (counter == 0)
					vertex_position_indicies.push_back(temp_glint);
				else if(counter == 1)
					vertex_uv_indicies.push_back(temp_glint);
				else if(counter == 2)
					vertex_normal_indicies.push_back(temp_glint);

				//Skipping spaces and forward slashes
				if (ss.peek() == '/')
				{
					counter++;
					ss.ignore(1, '/');
				}
				else if(ss.peek() == ' ')
				{
					counter++;
					ss.ignore(1, ' ');
				}

				//Reset the counter
				if (counter > 2)
					counter = 0;
			}*/

		}
		else
		{
			//If anything else, ignore
		}
		//Building mesh
		

		
	}
	vertices.resize(vertex_position_indicies.size(), VertexPosNormTex());
	for (int i = 0; i < vertex_position_indicies.size(); i++)
	{
		unsigned int vertexNum = vertex_position_indicies[i];
		unsigned int uvNum = vertex_uv_indicies[i];
		unsigned int normalNum = vertex_normal_indicies[i];
		vertices[i].Position = vertex_positions[vertexNum - 1];
		vertices[i].UV = vertex_uvs[uvNum - 1];
		vertices[i].Normal = vertex_normals[normalNum - 1];
		outPos.push_back(vertices[i].Position);
		outUV.push_back(vertices[i].UV);
		outNorm.push_back(vertices[i].Normal);
	}
	// Note: with actual OBJ files you're going to run into the issue where faces are composited of different indices
	// You'll need to keep track of these and create vertex entries for each vertex in the face
	// If you want to get fancy, you can track which vertices you've already added

	return true;
}