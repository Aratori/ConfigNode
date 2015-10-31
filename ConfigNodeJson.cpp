#include "ConfigNodeJson.h"


/**
* Reading XML or JSON file into our config structure.
* @param filename path to the file.
* @return true, if file was read successfully; false, otherwise.
*/
bool ConfigNodeJson::load(const std::string &filename) {
	//parse file
	std::ifstream 	fin(filename);
	Json::Value	root;
	Json::Reader 	reader;
	bool tog;
	
	tog	=	reader.parse(fin, root, false);
	
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
			
		if(!childValue.size() && (tag = std::dynamic_pointer_cast<ConfigNodeTagData>(parentNode)) != 0)	//if attribute
		{
			(tag->attribs).insert(strPair(members[i], (*itr).asString()));
			continue;
		}
			
		//if tag
		if(root.isArray())
			child 	=	setNode(parentNode, std::string("ArrayItem"), true);
		else
		 	child 	=	setNode(parentNode, members[i], true);
		 
		std::string str;
		if(childValue.size() == 0)
		{
			str = (*itr).asString();
			setNode(child, str, false);
		}
		else
			nextNode(childValue, child);
	}
}

