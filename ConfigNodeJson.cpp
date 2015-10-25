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
	
	if(!root.isArray())
		members	=	root.getMemberNames();
		
	for(int i = 0; itr != root.end(); itr++, i++)
	{
		ConfigNodeTag	tag;
		Json::Value	childValue;
		ConfigNode	child;
		
		childValue	=	*itr;
			
		if(!childValue.size() && (tag = dynamic_cast<ConfigNodeTag>(parentNode)) != 0)	//if attribute
		{
			(tag->attribs).insert(strPair(members[i], (*itr).asString()));
			continue;
		}
			
		//if tag
		if(root.isArray())
			child 	=	setNode(parentNode, std::string("ArrayItem"), true);
		else
		 	child 	=	setNode(parentNode, members[i], true);
		if(childValue.size() == 0)
			setNode(child, (*itr).asString(), false);
		else
			nextNode(childValue, child);
	}
}

