#include "SoundManager.h"

#ifndef SAFE_DELETE 
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } } 
#endif // SAFE_DELETE 



SoundManager::SoundManager()
	:system(nullptr), channel(nullptr), sound(nullptr), buffer(15), volume(0.5f)
{
	// 시스템(Device) 생성
	System_Create(&system);

	// 사운드 리소스 및 채널 생성
	system->init(buffer, FMOD_INIT_NORMAL, 0);
	sound = new Sound *[buffer];			// 15개의 음원을 가리키는 포인터 배열 동적할당
	channel = new Channel *[buffer];		// 15개의 채널을 가리키는 포인터 배열 동적할당

	memset(sound, 0, sizeof(Sound*) * (buffer));
	memset(channel, 0, sizeof(Channel*) * (buffer));
}

SoundManager::~SoundManager()
{
	// 소리는 중지를 시킨다음 지워야한다?
	if (channel != nullptr || sound != nullptr)
	{
		for (UINT i = 0; i < buffer; i++)
		{
			if (channel != nullptr && channel[i])
				channel[i]->stop();

			// 리소스 해제
			if (sound != nullptr && sound[i])
				sound[i]->release();
		}
	}

	SAFE_DELETE(channel);
	SAFE_DELETE(sound);

	// 시스템 닫기
	if (system != nullptr)
	{
		system->release();
		system->close();
	}

	// Map 비우기
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
		// 음원 추가
		system->createStream
		(
			soundFile.c_str(),
			FMOD_LOOP_NORMAL,		// 루프 모드
			nullptr,				// 추가 정보?
			&sound[sounds.size()]	// 가지고 있는 음원들
		);
	}
	else
	{
		// 음원 추가
		system->createStream
		(
			soundFile.c_str(),
			FMOD_DEFAULT,			// 한번 재생 (loop x)
			nullptr,
			&sound[sounds.size()]
		);
	}

	// 맵에 넣기 ( sounds[name] = &sound[sounds.size()];
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

		// name과 일치하는 음원이 있다면
		if (name == iter->first)
		{
			// 해당 사운드 플레이
			system->playSound
			(
				*iter->second,
				nullptr,	// 비어있는 채널 사용
				false,				// 안멈춤
				&channel[count]
			);

			// 볼륨
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
		// name과 일치하는 음원이 있다면
		if (name == iter->first)
		{
			// 해당 채널(음원) stop
			channel[count]->stop();
			break;
		}
	}
}

void SoundManager::AllStop()
{
	int count = 0;
	iter = sounds.begin();

	//모든 음악을 전부 종료한다.
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

		// name과 일치하는 음원이 있다면
		if (name == iter->first)
		{
			// 멈춤
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

		// name과 일치하는 음원이 있다면
		if (name == iter->first)
		{
			// 다시 재생
			channel[count]->setPaused(false);
			break;
		}
	}
}
bool SoundManager::Playing(string name)
{
	// 모든 채널 검사해서 하나라도 플레이 중이면 true 반환
	bool bPlay = false;
	int count = 0;
	iter = sounds.begin();

	for (iter; iter != sounds.end(); ++iter)
	{
		count++;

		// name과 일치하는 음원이 있다면
		if (name == iter->first)
		{
			// 재생중이라면 bPlay를 true로 설정
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

		// name과 일치하는 음원이 있다면
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
