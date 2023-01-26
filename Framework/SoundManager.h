#pragma once

#include <map>
#include <../Framework/Sound.h>

using std::map;

class SoundManager {
public:
	static void AddSound(string n);
	static Sound* GetSound(string name);

	static void DeleteSounds();

protected:
	SoundManager(void);
	~SoundManager(void);

	static map<string, Sound*> sounds;
};

