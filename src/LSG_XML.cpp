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

//LIB_XML::xmlNode* XML::VM_XML::GetChildNode(LIB_XML::xmlNode* node, const char* child, LIB_XML::xmlDoc* document)
//{
//	LIB_XML::xmlNode* result = NULL;
//
//	if ((node == NULL) || (child == NULL) || (document == NULL))
//		return result;
//
//	LIB_XML::xmlNode* childNode = node->xmlChildrenNode;
//
//	while (childNode != NULL)
//	{
//		if (LIB_XML::xmlStrcmp(childNode->name, reinterpret_cast<const LIB_XML::xmlChar*>(child)) == 0) {
//			result = childNode;
//			break;
//		}
//
//		childNode = childNode->next;
//	}
//
//	return result;
//}

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

//String XML::VM_XML::GetChildValue(LIB_XML::xmlNode* node, const char* child, LIB_XML::xmlDoc* document)
//{
//	String result = "";
//
//	if ((node == NULL) || (child == NULL) || (document == NULL))
//		return result;
//
//	LIB_XML::xmlNode* childNode = node->xmlChildrenNode;
//
//	while (childNode != NULL)
//	{
//		if (LIB_XML::xmlStrcmp(childNode->name, reinterpret_cast<const LIB_XML::xmlChar*>(child)) == 0)
//		{
//			LIB_XML::xmlChar* childValue = xmlNodeListGetString(
//				document, childNode->xmlChildrenNode, 1
//			);
//
//			result = String(reinterpret_cast<char*>(childValue));
//
//			LIB_XML::xmlFree(childValue);
//
//			break;
//		}
//
//		childNode = childNode->next;
//	}
//
//	return result;
//}

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

//XML::VM_XmlNodes XML::VM_XML::GetNodes(const char* xpath, LIB_XML::xmlDoc* document)
//{
//	VM_XmlNodes result;
//
//	if ((xpath == NULL) || (document == NULL))
//		return result;
//
//	LIB_XML::xmlXPathContext* context = LIB_XML::xmlXPathNewContext(document);
//
//	if (context == NULL)
//		return result;
//
//	LIB_XML::xmlXPathObject* object = LIB_XML::xmlXPathEvalExpression(
//		reinterpret_cast<const LIB_XML::xmlChar*>(xpath), context
//	);
//
//	LIB_XML::xmlXPathFreeContext(context);
//
//	if (object == NULL)
//		return result;
//
//	for (int i = 0; i < object->nodesetval->nodeNr; i++) {
//		if (object->nodesetval->nodeTab[i] != NULL)
//			result.push_back(object->nodesetval->nodeTab[i]);
//	}
//
//	LIB_XML::xmlXPathFreeObject(object);
//
//	return result;
//}
//
//String XML::VM_XML::GetValue(const char* xpath, LIB_XML::xmlDoc* document)
//{
//	String result = "";
//
//	if ((xpath == NULL) || (document == NULL))
//		return result;
//
//	LIB_XML::xmlXPathContext* context = xmlXPathNewContext(document);
//
//	if (context == NULL)
//		return result;
//
//	LIB_XML::xmlXPathObject* object = LIB_XML::xmlXPathEvalExpression(
//		reinterpret_cast<const LIB_XML::xmlChar*>(xpath), context
//	);
//
//	LIB_XML::xmlXPathFreeContext(context);
//
//	if (object == NULL)
//		return result;
//
//	if (object->nodesetval->nodeNr > 0)
//	{
//		LIB_XML::xmlChar* node = LIB_XML::xmlNodeListGetString(document, object->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
//
//		if (node != NULL)
//			result = String(reinterpret_cast<char*>(node));
//
//		LIB_XML::xmlFree(node);
//	}
//
//	LIB_XML::xmlXPathFreeObject(object);
//
//	return result;
//}

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

//LIB_XML::xmlDoc* XML::VM_XML::Load(const char* memory, int size)
//{
//	LIB_XML::xmlDoc* document = NULL;
//
//	if ((memory == NULL) || (size <= 0))
//		return NULL;
//
//	String response = VM_Text::Replace(memory, "xmlns=", "ns=");
//	document        = LIB_XML::xmlParseMemory(response.c_str(), (int)response.size());
//
//	return document;
//}

void LSG_XML::SetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute, const std::string& value)
{
	if (!xmlNode || attribute.empty())
		return;

	auto attr = reinterpret_cast<const LibXml::xmlChar*>(attribute.c_str());
	auto val  = reinterpret_cast<const LibXml::xmlChar*>(value.c_str());

	LibXml::xmlSetProp(xmlNode, attr, val);
}
