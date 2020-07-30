#include "SoundManager.h"

#ifndef SAFE_DELETE 
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } } 
#endif // SAFE_DELETE 



SoundManager::SoundManager()
	:system(nullptr), channel(nullptr), sound(nullptr), buffer(15), volume(0.5f)
{
	// �ý���(Device) ����
	System_Create(&system);

	// ���� ���ҽ� �� ä�� ����
	system->init(buffer, FMOD_INIT_NORMAL, 0);
	sound = new Sound *[buffer];			// 15���� ������ ����Ű�� ������ �迭 �����Ҵ�
	channel = new Channel *[buffer];		// 15���� ä���� ����Ű�� ������ �迭 �����Ҵ�

	memset(sound, 0, sizeof(Sound*) * (buffer));
	memset(channel, 0, sizeof(Channel*) * (buffer));
}

SoundManager::~SoundManager()
{
	// �Ҹ��� ������ ��Ų���� �������Ѵ�?
	if (channel != nullptr || sound != nullptr)
	{
		for (UINT i = 0; i < buffer; i++)
		{
			if (channel != nullptr && channel[i])
				channel[i]->stop();

			// ���ҽ� ����
			if (sound != nullptr && sound[i])
				sound[i]->release();
		}
	}

	SAFE_DELETE(channel);
	SAFE_DELETE(sound);

	// �ý��� �ݱ�
	if (system != nullptr)
	{
		system->release();
		system->close();
	}

	// Map ����
	sounds.clear();
}
void SoundManager::Initialize()
{
	AddSound("Start_BGM", "Resources/Sounds/Start_BGM.mp3", true);
	AddSound("InGame_BGM", "Resources/Sounds/InGame_BGM.mp3", true);
	AddSound("Mouse_Up", "Resources/Sounds/Mouse_Up.mp3", false);
	AddSound("Mouse_Down", "Resources/Sounds/Mouse_Down.mp3", false);
	AddSound("Gole", "Resources/Sounds/Gole.mp3", false);
	AddSound("Heat", "Resources/Sounds/Heat.mp3", false);
	AddSound("MeatEat", "Resources/Sounds/MeatEat.mp3", false);
	AddSound("ItemBox", "Resources/Sounds/ItemBox.mp3", false);
	AddSound("Curve", "Resources/Sounds/Curve.mp3", false);
	AddSound("Running", "Resources/Sounds/Running.mp3", true);
	AddSound("CountDown", "Resources/Sounds/123.mp3", false);
	AddSound("CountDownZero", "Resources/Sounds/Go.mp3", false);
	AddSound("MeteoriteMove", "Resources/Sounds/MeteoriteMove.mp3", true);
	AddSound("MeteoriteBoom", "Resources/Sounds/Boom.mp3", false);
}
void SoundManager::AddSound(string name, string soundFile, bool loop)
{
	if (loop == true)
	{
		// ���� �߰�
		system->createStream
		(
			soundFile.c_str(),
			FMOD_LOOP_NORMAL,		// ���� ���
			nullptr,				// �߰� ����?
			&sound[sounds.size()]	// ������ �ִ� ������
		);
	}
	else
	{
		// ���� �߰�
		system->createStream
		(
			soundFile.c_str(),
			FMOD_DEFAULT,			// �ѹ� ��� (loop x)
			nullptr,
			&sound[sounds.size()]
		);
	}

	// �ʿ� �ֱ� ( sounds[name] = &sound[sounds.size()];
	sounds.insert(make_pair(name, &sound[sounds.size()]));

}

void SoundManager::Update()
{
	system->update();
}
void SoundManager::Play(string name, float volume)
{
	int count = 0;

	iter = sounds.begin();
	for (iter; iter != sounds.end(); ++iter)
	{
		count++;

		// name�� ��ġ�ϴ� ������ �ִٸ�
		if (name == iter->first)
		{
			// �ش� ���� �÷���
			system->playSound
			(
				*iter->second,
				nullptr,	// ����ִ� ä�� ���
				false,				// �ȸ���
				&channel[count]
			);

			// ����
			channel[count]->setVolume(volume);
		}
	}
}


void SoundManager::Stop(string name)
{
	int count = 0;
	iter = sounds.begin();

	for (iter; iter != sounds.end(); ++iter)
	{

		count++;
		// name�� ��ġ�ϴ� ������ �ִٸ�
		if (name == iter->first)
		{
			// �ش� ä��(����) stop
			channel[count]->stop();
			break;
		}
	}
}

void SoundManager::AllStop()
{
	int count = 0;
	iter = sounds.begin();

	//��� ������ ���� �����Ѵ�.
	for (iter; iter != sounds.end(); ++iter)
	{
		count++;
		channel[count]->stop();

	}
}


void SoundManager::Pause(string name)
{
	int count = 0;
	iter = sounds.begin();

	for (iter; iter != sounds.end(); ++iter)
	{
		count++;

		// name�� ��ġ�ϴ� ������ �ִٸ�
		if (name == iter->first)
		{
			// ����
			channel[count]->setPaused(true);
			break;
		}
	}
}

void SoundManager::Resume(string name)
{
	int count = 0;
	iter = sounds.begin();

	for (iter; iter != sounds.end(); ++iter)
	{
		count++;

		// name�� ��ġ�ϴ� ������ �ִٸ�
		if (name == iter->first)
		{
			// �ٽ� ���
			channel[count]->setPaused(false);
			break;
		}
	}
}
bool SoundManager::Playing(string name)
{
	// ��� ä�� �˻��ؼ� �ϳ��� �÷��� ���̸� true ��ȯ
	bool bPlay = false;
	int count = 0;
	iter = sounds.begin();

	for (iter; iter != sounds.end(); ++iter)
	{
		count++;

		// name�� ��ġ�ϴ� ������ �ִٸ�
		if (name == iter->first)
		{
			// ������̶�� bPlay�� true�� ����
			channel[count]->isPlaying(&bPlay);
			break;
		}
	}

	return bPlay;
}

bool SoundManager::Paused(string name)
{
	bool bPause = false;
	int count = 0;
	iter = sounds.begin();

	for (iter; iter != sounds.end(); ++iter)
	{
		count++;

		// name�� ��ġ�ϴ� ������ �ִٸ�
		if (name == iter->first)
		{
			channel[count]->getPaused(&bPause);
			break;
		}
	}

	return bPause;
}
void SoundManager::Volume(string name, float volume)
{
	int count = 0;
	iter = sounds.begin();

	for (iter; iter != sounds.end(); ++iter)
	{
		count++;

		if (name == iter->first)
		{
			channel[count]->setVolume(volume);
			break;
		}
	}
}
