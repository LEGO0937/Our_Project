#pragma once
#include "Mesh.h"
class PlaneMesh : public CMesh
{
public:
	PlaneMesh();
	PlaneMesh(float fx,float fy,float fz,float minX,float maxX, float minY, float maxY);
	~PlaneMesh();
};

class UnderPlaneMesh : public CMesh
{
public:
	UnderPlaneMesh();
	UnderPlaneMesh(float fx, float fy, float fz, float minX, float maxX, float minY, float maxY);
	~UnderPlaneMesh();
};