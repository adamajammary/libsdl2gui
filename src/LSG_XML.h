#include "main.h"

#ifndef LSG_XML_H
#define LSG_XML_H

using LSG_XmlNodes = std::vector<LibXml::xmlNode*>;

class LSG_XML
{
private:
	LSG_XML()  {}
	~LSG_XML() {}

public:
	static LibXml::xmlNode* AddChildNode(LibXml::xmlNode* xmlNode, LibXml::xmlDoc* xmlDoc, const std::string& xmlName, const std::string& value = "");
	static std::string      GetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute);
	static LSG_UMapStrStr   GetAttributes(LibXml::xmlNode* xmlNode);
	static LSG_XmlNodes     GetChildNodes(LibXml::xmlNode* xmlNode);
	static LibXml::xmlNode* GetNode(const std::string& xpath, LibXml::xmlDoc* xmlDoc);
	static std::string      GetValue(LibXml::xmlNode* node, LibXml::xmlDoc* xmlDoc);
	static LibXml::xmlDoc*  Open(const std::string& xmlFile);
	static void             RemoveNode(LibXml::xmlNode* xmlNode);
	static void             SetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute, const std::string& value);
	static void             SetValue(LibXml::xmlNode* xmlNode, const std::string& value);

};

#endif
