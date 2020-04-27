#pragma once
#include "../stdafx.h"

class SoundManager
{
private:
	System* system;		// Dx�� ġ�� Device�� �ش��Ѵ�.
	Sound** sound;		// Resource �ڿ�(����)
	Channel** channel;	// ��� ���� ä��

	UINT buffer;		// ����
	float volume;		// ����

	map<string, Sound**> sounds;			// ���� ���� ( string : ���� �̸�, Sound** : ���� )
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

	bool Playing(string name);	// �÷��� ������ �˻�
	bool Paused(string name);	// �Ͻ� �������� �˻�

	void Volume(string name, float volume);
	float Volume() { return volume; }

	void Update();
};