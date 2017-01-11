#pragma once

#include "pugixml/pugixml.hpp"
#include "Utils/Reflection/Type.h"
#include "Utils/Data/DataNode.h"
#include "Utils/String.h"

namespace o2
{
	namespace XmlDataFormat
	{
		bool LoadDataDoc(const WString& data, DataNode& node);
		void LoadDataNode(const pugi::xml_node& xmlNode, DataNode& dataNode);

		String SaveDataDoc(const DataNode& node);
		void SaveDataNode(pugi::xml_node& xmlNode, const DataNode& dataNode);
	}
}
