#include "ConfigNodeXml.h"

/**
* Reading XML or JSON file into our config structure.
* @param filename path to the file.
* @return true, if file was read successfully; false, otherwise.
*/
bool ConfigNodeXml::load(const std::string &filename)
{
    TiXmlDocument conf(filename);
    TiXmlElement*	node;
    bool tog	=	conf.LoadFile();

    node = conf.FirstChildElement();
    nextNode(node, top);

    return tog;
}


void    ConfigNodeXml::nextNode(TiXmlElement *parentElem, ConfigNode parentNode)
{
    TiXmlNode*  node    =   parentElem->FirstChild();
    unsigned int type = node->Type();
    TiXmlElement*	elem;
    TiXmlText*	value;
    ConfigNodeTag child;

    while(node != NULL)
    {

        if(type == 1)
        {
            elem = node->ToElement();
            child = setTag(parentNode, elem->ValueStr());
        }
        else
        {
            value =	node->ToText();
            setValue(parentNode, value->ValueStr());
            node = node->NextSiblingElement();
            continue;
        }

        //getting attributes
        TiXmlAttribute *attr = elem->FirstAttribute();
        while (attr != NULL)
        {
            (child->attribs).insert(strPair(attr->Name(), attr->Value()));
            attr = attr->Next();
        }

        if (!node->NoChildren())//если есть потомки
            nextNode(node->ToElement(), (ConfigNode)child);
        node = node->NextSiblingElement();
    }
}
