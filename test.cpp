#include "ConfigNodeBase.h"
#include "ConfigNodeXml.h"
#include "ConfigNodeJson.h"
#include <iostream>
#include <gtest/gtest.h>


using namespace std;

ConfigNodeJson configJson;
ConfigNodeXml	configXml;
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
	
	nodeTag = (ConfigNodeTag)configJson.findChildNode("city", "postalCode", "101101")->parent;
		
	
	ASSERT_TRUE(str.compare((nodeTag->name)) == 0);					
	
}

TEST(ConfigNodeTest, FindNodeTest)
{
	nodeTag = (ConfigNodeTag)configJson.findNode("address#postalCode:101101.city.Ленинград#libs:open")->parent;
	cout<<nodeTag->name<<endl;
		
}

TEST(ConfigNodeTest, HasNodeTest)
{
	cout<<"failed here"<<endl;
	ASSERT_TRUE(configJson.hasNode("firstName"));
}

TEST(ConfigNodeTest,  GetNodeTest)
{
	ASSERT_TRUE(configJson.getNode("address#postalCode:101101.city.Ленинград#libs:open")->parent != NULL);
}

TEST(ConfigNodeTest, GetAttrTest)
{
	cout<<configJson.getAttr("address", std::string("nope"))<<endl;
}
