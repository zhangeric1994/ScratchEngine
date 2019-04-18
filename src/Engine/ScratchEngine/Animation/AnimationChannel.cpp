#include "AnimationChannel.h"

ScratchEngine::Animation::AnimationChannel::AnimationChannel(std::string _name, aiVectorKey * _PositionKeys, UINT _PosSize, aiQuatKey * _RotationKeys, UINT _RotSize, aiVectorKey * _ScalingKeys, UINT _ScaSize)
{
	name = _name;
	for (UINT i = 0; i < _PosSize; i++) {
		positionKeys.push_back(_PositionKeys[i]);
	}

	for (UINT i = 0; i < _RotSize; i++) {
		rotationKeys.push_back(_RotationKeys[i]);
	}

	for (UINT i = 0; i < _ScaSize; i++) {
		scalingKeys.push_back(_ScalingKeys[i]);
	}
}
