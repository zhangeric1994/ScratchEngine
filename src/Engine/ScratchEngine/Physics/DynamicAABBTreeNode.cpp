#include "DynamicAABBTreeNode.h"

bool ScratchEngine::Physics::DynamicAABBTreeNode::IsLeaf() const
{
	return left == null_index;
}

ScratchEngine::f32 ScratchEngine::Physics::DynamicAABBTreeNode::GetVolume() const
{
	return box.GetVolume();
}

ScratchEngine::f32 ScratchEngine::Physics::DynamicAABBTreeNode::GetUnionVolume(const DynamicAABBTreeNode& other) const
{
	return box.GetUnionVolume(other.box);
}
