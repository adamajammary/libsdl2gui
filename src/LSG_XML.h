#include "main.h"

#ifndef LSG_XML_H
#define LSG_XML_H

typedef std::vector<LibXml::xmlNode*> LSG_XmlNodes;

class LSG_XML
{
private:
	LSG_XML()  {}
	~LSG_XML() {}

public:
	static LibXml::xmlNode* AddChildNode(LibXml::xmlNode* xmlNode, LibXml::xmlDoc* xmlDoc, const std::string& xmlName, const std::string& value = "");
	static std::string      GetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute);
	static LSG_UMapStrStr   GetAttributes(LibXml::xmlNode* xmlNode);
	//static LIB_XML::xmlNode* GetChildNode(LIB_XML::xmlNode* node, const char* child, LIB_XML::xmlDoc* document);
	static LSG_XmlNodes     GetChildNodes(LibXml::xmlNode* xmlNode);
	//static String            GetChildValue(LIB_XML::xmlNode* node, const char* child, LIB_XML::xmlDoc* document);
	static LibXml::xmlNode* GetNode(const std::string& xpath, LibXml::xmlDoc* xmlDoc);
	//static VM_XmlNodes       GetNodes(const char* xpath, LIB_XML::xmlDoc* document);
	//static String            GetValue(const char* xpath, LIB_XML::xmlDoc* document);
	static std::string      GetValue(LibXml::xmlNode* node, LibXml::xmlDoc* xmlDoc);
	static LibXml::xmlDoc*  Open(const std::string& xmlFile);
	//static LIB_XML::xmlDoc*  Load(const char* memory, int size);
	static void             SetAttribute(LibXml::xmlNode* xmlNode, const std::string& attribute, const std::string& value);

};

#endif
