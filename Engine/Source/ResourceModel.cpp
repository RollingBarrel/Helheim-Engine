#include "ResourceModel.h"

#include "ModuleFileSystem.h"

ResourceModel::ResourceModel(unsigned int uid, ModelNode root) : Resource(uid, Type::Model), mRoot(root)
{
}