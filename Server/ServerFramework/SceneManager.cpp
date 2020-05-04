#include "SceneManager.h"

SceneManager::SceneManager()
{
	preIndex = 0;
	for (int i = 0; i < MAX_SCENE; ++i)
		pSceneList[i] = NULL;
}

SceneManager::~SceneManager()
{
	for (int i = 0; i < MAX_SCENE; ++i)
		SAFE_DELETE(pSceneList[i]);
}

void SceneManager::AddScene(int index, Scene* scene)
{
	pSceneList[index] = scene;
}

void SceneManager::MoveScene(int index)
{
	pSceneList[preIndex]->Destroy();
	preIndex = index;
	pSceneList[preIndex]->Enter();
}