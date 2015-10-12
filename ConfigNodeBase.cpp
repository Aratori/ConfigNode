#include "ConfigNodeBase.h"
#include <string>


typedef	std::pair<ConfigNode*, ConfigNode*> nodePair;
typedef	std::pair <std::multimap<ConfigNode*,ConfigNode*>::iterator, std::multimap<ConfigNode*,ConfigNode*>::iterator> itrPair;


ConfigNodeBase::ConfigNodeBase() {
	top = new ConfigNode;
	top->children.reset(new std::multimap<ConfigNode*, ConfigNode*>);	//set multimap
}

ConfigNodeBase::~ConfigNodeBase() {
    delete top;
}


ConfigNode*	ConfigNodeBase::setNode(ConfigNode*	parent, std::string val, bool tag/*set tag/value node*/)
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


void ConfigNodeBase::dump(std::stringstream& target, uint32_t indent) const
{
	static ConfigNode*	stepPointer		=	top;
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

    stepPointer = 0	;
}

std::string	ConfigNodeBase::toString() const
{
	std::stringstream target;
	
	this->dump(target,0);
	
	return target.str();
}

ConfigNodeMap	ConfigNodeBase::getChildren()
{
	return top->children;
}

bool	ConfigNodeBase::compareNodes(ConfigNode* node,const std::string& name,const std::vector<strPair>& attr) const
{
	ConfigNodeTag* tagPointer;
	
	if((tagPointer = dynamic_cast<ConfigNodeTag *>(node)) == 0 )//if wrong type
		return false;
	
	if((tagPointer->name).compare(name) != 0)//if wrong name
		return false;
		
	//check attributes
	if(attr.size() != 0){
		std::multimap<std::string, std::string>::iterator   mapIt = tagPointer->attribs.begin();
		for(int i = 0; i <  attr.size();i++, mapIt++)
		{	
			if((mapIt->first).compare(attr[i].first) != 0)
				return false;
			if((mapIt->second).compare(attr[i].second) != 0)
				return false;
		}
	}
	
	return true;
}

bool	ConfigNodeBase::parsePath(const std::string& path, std::string& tagName, std::vector<strPair>& attributes, int deep) const
{
    //get name, attributes and attributes's value for current deep level
	char*	c_path	=	strdup(path.c_str());
	char*	tag;
	bool	end	=	false;
	
	//get str with tagname, attributes, attributes value
	tag = strtok(c_path, ".");
	for(int i = 0; i < deep; i++)
		tag = strtok(NULL, ".");
		
		
	if(strtok(NULL, ".") == NULL)
		end = true;
		
	//get tagname
	tagName.assign(strtok(tag,"#"));
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
	
	return end;
}

ConfigNode		ConfigNodeBase::findChildNode(const std::string& node, const std::string& attr, const std::string& value) const
{
	static ConfigNode* stepPointer = top;
	static ConfigNode	targetPointer	=	*top;
    ConfigNodeTag* tagPointer;
    
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {		
		ConfigNode* child = it->second;
		std::vector<strPair>	attrs;
		attrs.push_back(std::pair<std::string, std::string>(attr, value));
		if(this->compareNodes(child, node, attrs) == true)	
			targetPointer = *child;
			
		stepPointer = child;
		targetPointer = findChildNode(node, attr, value);
		stepPointer = child->parent;
	}
	
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
ConfigNode ConfigNodeBase::findNode(const std::string& path) const
{
	static ConfigNode* stepPointer = top;
	static unsigned int 	deep_counter	=	0;
	typename std::multimap<ConfigNode*,ConfigNode*>::iterator it;
    ConfigNode		targetPointer;
    
    std::string tagName;
    std::vector<strPair> attributes;
    bool end	=	parsePath(path, tagName, attributes, deep_counter);
    
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {
		if(compareNodes(it->second, tagName, attributes) && end)
			return *it->second;
		
		stepPointer = it->second;
		deep_counter++;
		targetPointer = findNode(path);
		if(targetPointer.parent != NULL)
			return targetPointer;
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
bool ConfigNodeBase::hasNode(const std::string& path) const
{	
	static ConfigNode* stepPointer	=	top;	
	static unsigned int 	deep_counter	=	0;
    bool			has		=	false;
    
    std::string tagName;
    std::vector<strPair> attributes;
    bool end	=	parsePath(path, tagName, attributes, deep_counter);
			
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {	
		if(!compareNodes(it->second, tagName, attributes))
			continue;
			
		if(end)
			return end;
		
		stepPointer = it->second;
		deep_counter++;
		has = hasNode(path);
		deep_counter--;
		stepPointer = (it->second)->parent;
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
ConfigNode ConfigNodeBase::getNode(const std::string& path) const
{
	ConfigNode	tag	=	findNode(path);
	if(tag.parent != NULL)
		return findNode(path);		//поставить здесь проверку на исключения
	else
		throw	std::runtime_error("Node not found");
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
Y ConfigNodeBase::getAttr(const std::string& path, Y def) const
{
	static ConfigNode*	stepPointer		=	top;
	static unsigned int 	deep_counter	=	0;
    Y				targetAttr = def;
    
    std::string tagName;
    std::vector<strPair> attributes;
    bool end	=	parsePath(path, tagName, attributes, deep_counter);
    
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);																//get node children
    
    for(std::multimap<ConfigNode*, ConfigNode*>::iterator it = ret.first; it != ret.second; ++it)
    {	
		if(compareNodes(it->second, tagName, attributes))
			if(end)
			{
				ConfigNodeTag*	tagPointer	=	dynamic_cast<ConfigNodeTag*>(it->second);
				if(tagPointer->attribs.size())
					return (Y)(*tagPointer->attribs.begin());
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
Y ConfigNodeBase::getAttr(const std::string& path) const
{
	Y attr 	=	getAttr<Y>(path, 0);
	if(attr != 0)
		return NULL;
	else
		throw	std::runtime_error("Attribute not found");
}	



