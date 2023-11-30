#include "main.h"

#ifndef LSG_XML_H
#define LSG_XML_H

class LSG_XML
{
private:
	LSG_XML()  {}
	~LSG_XML() {}

public:
	static LibXml::xmlNode* AddChildNode(LibXml::xmlNode* xmlNode, const std::string& xmlName);
	static std::string      GetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute);
	static LSG_UMapStrStr   GetAttributes(LibXml::xmlNode* xmlNode);
	static LSG_XmlNodes     GetChildNodes(LibXml::xmlNode* xmlNode);
	static LibXml::xmlNode* GetNode(const std::string& xpath);
	static std::string      GetValue(LibXml::xmlNode* node);
	static LibXml::xmlDoc*  Open(const std::string& xmlFile);
	static void             RemoveChildNodes(LibXml::xmlNode* xmlNode);
	static void             RemoveNode(LibXml::xmlNode* xmlNode);
	static void             SetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute, const std::string& value);

};

#endif
