#include "ConfigNodeJson.h"


typedef	std::pair<ConfigNode*, ConfigNode*> nodePair;
typedef std::pair<std::string, std::string>	strPair;
typedef	std::pair <std::multimap<ConfigNode*,ConfigNode*>::iterator, std::multimap<ConfigNode*,ConfigNode*>::iterator> itrPair;

ConfigNode* stepPointer = 0;
ConfigNode	targetPointer;

ConfigNodeJson::ConfigNodeJson() {
	top = new ConfigNode;
	top->children.reset(new std::multimap<ConfigNode*, ConfigNode*>);	//set multimap
    stepPointer = top;
}

ConfigNodeJson::~ConfigNodeJson() {
    delete top;
}

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
	
	reader.parse(fin, root, false);
	
	stepPointer = top;
	
	nextNode(root, top);
	
	fin.close();
}

void    ConfigNodeJson::nextNode(const Json::Value &root, ConfigNode* parentNode) 
{
	//if object with attributes and/or other tags
	Json::Value::Members	members;
	Json::ValueIterator		itr		=	root.begin();
	
	if(!root.isArray())
		members	=	root.getMemberNames();
		
	for(int i = 0; itr != root.end(); itr++, i++)
	{
		ConfigNodeTag*	tag;
		Json::Value	childValue;
		ConfigNode*	child;
		
		childValue	=	*itr;
			
		if(!childValue.size() && (tag = dynamic_cast<ConfigNodeTag*>(parentNode)) != 0)	//if attribute
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

ConfigNode*	ConfigNodeJson::setNode(ConfigNode*	parent, std::string val, bool tag/*set tag/value node*/)
{
	ConfigNode*	ret;
	if(tag)
	{
		ConfigNodeTag* child	=	new ConfigNodeTag();
		child->parent	=	parent;
		child->children	=	parent->children;
		parent->children->insert(nodePair(parent, (ConfigNode*)child));
		child->name		=	val;
		ret	=		child;
	}
	else
	{
		ConfigNodeValue* child	=	new ConfigNodeValue();
		child->parent	=	parent;
		child->children	=	parent->children;
		parent->children->insert(nodePair(parent, (ConfigNode*)child));
		child->value	=	val;
		ret	=	child;
	}		
		
	return ret;
}


void ConfigNodeJson::dump(std::stringstream& target, uint32_t indent) const
{
    if(stepPointer == 0)
        stepPointer = top;
    typename std::multimap<ConfigNode*,ConfigNode*>::iterator it,it_2;
    std::string indentStr(indent, ' ');
    ConfigNodeTag* tagPointer;
    ConfigNodeValue* valuePointer;

    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);//get node children


    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {
        ConfigNode* currentNode = it->second;

        //print node
        target<<indentStr;//print indent
        if((tagPointer = dynamic_cast<ConfigNodeTag *>(currentNode)) != 0)
        {
            target<<"Element{"<<tagPointer->name;
            //print attributes
            std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
            for(; mapIt != tagPointer->attribs.end(); mapIt++)
                target<<"# "<<mapIt->first<<":"<<mapIt->second;
            target<<"}";
        }
        else if((valuePointer = dynamic_cast<ConfigNodeValue *>(currentNode)) != 0)
                target<<"Value{"<<valuePointer->value<<"}";
        target<<std::endl;

        //pass children
        stepPointer = currentNode;
        dump(target, indent+=2);
        stepPointer = currentNode->parent;


    }

    stepPointer = 0;
}

std::string	ConfigNodeJson::toString() const
{
	std::stringstream target;
	
	this->dump(target,0);
	
	return target.str();
}

ConfigNodeMap	ConfigNodeJson::getChildren()
{
		return top->children;
}

ConfigNode		ConfigNodeJson::findChildNode(const std::string& node, const std::string& attr, const std::string& value) const
{
	if(stepPointer == 0)
		stepPointer = top;
		
    ConfigNodeTag* tagPointer;
    
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {	
		
		stepPointer = it->second;
		targetPointer = findChildNode(node, attr, value);
		stepPointer = (it->second)->parent;
		
		if((tagPointer = dynamic_cast<ConfigNodeTag *>(it->second)) == 0)//if wrong type
			continue;
			
		if((tagPointer->name).compare(node) != 0)//if wrong name
			continue;
			
		//check attributes
		if(attr.compare("") != 0){
			std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
			for(; mapIt != tagPointer->attribs.end(); mapIt++)
			{	
				if((mapIt->first).compare(attr) != 0)
					continue;
				if((mapIt->second).compare(value) == 0)
					targetPointer = *tagPointer;
			}
		}
	}
	
	stepPointer = 0;
	
	return targetPointer;
}

		/**
		 * Lookup request function. Searches in the existing node tree and
		 * return the resulting node.
		 *
		 * Request should be formatted like:
		 *  "nodeA.nodeB#name:target.nodeC#attrX:attrValue"
		 *
		 * This request will lookup from the current node to the nodeC.
		 * It assumes that tree looks like this:
		 *  <CurrentNode>
		 *      <nodeA>
		 *          <nodeB name="target">
		 *              <nodeC attrX="attrValue"/>
		 *          </nodeB>
		 *          <nodeB name="one more node"/>
		 *      </nodeA>
		 *  </CurrentNode>
		 *
		 * Return value will either contain ConfigNode of the node matching
		 * searching sequence or ConfigNode(nullptr).
		 *
		 * @param path node name path.
		 * @return first matching node or ConfigNode(nullptr).
		 */
ConfigNode ConfigNodeJson::findNode(const std::string& path) const
{
	if(stepPointer == 0)
		stepPointer = top;
		
	static unsigned int 	deep_counter	=	0;
	typename std::multimap<ConfigNode*,ConfigNode*>::iterator it;
    ConfigNodeTag* tagPointer;
    ConfigNode		targetPointer;
    bool			end		=	false;
    
    //get name, attributes and attributes's value for current deep level
	char*	c_path	=	strdup(path.c_str());
	char*	tag;
	std::vector<std::pair<std::string, std::string> > attributes;
	
	//get str with tagname, attributes, attributes value
	tag = strtok(c_path, ".");
	for(int i = 0; i < deep_counter; i++)
		tag = strtok(NULL, ".");
		
	if(strtok(NULL, ".") == NULL)
		end = true;
		
	//get tagname
	std::string tagName(strtok(tag,"#"));
	c_path	=	strtok(NULL, "#");			//get str with attributes
	
	//get attributes
	c_path = strtok(c_path, ",:");
	
	while(c_path != NULL)
	{
		std::string attrName(c_path);
		std::string attrValue(strtok(NULL, ",:"));
		attributes.push_back(std::pair<std::string, std::string>(attrName, attrValue));
		c_path	=	strtok(NULL, ",:");
	}
    
    std::pair <std::multimap<ConfigNode*,ConfigNode*>::iterator, std::multimap<ConfigNode*,ConfigNode*>::iterator> ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {			
		if((tagPointer = dynamic_cast<ConfigNodeTag *>(it->second)) == 0)//if wrong type
			continue;
			
		if((tagPointer->name).compare(tagName) != 0)//if wrong name
			continue;
			
		//check attributes
		std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
		for(int i = 0; i < attributes.size() ; mapIt++, i++)
			{	
			if((mapIt->first).compare(attributes[i].first) != 0)
				continue;
			if((mapIt->second).compare(attributes[i].second) == 0)
			{
				if(end)
				{
					return (*tagPointer);
				}
			}
		}
		stepPointer = it->second;
		deep_counter++;
		targetPointer = findNode(path);
		deep_counter--;
		stepPointer = (it->second)->parent;
	}

	
	return targetPointer;
}

/**
 * Checks whether node matching given path exists or not.
 * Searches nodes hierarchy if complex path is supplied.
 * @param path node name path.
 * @return true, if attribute exists; false, otherwise.
*/
bool ConfigNodeJson::hasNode(const std::string& path) const
{
	if(stepPointer == 0)
		stepPointer = top;
		
	static unsigned int 	deep_counter	=	0;
	typename std::multimap<ConfigNode*,ConfigNode*>::iterator it;
    ConfigNodeTag* tagPointer;
    bool			end		=	false;
    bool			has		=	false;
    
    //get name, attributes and attributes's value for current deep level
	char*	c_path	=	strdup(path.c_str());//convert to char*
	char*	tag;
	std::vector<std::pair<std::string, std::string> > attributes;
	
	//get str with tagname, attributes, attributes value
	tag = strtok(c_path, ".");
	for(int i = 0; i < deep_counter; i++)
		tag = strtok(NULL, ".");
		
	if(strtok(NULL, ".") == NULL)
		end = true;
		
	//get tagname
	std::string tagName(strtok(tag,"#"));
	c_path	=	strtok(NULL, "#");			//get str with attributes
	
	//get attributes
	c_path = strtok(c_path, ",:");
	
	while(c_path != NULL)
	{
		std::string attrName(c_path);
		std::string attrValue(strtok(NULL, ",:"));
		attributes.push_back(std::pair<std::string, std::string>(attrName, attrValue));
		c_path	=	strtok(NULL, ",:");
	}
    
    std::pair <std::multimap<ConfigNode*,ConfigNode*>::iterator, std::multimap<ConfigNode*,ConfigNode*>::iterator> ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {			
		if((tagPointer = dynamic_cast<ConfigNodeTag *>(it->second)) == 0)//if wrong type
			continue;
			
		if((tagPointer->name).compare(tagName) != 0)//if wrong name
			continue;
			
		//check attributes
		std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
		for(int i = 0; i < attributes.size() ; mapIt++, i++)
		{	
			if((mapIt->first).compare(attributes[i].first) != 0)
				continue;
			if((mapIt->second).compare(attributes[i].second) != 0)
				continue;
		}
		if(end)
			return end;
		else
		{
			stepPointer = it->second;
			deep_counter++;
			has = hasNode(path);
			deep_counter--;
			stepPointer = (it->second)->parent;
		}
	}

	
	return has;
}

/**
 * Node object getter.
 * Searches nodes hierarchy if complex path is supplied.
 * Throws exception if node does not exist.
 * @param path node name path.
 * @return ConfigNode holding matching object.
*/
ConfigNode ConfigNodeJson::getNode(const std::string& path) const
{
		if(stepPointer == 0)
		stepPointer = top;
		
	static unsigned int 	deep_counter	=	0;
	typename std::multimap<ConfigNode*,ConfigNode*>::iterator it;
    ConfigNodeTag* tagPointer;
    ConfigNode		targetPointer;
    bool			end		=	false;
    
    //get name, attributes and attributes's value for current deep level
	char*	c_path	=	strdup(path.c_str());
	char*	tag;
	std::vector<std::pair<std::string, std::string> > attributes;
	
	//get str with tagname, attributes, attributes value
	tag = strtok(c_path, ".");
	for(int i = 0; i < deep_counter; i++)
		tag = strtok(NULL, ".");
		
	if(strtok(NULL, ".") == NULL)
		end = true;
		
	//get tagname
	std::string tagName(strtok(tag,"#"));
	c_path	=	strtok(NULL, "#");			//get str with attributes
	
	//get attributes
	c_path = strtok(c_path, ",:");
	
	while(c_path != NULL)
	{
		std::string attrName(c_path);
		std::string attrValue(strtok(NULL, ",:"));
		attributes.push_back(std::pair<std::string, std::string>(attrName, attrValue));
		c_path	=	strtok(NULL, ",:");
	}
    
    std::pair <std::multimap<ConfigNode*,ConfigNode*>::iterator, std::multimap<ConfigNode*,ConfigNode*>::iterator> ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {			
		if((tagPointer = dynamic_cast<ConfigNodeTag *>(it->second)) == 0)//if wrong type
			continue;
			
		if((tagPointer->name).compare(tagName) != 0)//if wrong name
			continue;
			
		//check attributes
		std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
		for(int i = 0; i < attributes.size() ; mapIt++, i++)
			{	
			if((mapIt->first).compare(attributes[i].first) != 0)
				continue;
			if((mapIt->second).compare(attributes[i].second) == 0)
			{
				if(end)
				{
					return (*tagPointer);
				}
			}
		}
		stepPointer = it->second;
		deep_counter++;
		targetPointer = getNode(path);
		deep_counter--;
		stepPointer = (it->second)->parent;
	}

	
	return targetPointer;
}

/**
 * Return attribute value or default value (if attribute does not exist).
 * Searches nodes hierarchy if complex path is supplied.
 * @param path node/attribute search path.
 * @param def default value.
 * @return attribute value or default value.
 * "parent.child#attr1:val.superchild"
*/
template <typename Y>
Y ConfigNodeJson::getAttr(const std::string& path, Y def) const
{
	if(stepPointer == 0)
		stepPointer = top;
		
	static unsigned int 	deep_counter	=	0;
	typename std::multimap<ConfigNode*,ConfigNode*>::iterator it;
	std::vector<std::pair<std::string, std::string> > attributes;
    ConfigNodeTag* tagPointer;
    Y				targetAttr = def;
    bool			end		=	false;
    
    //get name, attributes and attributes's value for current deep level
	char*	c_path	=	strdup(path.c_str());
	char*	tag;
	std::string attributeName;
	
	//get str with tagname, attributes, attributes value
	tag = strtok(c_path, ".");
	for(int i = 0; i < deep_counter; i++)
		tag = strtok(NULL, ".");
		
	if(strtok(NULL, ".") == NULL)
		end = true;
		
	//get tagname
	std::string tagName(strtok(tag,"#"));
	c_path	=	strtok(NULL, "#");			//get str with attributes
	
	//get attributes
	if(end)
		attributeName.assign(strtok(c_path,",:"));	//sought-for attribute
	else
	{										//usual search
		c_path = strtok(c_path, ",:");
		
		while(c_path != NULL)
		{
			std::string attrName(c_path);
			std::string attrValue(strtok(NULL, ",:"));
			attributes.push_back(std::pair<std::string, std::string>(attrName, attrValue));
			c_path	=	strtok(NULL, ",:");
		}
	}
    
    std::pair <std::multimap<ConfigNode*,ConfigNode*>::iterator, std::multimap<ConfigNode*,ConfigNode*>::iterator> ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {			
		if((tagPointer = dynamic_cast<ConfigNodeTag *>(it->second)) == 0)//if wrong type
			continue;
			
		if((tagPointer->name).compare(tagName) != 0)//if wrong name
			continue;
			
		//check attributes

		
		std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
		for(int i = 0; mapIt != tagPointer->attribs.end() ; mapIt++, i++)
		{			
			if(end)					//search for attribute
			{
				if((mapIt->first).compare(attributeName) == 0)
					return (Y)(mapIt->second);
			}
			else                   //else continue path check
			{
				if((mapIt->first).compare(attributes[i].first) != 0)
					continue;
				if((mapIt->second).compare(attributes[i].second) != 0)
					continue;
			}
		}
		stepPointer = it->second;
		deep_counter++;
		targetAttr = getAttr(path, def);
		deep_counter--;
		stepPointer = (it->second)->parent;
	}

	
	return targetAttr;
}

/**
 * Return attribute value.
 * Searches nodes hierarchy if complex path is supplied.
 * Throws exception if attribute does not exist.
 * @param path node/attribute search path.
 * @return attribute value.
*/
template <typename Y>
Y ConfigNodeJson::getAttr(const std::string& path) const
{
	if(stepPointer == 0)
		stepPointer = top;
		
	static unsigned int 	deep_counter	=	0;
	typename std::multimap<ConfigNode*,ConfigNode*>::iterator it;
	std::vector<std::pair<std::string, std::string> > attributes;
    ConfigNodeTag* tagPointer;
    Y				targetAttr = 0;
    bool			end		=	false;
    
    //get name, attributes and attributes's value for current deep level
	char*	c_path	=	strdup(path.c_str());
	char*	tag;
	std::string attributeName;
	
	//get str with tagname, attributes, attributes value
	tag = strtok(c_path, ".");
	for(int i = 0; i < deep_counter; i++)
		tag = strtok(NULL, ".");
		
	if(strtok(NULL, ".") == NULL)
		end = true;
		
	//get tagname
	std::string tagName(strtok(tag,"#"));
	c_path	=	strtok(NULL, "#");			//get str with attributes
	
	//get attributes
	if(end)
		attributeName.assign(strtok(c_path,",:"));	//sought-for attribute
	else
	{										//usual search
		c_path = strtok(c_path, ",:");
		
		while(c_path != NULL)
		{
			std::string attrName(c_path);
			std::string attrValue(strtok(NULL, ",:"));
			attributes.push_back(std::pair<std::string, std::string>(attrName, attrValue));
			c_path	=	strtok(NULL, ",:");
		}
	}
    
    std::pair <std::multimap<ConfigNode*,ConfigNode*>::iterator, std::multimap<ConfigNode*,ConfigNode*>::iterator> ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {			
		if((tagPointer = dynamic_cast<ConfigNodeTag *>(it->second)) == 0)//if wrong type
			continue;
			
		if((tagPointer->name).compare(tagName) != 0)//if wrong name
			continue;
			
		//check attributes

		
		std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
		for(int i = 0; mapIt != tagPointer->attribs.end() ; mapIt++, i++)
		{			
			if(end)					//search for attribute
			{
				if((mapIt->first).compare(attributeName) == 0)
					return (Y)(mapIt->second);
			}
			else                   //else continue path check
			{
				if((mapIt->first).compare(attributes[i].first) != 0)
					continue;
				if((mapIt->second).compare(attributes[i].second) != 0)
					continue;
			}
		}
		stepPointer = it->second;
		deep_counter++;
		targetAttr = getAttr<Y>(path);
		deep_counter--;
		stepPointer = (it->second)->parent;
	}

	
	return targetAttr;
}


