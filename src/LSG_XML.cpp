#include "LSG_XML.h"


LibXml::xmlNode* LSG_XML::AddChildNode(LibXml::xmlNode* xmlNode, LibXml::xmlDoc* xmlDoc, const std::string& xmlName, const std::string& value)
{
	if (!xmlNode || xmlName.empty() || value.empty())
		return nullptr;

	LibXml::xmlNode* xmlChildNode = nullptr;

	auto xmlChildName = reinterpret_cast<const LibXml::xmlChar*>(xmlName.c_str());

	if (!value.empty()) {
		auto xmlContent = reinterpret_cast<const LibXml::xmlChar*>(value.c_str());
		xmlChildNode    = LibXml::xmlNewDocNode(xmlDoc, nullptr, xmlChildName, xmlContent);
	} else {
		xmlChildNode = LibXml::xmlNewNode(nullptr, xmlChildName);
	}

	if (xmlChildNode)
		LibXml::xmlAddChild(xmlNode, xmlChildNode);

	return xmlChildNode;
}


std::string LSG_XML::GetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute)
{
	if (!xmlNode || attribute.empty())
		return "";

	auto xmlAttribute = xmlNode->properties;

	while (xmlAttribute)
	{
		if (LibXml::xmlStrcmp(xmlAttribute->name, reinterpret_cast<const LibXml::xmlChar*>(attribute.c_str())) == 0)
			return reinterpret_cast<const char*>(xmlAttribute->xmlChildrenNode->content);

		xmlAttribute = xmlAttribute->next;
	}

	return "";
}

LSG_UMapStrStr LSG_XML::GetAttributes(LibXml::xmlNode* xmlNode)
{
	if (!xmlNode)
		return {};

	LSG_UMapStrStr attributes;

	auto xmlAttribute = xmlNode->properties;

	while (xmlAttribute)
	{
		auto key   = reinterpret_cast<const char*>(xmlAttribute->name);
		auto value = reinterpret_cast<const char*>(xmlAttribute->xmlChildrenNode->content);

		attributes[key] = value;
		xmlAttribute    = xmlAttribute->next;
	}

	return attributes;
}

LSG_XmlNodes LSG_XML::GetChildNodes(LibXml::xmlNode* xmlNode)
{
	if (!xmlNode)
		return {};

	LSG_XmlNodes nodes;

	auto xmlChildNode = xmlNode->xmlChildrenNode;

	while (xmlChildNode) {
		nodes.push_back(xmlChildNode);
		xmlChildNode = xmlChildNode->next;
	}

	return nodes;
}

LibXml::xmlNode* LSG_XML::GetNode(const std::string& xpath, LibXml::xmlDoc* xmlDoc)
{
	if (xpath.empty() || !xmlDoc)
		return nullptr;

	auto pathContext = LibXml::xmlXPathNewContext(xmlDoc);

	if (!pathContext)
		return nullptr;

	auto pathObject = LibXml::xmlXPathEvalExpression(reinterpret_cast<const LibXml::xmlChar*>(xpath.c_str()), pathContext);

	LibXml::xmlXPathFreeContext(pathContext);

	if (!pathObject)
		return nullptr;

	LibXml::xmlNode* result = nullptr;

	if (pathObject->nodesetval->nodeNr > 0)
		result = pathObject->nodesetval->nodeTab[0];

	LibXml::xmlXPathFreeObject(pathObject);

	return result;
}

std::string LSG_XML::GetValue(LibXml::xmlNode* node, LibXml::xmlDoc* xmlDoc)
{
	if (!node || !xmlDoc)
		return "";

	auto xmlValue = LibXml::xmlNodeListGetString(xmlDoc, node->children, 1);
	auto value    = (xmlValue ? std::string(reinterpret_cast<char*>(xmlValue)) : "");

	LibXml::xmlFree(xmlValue);

	return value;
}

LibXml::xmlDoc* LSG_XML::Open(const std::string& xmlFile)
{
	return LibXml::xmlParseFile(xmlFile.c_str());
}

void LSG_XML::SetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute, const std::string& value)
{
	if (!xmlNode || attribute.empty())
		return;

	auto attr = reinterpret_cast<const LibXml::xmlChar*>(attribute.c_str());
	auto val  = reinterpret_cast<const LibXml::xmlChar*>(value.c_str());

	LibXml::xmlSetProp(xmlNode, attr, val);
}
