#pragma once
#include "../stdafx.h"

class SoundManager
{
private:
	System* system;		// Dx로 치면 Device에 해당한다.
	Sound** sound;		// Resource 자원(음원)
	Channel** channel;	// 재생 사운드 채널

	UINT buffer;		// 버퍼
	float volume;		// 볼륨

	map<string, Sound**> sounds;			// 음원 개수 ( string : 음원 이름, Sound** : 음원 )
	map<string, Sound**>::iterator iter;

public:
	SoundManager();
	~SoundManager();

	void AddSound(string name, string soundFile, bool loop = false);
	void Play(string name, float volume = 1.0f);
	void Stop(string name);
	void AllStop();
	void Pause(string name);
	void Resume(string name);

	bool Playing(string name);	// 플레이 중인지 검사
	bool Paused(string name);	// 일시 정지인지 검사

	void Volume(string name, float volume);
	float Volume() { return volume; }

	void Update();
};