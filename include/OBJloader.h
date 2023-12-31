#include <glm/glm.hpp>
#include <cstring>
#include <vector>
#include <string>

using namespace glm;

bool loadOBJ(
	const char * path,
	vector<vec3> & out_vertices,
	vector<vec3> & out_normals,
	vector<vec2> & out_uvs) {

	vector<int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices;
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;

	FILE * file;
	file = fopen(path, "r");
	if (!file) {
		printf("Impossible to open the file ! Are you in the right path ?\n");
		printf(path);
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			int vertexIndex[3], uvIndex[3], normalIndex[3];
			bool uv = true;
			bool norm = true;
			char line[128];
			fgets(line, 128, file);
			//vertex, uv, norm
			int matches = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				//vertex, norm
                matches = sscanf(line, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches != 6) {
					//vertex, uv
                    matches = sscanf(line, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
					if (matches != 6) {
						//vertex
                        matches = sscanf(line, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
						if (matches != 6) {
							printf("File can't be read by our simple parser. 'f' format expected: d/d/d d/d/d d/d/d || d/d d/d d/d || d//d d//d d//d\n");
							printf("Character at %ld", ftell(file));
							return false;
						}
						uv, norm = false;
					}
					else {
						norm = false;
					}
				}
				else {
					uv = false;
				}
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			if (norm) {
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			if (uv) {
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
			}
		}
		else {
			char clear[1000];
			fgets(clear, 1000, file);
		}

	}
	//cout << "Vertex indices: " << vertexIndices.size() << endl;
	//cout << "UV indices: " << uvIndices.size() << endl;
	//cout << "Normal indices: " << normalIndices.size() << endl;
	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		if (uvIndices.size() != 0) {
			if (i < uvIndices.size()) {
				unsigned int uvIndex = abs(uvIndices[i]);
				vec2 uv = temp_uvs[uvIndex - 1];
				out_uvs.push_back(uv);
			}
		}
		if (normalIndices.size() != 0) {
			if (i < normalIndices.size()) {
				unsigned int normalIndex = abs(normalIndices[i]);
				vec3 normal = temp_normals[normalIndex - 1];
				out_normals.push_back(normal);
			}
		}

		unsigned int vertexIndex = abs(vertexIndices[i]);
		vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

	return true;
}

bool loadOBJ2(const char *path, vector<int> &vertexIndices, vector<vec3> &temp_vertices, vector<vec3> &out_normals,
              vector<vec2> &out_uvs) {

    vector<int> uvIndices, normalIndices;
    vector<vec2> temp_uvs;
    vector<vec3> temp_normals;

    FILE *file;
    file = fopen(path, "r");
    if (!file) {
        printf("Impossible to open the file ! Are you in the right path ?\n");
        getchar();
        return false;
    }

    while (true) {

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0) {
            vec3 vertex;
            res = fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0) {
            vec2 uv;
            res = fscanf(file, "%f %f\n", &uv.x, &uv.y);
            if (res != 2) {
                printf("Missing uv information!\n");
            }
            // Invert V coordinate since we will only use DDS texture, which are inverted.
            // Remove if you want to use TGA or BMP loaders.
            uv.y = -uv.y;
            temp_uvs.push_back(uv);
        } else if (strcmp(lineHeader, "vn") == 0) {
            vec3 normal;
            res = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            if (res != 3) {
                printf("Missing normal information!\n");
            }
            temp_normals.push_back(normal);
        } else if (strcmp(lineHeader, "f") == 0) {
            char *getRes;
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            bool uv = true;
            bool norm = true;
            char line[128];
            getRes = fgets(line, 128, file);
            if (getRes == 0) {
                printf("incomplete face\n");
            }

            //vertex, uv, norm
            int matches = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0],
                                 &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2],
                                 &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                //vertex, norm
                matches = sscanf(line, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 6) {
                    //vertex, uv
                    matches = sscanf(line, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0],
                                     &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
                    if (matches != 6) {
                        //vertex
                        matches = sscanf(line, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1],
                                         &vertexIndex[2]);
                        if (matches != 3) {
                            printf("File can't be read by our simple parser. 'f' format expected: "
                                   "d/d/d d/d/d d/d/d || d/d d/d d/d || d//d d//d d//d\n");
                            printf("Character at %ld", ftell(file));
                            return false;
                        }
                        uv, norm = false;
                    } else {
                        norm = false;
                    }
                } else {
                    uv = false;
                }
            }
            vertexIndices.push_back(abs(vertexIndex[0]) - 1);
            vertexIndices.push_back(abs(vertexIndex[1]) - 1);
            vertexIndices.push_back(abs(vertexIndex[2]) - 1);
            if (norm) {
                normalIndices.push_back(abs(normalIndex[0]) - 1);
                normalIndices.push_back(abs(normalIndex[1]) - 1);
                normalIndices.push_back(abs(normalIndex[2]) - 1);
            }
            if (uv) {
                uvIndices.push_back(abs(uvIndex[0]) - 1);
                uvIndices.push_back(abs(uvIndex[1]) - 1);
                uvIndices.push_back(abs(uvIndex[2]) - 1);
            }
        } else {
            char clear[1000];
            char *getsRes = fgets(clear, 1000, file);
        }
    }
    if (!normalIndices.empty())
        out_normals.resize(temp_normals.size());
    if (!uvIndices.empty())
        out_uvs.resize(temp_normals.size());
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        int vi = vertexIndices[i];
        if (!normalIndices.empty()) {
            int ni = normalIndices[i];
            out_normals[vi] = temp_normals[ni];
        }
        if (!uvIndices.empty() && i < uvIndices.size()) {
            int ui = uvIndices[i];
            out_uvs[vi] = temp_uvs[ui];
        }
    }

    return true;
}
