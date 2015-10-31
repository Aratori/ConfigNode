#include "ConfigNodeBase.h"

ConfigNodeBase::ConfigNodeBase(): top(new ConfigNodeData()){
	top->children.reset(new std::multimap<ConfigNode, ConfigNode>);	
	//в самой структуре инициализируется только shared_ptr, не multimap, поэтому надо отдельно заносить в него multimap
	//multimap в shared_ptr нужен для того, чтобы передавать его в дочерние узлы
}


ConfigNode	ConfigNodeBase::setNode(ConfigNode	parent, std::string val, bool tog/*set tag/value node*/)
{
	ConfigNodeTagData* tag;
	ConfigNodeValueData* value;
	ConfigNodeData* data;
	
	if(tog)
	{
		tag = new ConfigNodeTagData();
		tag->name = val;
		data = tag;
	}
	else
	{
		value = new ConfigNodeValueData();
		value->value = val;
		data = value;
	}
	ConfigNode	child(data);
	
	child->parent =	parent;
	child->children	= parent->children;
	parent->children->insert(nodePair(parent,child));
	
		
	return child;
}


void ConfigNodeBase::dump(std::stringstream& target, uint32_t indent) const
{
	static ConfigNode stepPointer = top;
	if(indent == 0)
		stepPointer = top;
    std::string indentStr(indent, ' ');
   
    itrPair ret;
    ret = (stepPointer->children)->equal_range(stepPointer);//get node children

    for(auto it = ret.first; it != ret.second; ++it)
    {
        ConfigNode currentNode = it->second;
		ConfigNodeTag tagPointer;
		ConfigNodeValue valuePointer;
		
        //print node
        target<<indentStr;//print indent
        if((tagPointer = std::dynamic_pointer_cast<ConfigNodeTagData>(currentNode)) != 0)
        {
			target<<(*tagPointer);
        }
        else if((valuePointer = std::dynamic_pointer_cast<ConfigNodeValueData>(currentNode)) != 0)
                target<<*valuePointer;                
                
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
	
	if((tagPointer = std::dynamic_pointer_cast<ConfigNodeTagData>(node)) == 0 )//if wrong type
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

bool ConfigNodeBase::compareNodes(const ConfigNode parentR, const ConfigNode parentL) const
{
	auto retR = (parentR->children)->equal_range(parentR);
	auto retL = (parentL->children)->equal_range(parentL);
	bool comp = true;
	bool type = true;
	ConfigNodeTag	tagR, tagL;
	ConfigNodeValue	valueR, valueL;
	
	for(auto itrR = retR.first, itrL = retL.first; itrR != retR.second; ++itrR, ++itrL)
	{
		//выявление типа данных
		if((tagR = std::dynamic_pointer_cast<ConfigNodeTagData>(itrR->second)) != 0 && (tagL = std::dynamic_pointer_cast<ConfigNodeTagData>(itrL->second)) != 0)
			type = true;
		else if((valueR = std::dynamic_pointer_cast<ConfigNodeValueData>(itrR->second))!= 0 && (valueL = std::dynamic_pointer_cast<ConfigNodeValueData>(itrL->second)) != 0)
				type = false;
			 else
				return false;
				
		//сравнение нодов
		if(type)
		{
			if((tagR->name).compare(tagL->name) != 0 || (tagR->attribs).size() != (tagR->attribs).size())
				return false;
			
			auto attrIteratorR = (tagR->attribs).begin();
			auto attrIteratorL = (tagL->attribs).begin();
			
			for(; attrIteratorR != (tagR->attribs).end(); attrIteratorR++, attrIteratorL++)
			{
				if((attrIteratorR->first).compare(attrIteratorL->first) != 0)
					return false;
				
				if((attrIteratorR->second).compare(attrIteratorL->second) != 0)
					return false;
			}
		}
		else if((valueR->value).compare(valueL->value) != 0)
				return false;
		
		if(type)	
			comp = compareNodes(tagR, tagL);
	}
	
	return comp;
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
	static ConfigNode targetPointer(nullptr);
	if(stepPointer == top)
		targetPointer.reset();
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
	static ConfigNode		targetPointer(nullptr);
	if(stepPointer == top)
		targetPointer.reset();
	
    
    
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
	return findNode(path).use_count() != 0;
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

bool ConfigNodeBase::operator==(const ConfigNodeBase& ex)
{
    if((top->children)->size() != ((ex.top)->children)->size())
		return false;
			
	return compareNodes(top, (ex.top));
}
