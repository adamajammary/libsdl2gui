#include "LSG_XML.h"

LibXml::xmlNode* LSG_XML::AddChildNode(LibXml::xmlNode* xmlNode, const std::string& xmlName)
{
	if (!xmlNode || xmlName.empty())
		return nullptr;

	auto xmlChildName = reinterpret_cast<const LibXml::xmlChar*>(xmlName.c_str());
	auto xmlChildNode = LibXml::xmlNewNode(nullptr, xmlChildName);

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

	LSG_XmlNodes nodes = {};

	auto xmlChildNode = xmlNode->xmlChildrenNode;

	while (xmlChildNode) {
		nodes.push_back(xmlChildNode);
		xmlChildNode = xmlChildNode->next;
	}

	return nodes;
}

LibXml::xmlNode* LSG_XML::GetNode(const std::string& xpath)
{
	auto xmlDoc = LSG_UI::GetXmlDocument();

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

std::string LSG_XML::GetValue(LibXml::xmlNode* node)
{
	auto xmlDoc = LSG_UI::GetXmlDocument();

	if (!node || !xmlDoc)
		return "";

	auto xmlValue = LibXml::xmlNodeListGetString(xmlDoc, node->children, 1);
	auto value    = (xmlValue ? std::string(reinterpret_cast<char*>(xmlValue)) : "");

	LibXml::xmlFree(xmlValue);

	return value;
}

LibXml::xmlDoc* LSG_XML::Open(const std::string& xmlFile)
{
	return LibXml::xmlReadFile(xmlFile.c_str(), nullptr, 0);
}

void LSG_XML::RemoveChildNodes(LibXml::xmlNode* xmlNode)
{
	if (!xmlNode)
		return;

	auto xmlChildNode = xmlNode->children;

	while (xmlChildNode) {
		LSG_XML::RemoveNode(xmlChildNode);
		xmlChildNode = xmlChildNode->next;
	}
}

void LSG_XML::RemoveNode(LibXml::xmlNode* xmlNode)
{
	if (!xmlNode)
		return;

	LibXml::xmlUnlinkNode(xmlNode);
	LibXml::xmlFreeNode(xmlNode);
}

void LSG_XML::SetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute, const std::string& value)
{
	if (!xmlNode || attribute.empty())
		return;

	auto xmlAttribute = reinterpret_cast<const LibXml::xmlChar*>(attribute.c_str());
	auto xmlValue     = reinterpret_cast<const LibXml::xmlChar*>(value.c_str());

	LibXml::xmlSetProp(xmlNode, xmlAttribute, xmlValue);
}
