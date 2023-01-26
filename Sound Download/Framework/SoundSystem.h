#pragma once

#include <vector>
#include <algorithm>

#include "Sound.h"


#include "../OpenAL 1.1 SDK/include/al.h"
#include "../OpenAL 1.1 SDK/include/alc.h"

using std::vector;

class SoundNode;

struct OALSource {
	ALuint	source;
	bool	inUse;

	OALSource(ALuint src) {
		source	= src;
		inUse	= false;
	}
};

#include "SoundNode.h"

enum SoundPriority;

class SoundSystem	{
public:
	static void Initialise(unsigned int channels = 8) {
		instance = new SoundSystem(channels);
	}

	static void Destroy() {
		delete instance;
	}

	static SoundSystem* GetSoundSystem() {
		return instance;
	}

	void		SetListener(SceneNode* l)	{listener = l;}
	SceneNode*	GetListener()				{return listener;}

	void		AddSoundNode(SoundNode* s)	{emitters.push_back(s);}

	void		Update(float msec);

	void		SetMasterVolume(float value);

	void		PlaySound(Sound* s, Vector3 position);
	void		PlaySound(Sound* s, SoundPriority p);

protected:
	SoundSystem(unsigned int channels = 128);
	~SoundSystem(void);

	void		UpdateListener();

	void		DetachSources(vector<SoundNode*>::iterator from, vector<SoundNode*>::iterator to);
	void		AttachSources(vector<SoundNode*>::iterator from, vector<SoundNode*>::iterator to);

	void		CullNodes();

	OALSource*	GetSource();

	void		UpdateTemporaryEmitters(float msec);

	vector<OALSource*>	sources;
	
	vector<SoundNode*>	emitters;
	vector<SoundNode*>	temporaryEmitters;

	SceneNode*			listener;

	ALCcontext*			context;
	ALCdevice*			device;

	float				masterVolume;

	static SoundSystem* instance;
};