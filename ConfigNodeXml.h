#ifndef CONFIGNODEXML_H
#define CONFIGNODEXML_H
#include <tinyxml.h>
#include "ConfigNodeBase.h"

class ConfigNodeXml : public ConfigNodeBase
{
	public: 	 
		/**
		 * Reading XML or JSON file into our config structure.
		 * @param filename path to the file.
		 * @return true, if file was read successfully; false, otherwise.
		 */
		bool load(const std::string& filename);
	private:
		void    nextNode(TiXmlElement *parentElem, ConfigNode parentNode);
} ;

#endif
