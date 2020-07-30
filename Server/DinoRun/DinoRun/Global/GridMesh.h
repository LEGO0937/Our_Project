#pragma once
#include "Mesh.h"

class GridMesh : public CMesh
{
public:
	GridMesh() {}
	GridMesh(float width, float height, int numX, int numY);
	~GridMesh() {}
};

