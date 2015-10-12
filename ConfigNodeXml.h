/*Comments*/
#ifndef CONFIG_H_
#define CONFIG_H_
#include <sstream>
#include <stdint.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <tinyxml.h>
#include "ConfigNodeBase.cpp"
#include <memory>
#include <map>
#include <vector>
/* Place to put all of my definitions etc. */
#endif

class ConfigNodeXml : public ConfigNodeBase
{
	public:
		ConfigNodeXml(): ConfigNodeBase(){};
		/**
		 * Reading XML or JSON file into our config structure.
		 * @param filename path to the file.
		 * @return true, if file was read successfully; false, otherwise.
		 */
		bool load(const std::string& filename);

	private:
		void    nextNode(TiXmlElement *parentElem, ConfigNode *parentNode);
		//ConfigNode*	top;
};
