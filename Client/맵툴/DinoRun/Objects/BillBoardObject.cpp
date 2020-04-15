#include "BillBoardObject.h"
BillBoardObject::BillBoardObject(int nMeshes) :CGameObject(nMeshes)
{
	strcpy_s(m_pstrFrameName, "Billboard");
}
BillBoardObject::~BillBoardObject()
{

}