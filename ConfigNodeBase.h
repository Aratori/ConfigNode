#ifndef CONFIGBASE_H_
#define CONFIGBASE_H_
#include <sstream> 
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <memory>
#include <map>
#include <vector>

struct	ConfigNodeData;

typedef	ConfigNodeData* ConfigNode;
typedef std::shared_ptr<std::multimap<ConfigNode,ConfigNode> > ConfigNodeMap;
typedef std::pair<std::string, std::string>	strPair;
typedef	std::pair<ConfigNode, ConfigNode> nodePair;
typedef	std::pair <std::multimap<ConfigNode,ConfigNode>::iterator, std::multimap<ConfigNode,ConfigNode>::iterator> itrPair;


struct ConfigNodeData
{
	ConfigNode parent;
	ConfigNodeMap children;
	virtual ~ConfigNodeData(){};
};

struct ConfigNodeTagData : ConfigNodeData
{
	std::string name;
	std::map<std::string, std::string> attribs;
};
struct	ConfigNodeValueData : ConfigNodeData
{
	std::string	value;
};

typedef ConfigNodeTagData* ConfigNodeTag;
typedef ConfigNodeValueData* ConfigNodeValue;


class ConfigNodeBase
{
	public:
		ConfigNodeBase();
		~ConfigNodeBase();
		/**
		 * Reading XML or JSON file into our config structure.
		 * @param filename path to the file.
		 * @return true, if file was read successfully; false, otherwise.
		 */
		virtual	bool load(const std::string& filename)=0;
		/**
		 * Recursively prints data to sstream.
		 * @param target output stream.
		 * @param indent current indentation level.
		 */
		void dump(std::stringstream& target, uint32_t indent = 0) const;

		/**
		 * Converts structure to human-readable form.
		 * @return string representing the structure.
		 */
		std::string toString() const;

		/**
		 * Returns (a reference to) the map of children nodes.
		 * @return shared_ptr to std::multimap with nodes.
		 */
		ConfigNodeMap getChildren();

		/**		 * Looks for the node with request parameters in the node's children list.
		 * If there are several nodes with matching params, the 1st one is returned.
		 *
		 * @param node node name.
		 * @param attr attribute name (not checked if empty).
		 * @param value attribute value (not checked if empty or attr is empty).
		 * @return ConfigNode of node with matching params, or ConfigNode(nullptr).
		 */
		ConfigNode findChildNode(const std::string& node,
				const std::string& attr = "", const std::string& value = "") const;
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
		ConfigNode findNode(const std::string& path) const;

		/**
		 * Checks whether node matching given path exists or not.
		 * Searches nodes hierarchy if complex path is supplied.
		 * @param path node name path.
		 * @return true, if attribute exists; false, otherwise.
		 */
		bool hasNode(const std::string& path) const;

		/**
		 * Node object getter.
		 * Searches nodes hierarchy if complex path is supplied.
		 * Throws exception if node does not exist.
		 * @param path node name path.
		 * @return ConfigNode holding matching object.
		 */
		ConfigNode getNode(const std::string& path) const;

		/**
		 * Return attribute value or default value (if attribute does not exist).
		 * Searches nodes hierarchy if complex path is supplied.
		 * @param path node/attribute search path.
		 * @param def default value.
		 * @return attribute value or default value.
		 * "parent.child#attr1:val.superchild"
		*/
		template <typename Y>
		Y getAttr(const std::string& path, Y def) const
		{
			static ConfigNode	stepPointer		=	top;
			static unsigned int 	deep_counter	=	0;
			Y				targetAttr = def;
			
			std::string tagName;
			std::vector<strPair> attributes;
			bool end	=	parsePath(path, tagName, attributes, deep_counter);
			
			itrPair ret;
			ret = (stepPointer->children)->equal_range(stepPointer);																//get node children
			
			for(std::multimap<ConfigNode, ConfigNode>::iterator it = ret.first; it != ret.second; ++it)
			{	
				if(compareNodes(it->second, tagName, attributes))
					if(end)
					{
						ConfigNodeTag	tagPointer	=	dynamic_cast<ConfigNodeTag>(it->second);
						if(tagPointer->attribs.size())
						{
							Y attr	=	(Y)((*tagPointer->attribs.begin()).second);
							return attr;
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
		Y getAttr(const std::string& path) const
		{
			Y attr 	=	getAttr<Y>(path, 0);
			if(attr != 0)
				return NULL;
			else
				throw	std::runtime_error("Attribute not found");
		}	
	protected:
		bool	parsePath(const std::string& path, std::string& tagName, std::vector<strPair>& attributes, int deep) const;
		ConfigNode	setNode(ConfigNode parent, std::string val, bool tog);//set tag/value config nodes
		bool	compareNodes(ConfigNode node,const std::string& name, const std::vector<strPair>& attr) const;
		
		ConfigNode	top;
};
#endif /*CONFIGBASE_H_*/

