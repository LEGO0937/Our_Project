#pragma once
#include "Mesh.h"
class PlaneMesh : public CMesh
{
public:
	PlaneMesh();
	PlaneMesh(float fx,float fy,float fz,float minX,float maxX, float minY, float maxY);
	~PlaneMesh();
};

