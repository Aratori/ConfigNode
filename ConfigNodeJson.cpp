#include "ConfigNodeJson.h"


/**
* Reading XML or JSON file into our config structure.
* @param filename path to the file.
* @return true, if file was read successfully; false, otherwise.
*/
bool ConfigNodeJson::load(const std::string &filename)
{
    std::ifstream 	fin(filename);
    Json::Value	root;
    Json::Reader 	reader;
    bool tog =	reader.parse(fin, root, false);;

    nextNode(root, top);
    fin.close();

    return	tog;
}

void    ConfigNodeJson::nextNode(const Json::Value &root, ConfigNode parentNode)
{
    //if object with attributes and/or other tags
    Json::Value::Members	members;
    Json::ValueIterator		itr		=	root.begin();
    ConfigNodeTag tag;
    if(!root.isArray())
        members	=	root.getMemberNames();

    for(int i = 0; itr != root.end(); itr++, i++)
    {
        Json::Value	childValue;
        ConfigNode	child;
        childValue	=	*itr;

        //мы не можем здесь обойтись без dynamic_pointer_cast, по причине того, что мы не знаем каким типом располагаем
        //в jsoncpp есть только один тип данных - Value
        //в tinyxml мы можем обойтись без dynamic, потому что для каждого возможного типа узла есть свой тип(Text, Element, Document и тд)
        if(!childValue.size() && (tag = std::dynamic_pointer_cast<ConfigNodeTagData>(parentNode)) != 0)	//if attribute
        {
            (tag->attribs).insert(strPair(members[i], (*itr).asString()));
            continue;
        }

        //if tag
        if(root.isArray())
            child 	=	setTag(parentNode, std::string("ArrayItem"));
        else
            child 	=	setTag(parentNode, members[i]);

        std::string str;
        if(childValue.size() == 0)
        {
            str = (*itr).asString();
            setValue(child, str);
        }
        else
            nextNode(childValue, child);
    }
}

