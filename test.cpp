#include "ConfigNodeBase.h"
#include "ConfigNodeXml.h"
#include "ConfigNodeJson.h"
#include <iostream>
#include <gtest/gtest.h>


using namespace std;

ConfigNodeJson configJson;
ConfigNodeXml	configXml;
ConfigNodeJson eqJson;
ConfigNode 		node;
ConfigNodeTag 	nodeTag;
ConfigNodeValue nodeValue;

class ConfigNodeTest : public ::testing::Test {
	protected:
	virtual void SetUp()
	{}
	
	virtual void TearDown()
	{}
};

TEST(ConfigNodeTest, DefaultConstructorTest)
{
	configJson.load("jsonExample.json");	
	configXml.load("example.xml");
	
	std::string str = "address";
	
	cout << configJson.toString() << endl;
	cout << configXml.toString() << endl;
	
	nodeTag = dynamic_pointer_cast<ConfigNodeTagData>(configJson.findChildNode("city", "postalCode", "101101")->parent);
		
	
	ASSERT_TRUE(str.compare((nodeTag->name)) == 0);					
	
}

TEST(ConfigNodeTest, FindChildNodeTest)
{
	nodeTag = std::dynamic_pointer_cast<ConfigNodeTagData>(configJson.findChildNode("city", "postalCode", "101101"));
	ASSERT_TRUE((nodeTag->name).compare("city") == 0);
}

TEST(ConfigNodeTest, NegativeFindChildNodeTest)
{
	ASSERT_TRUE(configJson.findChildNode("nocity").use_count() == 0);
}

TEST(ConfigNodeTest, FindNodeTest)
{
	nodeTag = std::dynamic_pointer_cast<ConfigNodeTagData>(configJson.findNode("address#postalCode:101101.city"));
	ASSERT_TRUE((nodeTag->name).compare("city") == 0);
}

TEST(ConfigNodeTest, NegativeFindNodeTest)
{
	ASSERT_TRUE(configJson.findNode("nocity").use_count() == 0);
}

TEST(ConfigNodeTest, HasNodeTest)
{
	ASSERT_TRUE(configJson.hasNode("firstName"));
}

TEST(ConfigNodeTest, NegativeHasNodeTest)
{
	ASSERT_FALSE(configJson.hasNode("notsecondName"));
}

TEST(ConfigNodeTest,  GetNodeTest)
{
	ASSERT_TRUE(configJson.getNode("address#postalCode:101101.city.Ленинград#libs:open").use_count() != 0);
}

TEST(ConfigNodeTest, NegativeGetNodeTest)
{
	try{
		configJson.getNode("not node");
	}
	catch(std::runtime_error ex)
	{
		ASSERT_TRUE(strcmp("Node not found",ex.what()) == 0);
	}
}

TEST(ConfigNodeTest, GetAttrTest)
{
	cout<<configJson.getAttr("address", std::string("nope"))<<endl;
}


TEST(ConfigNodeTest, NegativeGetAttrTest)
{
	eqJson.load("eqJson.json");
	ASSERT_TRUE(eqJson.getAttr("address", std::string("nope")).compare("nope") == 0);
}

TEST(ConfigNodeTest, ConfigCompare)
{
	cout<<eqJson.toString()<<endl;
	cout<<configXml.toString()<<endl;
	ASSERT_TRUE(eqJson == configXml);	
}

TEST(ConfigNodeTest, ConfigJsonCompare)
{
	ConfigNodeJson eqConfigJsonOne;
	ConfigNodeJson eqConfigJsonTwo;
	eqConfigJsonOne.load("eqJsonExample.json");
	eqConfigJsonTwo.load("jsonExample.json");
	cout<<eqConfigJsonOne.toString()<<endl;
	cout<<eqConfigJsonTwo.toString()<<endl;	
	ASSERT_TRUE(eqConfigJsonOne == eqConfigJsonTwo);
}

TEST(ConfigNodeTest, ConfigXmlCompare)
{
	ConfigNodeXml eqConfigXmlOne;
	ConfigNodeXml eqConfigXmlTwo;
	eqConfigXmlOne.load("example.xml");
	eqConfigXmlTwo.load("example.xml");
	cout<<eqConfigXmlOne.toString()<<endl;
	cout<<eqConfigXmlTwo.toString()<<endl;	
	ASSERT_TRUE(eqConfigXmlOne == eqConfigXmlTwo);
}

TEST(ConfigNodeTest, NegativeConfigCompare)
{
	ConfigNodeJson eqConfigJsonOne;
	ConfigNodeXml eqConfigXmlTwo;
	eqConfigJsonOne.load("jsonExample.json");
	eqConfigXmlTwo.load("example.xml");
	cout<<eqConfigJsonOne.toString()<<endl;
	cout<<eqConfigXmlTwo.toString()<<endl;	
	ASSERT_FALSE(eqConfigJsonOne == eqConfigXmlTwo);
}
