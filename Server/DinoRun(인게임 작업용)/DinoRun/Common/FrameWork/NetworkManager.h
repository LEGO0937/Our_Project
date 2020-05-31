#pragma once
#include "../stdafx.h"
class NetWorkManager
{
private:
	int m_iRoomNum;
	bool m_bGameMode;
	string m_sPlayerName;
public:
	NetWorkManager() {}
	~NetWorkManager() {}

	void SetRoomNum(const int& num) { m_iRoomNum = num; }
	void SetGameMode(const bool& mode) { m_bGameMode = mode; }
	void SetPlayerName(const string& name) { m_sPlayerName = name; }

	int GetRoomNum() { return m_iRoomNum; }
	bool GetGameMode() { return m_bGameMode; }
	string GetPlayerName() { return m_sPlayerName; }


};

