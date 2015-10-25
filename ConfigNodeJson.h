#ifndef CONFIGNODEJSON_H
#define CONFIGNODEJSON_H
#include "jsoncpp/json/json.h"
#include "ConfigNodeBase.h"

class ConfigNodeJson : public ConfigNodeBase
{
	public:
		/**
		 * Reading XML or JSON file into our config structure.
		 * @param filename path to the file.
		 * @return true, if file was read successfully; false, otherwise.
		 */
		bool load(const std::string& filename);

	private:
		void    nextNode(const Json::Value &root, ConfigNode parentNode);
};

#endif
