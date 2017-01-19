#include "Serializable.h"

namespace o2
{
	DataNode ISerializable::SerializeBasic(const void* thisObject) const
	{
		DataNode res;
		OnSerialize(res);
		SerializeBasicType(res, thisObject, GetType());

		return res;
	}

	void ISerializable::DeserializeBasic(const DataNode& node, const void* thisObject)
	{
		DeserializeBasicType(node, thisObject, GetType());
		OnDeserialized(node);
	}

	void ISerializable::SerializeBasicType(DataNode& node, const void* thisObject, const Type& type) const
	{
		char* thisPtr = (char*)thisObject;
		for (auto field : type.GetFields())
		{
			auto srlzAttribute = field->HasAttribute<SerializableAttribute>();
			if (srlzAttribute)
				field->SerializeObject(field->GetValuePtrStrong(thisPtr), *node.AddNode(field->GetName()));
		}

		for (auto basic : type.GetBaseTypes())
			SerializeBasicType(node, thisObject, *basic);
	}

	void ISerializable::DeserializeBasicType(const DataNode& node, const void* thisObject, const Type& type)
	{
		char* thisPtr = (char*)thisObject;
		for (auto field : type.GetFields())
		{
			auto srlzAttribute = field->HasAttribute<SerializableAttribute>();
			if (srlzAttribute)
			{
				auto fldNode = node.GetNode(field->GetName());
				if (fldNode)
					field->DeserializeObject(field->GetValuePtrStrong(thisPtr), *fldNode);
			}
		}

		for (auto basic : type.GetBaseTypes())
			DeserializeBasicType(node, thisObject, *basic);
	}

}

CLASS_META(o2::ISerializable)
{
	BASE_CLASS(o2::IObject);


	PUBLIC_FUNCTION(DataNode, Serialize);
	PUBLIC_FUNCTION(void, Deserialize, const DataNode&);
	PROTECTED_FUNCTION(void, OnSerialize, DataNode&);
	PROTECTED_FUNCTION(void, OnDeserialized, const DataNode&);
	PROTECTED_FUNCTION(DataNode, SerializeBasic, const void*);
	PROTECTED_FUNCTION(void, DeserializeBasic, const DataNode&, const void*);
	PROTECTED_FUNCTION(void, SerializeBasicType, DataNode&, const void*, const Type&);
	PROTECTED_FUNCTION(void, DeserializeBasicType, const DataNode&, const void*, const Type&);
}
END_META;
 
