#include "AnimationClip.h"


ScratchEngine::Animation::AnimationClip::AnimationClip(aiAnimation * anim)
{
	lastTime = 0.0f;
	name = anim->mName.C_Str();
	ticksPerSecond = anim->mTicksPerSecond > 0.0f ? (float)anim->mTicksPerSecond : 920.0f;
	duration = (float)anim->mDuration;
	for (UINT i = 0; i < anim->mNumChannels; i++) {
		auto channel = anim->mChannels[i];
		AnimationChannel * c = new AnimationChannel(
			channel->mNodeName.C_Str(),
			channel->mPositionKeys,
			channel->mNumPositionKeys,
			channel->mRotationKeys,
			channel->mNumRotationKeys,
			channel->mScalingKeys,
			channel->mNumScalingKeys
		);
		channels.push_back(c);
		XMFLOAT3 zero = { 0,0,0 };
		lastPositions.push_back(zero);
	}
	transforms.clear();
	playAnimationForward = true;
}

std::vector<XMMATRIX> ScratchEngine::Animation::AnimationClip::GetTransforms(float dt)
{
	return transforms[GetFrameIndexAt(dt)];
}

int ScratchEngine::Animation::AnimationClip::GetFrameIndexAt(float dt)
{
	//printf("animation frame time : %f  * %f\n", dt , ticksPerSecond);
	dt *= ticksPerSecond;
	float time = 0.0f;
	if (duration > 0.0f) {
		time = fmod(dt,duration);
	}
	float percent = time / duration;
	if (!playAnimationForward) {
		percent = (percent - 1.0f) * -1.0f;
	}
	int frameIndexAt = (int)(transforms.size() * percent);
	
	return frameIndexAt;
}

void ScratchEngine::Animation::AnimationClip::Evaluate(float dt, std::map<std::string, Bone*> bonesByName)
{
	dt *= ticksPerSecond;
	float time = 0.0f;
	if (duration > 0.0f) {
		time = fmod(dt,duration);
	}
	for (int i = 0; i < channels.size(); i++) {
		AnimationChannel * channel = channels[i];

		std::map<std::string, Bone*>::iterator it = bonesByName.find(channel->name);
		if (it == bonesByName.end()) {
			printf("Did not find the bone node %s", channel->name);
			continue;
		}
		// interpolate position keyframes
		XMFLOAT3 pPosition;
		if (channel->positionKeys.size() > 0) {
			int frame = (time >= lastTime) ? (int)lastPositions[i].x : 0;
			while (frame < channel->positionKeys.size() - 1) {
				if (time < channel->positionKeys[frame + 1].mTime) {
					break;
				}
				frame++;
			}
			if (frame >= channel->positionKeys.size()) {
				frame = 0;
			}

			int nextFrame = (frame + 1) % channel->positionKeys.size();

			aiVectorKey key = channel->positionKeys[frame];
			aiVectorKey nextKey = channel->positionKeys[nextFrame];
			float diffTime = nextKey.mTime - key.mTime;
			if (diffTime < 0.0) {
				diffTime += duration;
			}
			if (diffTime > 0.0) {
				float factor = (float)((time - key.mTime) / diffTime);
				pPosition = TOXMFLOAT3(key.mValue + (nextKey.mValue - key.mValue) * factor);
			}
			else {
				pPosition = TOXMFLOAT3(key.mValue);
			}
			lastPositions[i].x = frame;

		}

		// interpolate rotation keyframes
		XMFLOAT4 pRot = { 0, 0, 0, 1 };
		if (channel->rotationKeys.size() > 0) {
			int frame = (time >= lastTime) ? (int)lastPositions[i].y : 0;
			while (frame < channel->rotationKeys.size() - 1) {
				if (time < channel->rotationKeys[frame + 1].mTime) {
					break;
				}
				frame++;
			}
			if (frame >= channel->rotationKeys.size()) {
				frame = 0;
			}
			int nextFrame = (frame + 1) % channel->rotationKeys.size();

			aiQuatKey key = channel->rotationKeys[frame];
			aiQuatKey nextKey = channel->rotationKeys[nextFrame];
			key.mValue.Normalize();
			nextKey.mValue.Normalize();
			float diffTime = nextKey.mTime - key.mTime;
			if (diffTime < 0.0) {
				diffTime += duration;
			}
			if (diffTime > 0) {
				float factor = (float)((time - key.mTime) / diffTime);
				XMVECTOR rot1 = XMLoadFloat4(&TOXMFLOAT4(key.mValue));
				XMVECTOR rot2= XMLoadFloat4(&TOXMFLOAT4(nextKey.mValue));
				XMStoreFloat4(&pRot,XMQuaternionSlerp(rot1, rot2, factor));
			}
			else {
				pRot = TOXMFLOAT4(key.mValue);
			}
			lastPositions[i].y = frame;
			//TODO change y to x if not work

		}
		// interpolate scale keyframes
		XMFLOAT3 pscale = { 1, 1, 1};
		if (channel->scalingKeys.size() > 0) {
			int frame = (time >= lastTime) ? (int)lastPositions[i].z : 0;
			while (frame < channel->scalingKeys.size() - 1) {
				if (time < channel->scalingKeys[frame + 1].mTime) {
					break;
				}
				frame++;
			}
			if (frame >= channel->scalingKeys.size()) {
				frame = 0;
			}
			lastPositions[i].z = frame;
		}


		XMMATRIX mat = XMMatrixScalingFromVector(XMLoadFloat3(&pscale))
			* XMMatrixRotationQuaternion(XMLoadFloat4(&pRot))
			* XMMatrixTranslationFromVector(XMLoadFloat3(&pPosition));
		
		XMMATRIX temp = bonesByName[channel->name]->localTransform;
		bonesByName[channel->name]->localTransform = mat;
	}
	lastTime = time;
}

XMFLOAT3 ScratchEngine::Animation::AnimationClip::TOXMFLOAT3(aiVector3D val)
{
	return XMFLOAT3{val.x,val.y,val.z};
}


XMFLOAT4 ScratchEngine::Animation::AnimationClip::TOXMFLOAT4(aiQuaternion val)
{
	return XMFLOAT4{ val.x,val.y,val.z,val.w };
}
