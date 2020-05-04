#pragma once
#include <Windows.h>

class Scene {
public:
	Scene() {}
	virtual ~Scene() {}
public:
	virtual void Enter() = 0;
	virtual void Destroy() = 0;
public:
	virtual DWORD WINAPI RecvThread(LPVOID) = 0;
	virtual void SendFunc(const SOCKET&) = 0;
};