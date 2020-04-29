#pragma once
#include "Mesh.h"
class LiePlaneMesh : public CMesh
{
public:
	LiePlaneMesh();
	LiePlaneMesh(float fx, float fy, float fz, float minX, float maxX, float minY, float maxY);
	~LiePlaneMesh();
};

