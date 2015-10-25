#include "ConfigNodeBase.h"

ConfigNodeBase::ConfigNodeBase() {
	top = new ConfigNodeData();
	top->children.reset(new std::multimap<ConfigNode, ConfigNode>);	//set multimap
}

ConfigNodeBase::~ConfigNodeBase() {
    delete top;
}


ConfigNode	ConfigNodeBase::setNode(ConfigNode	parent, std::string val, bool tag/*set tag/value node*/)
{
	ConfigNode child;
	
	if(tag)
		child = new ConfigNodeTagData();
	else
		child = new ConfigNodeValueData();
	
	child->parent =	parent;
	child->children	= parent->children;
	parent->children->insert(nodePair(parent, (ConfigNode)child));
	
	if(tag)
	{
		ConfigNodeTag childTag = (ConfigNodeTag)child;
		childTag->name = val;
	}
	else
	{
		ConfigNodeValue childValue = (ConfigNodeValue)child;
		childValue->value = val;
	}	
		
	return child;
}


void ConfigNodeBase::dump(std::stringstream& target, uint32_t indent) const
{
	static ConfigNode stepPointer;
	if(indent == 0)
		stepPointer	=	top;
    std::string indentStr(indent, ' ');
    ConfigNodeTag tagPointer;
    ConfigNodeValue valuePointer;

    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);//get node children

    for(auto it = ret.first; it != ret.second; ++it)
    {
        ConfigNode currentNode = it->second;

        //print node
        target<<indentStr;//print indent
        if((tagPointer = dynamic_cast<ConfigNodeTag>(currentNode)) != 0)
        {
            target<<"Element{"<<tagPointer->name;
            //print attributes
            auto   mapIt = tagPointer->attribs.begin();
            for(; mapIt != tagPointer->attribs.end(); mapIt++)
                target<<"# "<<mapIt->first<<":"<<mapIt->second;
            target<<"}";
        }
        else if((valuePointer = dynamic_cast<ConfigNodeValue>(currentNode)) != 0)
                target<<"Value{"<<valuePointer->value<<"}";
        target<<std::endl;

        //pass children
        stepPointer = currentNode;
        dump(target, indent+=2);
        stepPointer = currentNode->parent;
    }	
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

bool	ConfigNodeBase::compareNodes(ConfigNode node,const std::string& name,const std::vector<strPair>& attr) const
{
	ConfigNodeTag tagPointer;
	
	if((tagPointer = dynamic_cast<ConfigNodeTag>(node)) == 0 )//if wrong type
		return false;
	
	if((tagPointer->name).compare(name) != 0)//if wrong name
		return false;
		
	//check attributes
	if(attr.size() != 0){
		auto   mapIt = tagPointer->attribs.begin();
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
	std::string	c_path	=	path;
	std::string	tag;
	bool	end	=	false;
	
	//get str with tagname, attributes, attributes value
	int startPos = 0;
	int endPos	= c_path.find('.', startPos);
	tag = c_path.substr(startPos, endPos);
	
	for(int i = 0; i < deep; i++)
	{
		startPos	+=	endPos + 1;
		endPos		=	c_path.find('.', startPos) - startPos;
		tag = c_path.substr(startPos, endPos);
		
	}
	if(c_path.find('.', startPos) == std::string::npos)
		end = true;
		
	//get tagname
	if(tag.find('#') < tag.find('.'))
		endPos = tag.find('#');
	else
		endPos = tag.find('.');
		
	tagName = tag.substr(0, endPos);
	startPos = 0;
	
	
	//get attributes
	if(tag.find(':') != std::string::npos)
		do{
			startPos	+=	endPos + 1;
			endPos	=	tag.find(':', startPos) - startPos;
			std::string attrName = tag.substr(startPos, endPos);
			
			startPos += endPos + 1;
			endPos = tag.find(',', startPos) - startPos;
			std::string attrValue = tag.substr(startPos,endPos);
			
			attributes.push_back(std::pair<std::string, std::string>(attrName, attrValue));	
		}while(tag.find(':',endPos) != std::string::npos);
	
	return end;
}

ConfigNode	ConfigNodeBase::findChildNode(const std::string& node, const std::string& attr, const std::string& value) const
{
	static ConfigNode stepPointer = top;
	static ConfigNode targetPointer	= top;
    ConfigNodeTag tagPointer;
    
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);//get node children
    
    for(auto it = ret.first; it != ret.second; ++it)
    {		
		ConfigNode child = it->second;
		std::vector<strPair>	attrs;
		attrs.push_back(std::pair<std::string, std::string>(attr, value));
		if(this->compareNodes(child, node, attrs) == true)	
			targetPointer = child;
			
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
	static ConfigNode stepPointer = top;
	static unsigned int 	deep_counter	=	0;
    ConfigNode		targetPointer = nullptr;
    
    std::string tagName;
    std::vector<strPair> attributes;
    bool end	=	parsePath(path, tagName, attributes, deep_counter);
    
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);															//get node children
    
    for(auto it = ret.first; it != ret.second; ++it)
    {
		if(compareNodes(it->second, tagName, attributes) && end)
			return it->second;
		
		stepPointer = it->second;
		deep_counter++;
		targetPointer = findNode(path);
		if(targetPointer != nullptr)
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
	return findNode(path) == nullptr;
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
	if(tag)
		return findNode(path);		//поставить здесь проверку на исключения
	else
		throw	std::runtime_error("Node not found");
}
