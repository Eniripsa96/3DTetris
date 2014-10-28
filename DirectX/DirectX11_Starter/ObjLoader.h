#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include "Material.h"

#define MAX_LINE_SIZE 64
#define MAX_WORDS 32

using namespace DirectX;
using namespace std;

struct ObjObject {
	XMFLOAT3* positions;
	XMFLOAT2* uvs;
	UINT* posIndices;
	UINT* uvIndices;
	int positionsLength;
	int uvsLength;
	int posIndicesLength;
	int uvIndicesLength;
};

class ObjLoader
{
public:
	ObjLoader();
	~ObjLoader();

	ObjObject* load(char* fileName);
};

