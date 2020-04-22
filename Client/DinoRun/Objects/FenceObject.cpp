#include "FenceObject.h"

FenceObject::FenceObject(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 200;
	isKinematic = true;
	m_ModelType = ModelType::Fence;
}
FenceObject::~FenceObject()
{

}