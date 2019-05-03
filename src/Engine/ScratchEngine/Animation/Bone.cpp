#include "Bone.h"

ScratchEngine::Animation::Bone::Bone()
{

}

ScratchEngine::Animation::Bone::Bone(std::string Name, Bone * _parent)
{
	name = Name;
	parent = _parent;
}
