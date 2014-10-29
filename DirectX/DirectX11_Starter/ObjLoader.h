#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include "Mesh.h"

using namespace DirectX;
using namespace std;

struct ObjObject {
	Vertex* vertices;
	UINT* indices;
	int vertexLength;
	int indexLength;
};

class ObjLoader
{
public:
	ObjLoader();
	~ObjLoader();

	ObjObject* load(char* fileName);
};

