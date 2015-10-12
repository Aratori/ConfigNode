#include <gtest/gtest.h>
#include "ConfigNodeXml.cpp"
#include "ConfigNodeJson.cpp"
#include <iostream>
#include <string>


using namespace std;


ConfigNodeJson configJson;
ConfigNodeXml	configXml;
ConfigNode 		node;
ConfigNode* 	pNode;
ConfigNodeTag* 	pNodeTag;
ConfigNodeValue* pNodeValue;

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
	
	cout<<configJson.toString()<<endl;
	cout<<configXml.toString()<<endl;
	
	pNodeTag = dynamic_cast<ConfigNodeTag*>(configJson.findChildNode("city", "postalCode", "101101").parent);
		
	
	ASSERT_TRUE(str.compare((pNodeTag->name)) == 0);						
	
}

TEST(ConfigNodeTest, FindNodeTest)
{
	pNodeTag = dynamic_cast<ConfigNodeTag*>(configJson.findNode("address#postalCode:101101.city.Ленинград#libs:open").parent);
	cout<<pNodeTag->name<<endl;
		
}

TEST(ConfigNodeTest, HasNodeTest)
{
	ASSERT_TRUE(configJson.hasNode("firstName"));
}

TEST(ConfigNodeTest,  GetNodeTest)
{
	ASSERT_TRUE( configJson.getNode("address#postalCode:101101.city.Ленинград#libs:open").parent != NULL);
}

