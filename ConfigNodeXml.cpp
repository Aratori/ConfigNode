#include "ConfigNodeXml.h"
//typedef std::shared_ptr<std::multimap<ConfigNode*,ConfigNode*> > ConfigNodeMap;

TiXmlElement*	tinyNode;	

/**
* Reading XML or JSON file into our config structure.
* @param filename path to the file.
* @return true, if file was read successfully; false, otherwise.
*/
bool ConfigNodeXml::load(const std::string &filename) {
    TiXmlDocument conf(filename);
    bool tog;

	tog	=	conf.LoadFile();
	
    tinyNode = conf.FirstChildElement();

    nextNode(tinyNode, top);
    
	return tog;
}
void    ConfigNodeXml::nextNode(TiXmlElement *parentElem, ConfigNode *parentNode) {
	TiXmlNode*  node    =   parentElem->FirstChild();
	unsigned int type = node->Type();
	TiXmlElement*	elem;
	TiXmlText*	value;
	ConfigNodeTag* child;
	ConfigNodeValue*	childValue;
	
	while(node != NULL) {
		if(type == 1)
		{
				elem = node->ToElement();
				child = dynamic_cast<ConfigNodeTag*>(setNode(parentNode, elem->ValueStr(), true));
		}
		else
		{
			value	=	node->ToText();
			setNode(parentNode, value->ValueStr(), false);
			node 	= 	node->NextSiblingElement();
			continue;
		}
             
       //getting attributes
       TiXmlAttribute *attr = elem->FirstAttribute();
       while (attr != NULL) {
             (child->attribs).insert(strPair(attr->Name(), attr->Value()));
             attr = attr->Next();
       }
             
       if (!node->NoChildren())//если нет потомков
		nextNode(node->ToElement(), dynamic_cast<ConfigNode*>(child));
           node = node->NextSiblingElement();
	}
}



