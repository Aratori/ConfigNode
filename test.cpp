#include "ConfigNodeBase.h"
#include "ConfigNodeXml.h"
#include "ConfigNodeJson.h"
#include <iostream>
#include <gtest/gtest.h>


using namespace std;



class ConfigNodeTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {}

    virtual void TearDown()
    {}
};

TEST(ConfigNodeTest, DefaultConstructorTest)
{
    ConfigNodeJson configJson;
    ConfigNodeTag 	nodeTag;
    ConfigNodeXml	configXml;
    std::string str = "address";

    configJson.load("jsonExample.json");
    configXml.load("example.xml");

    cout << configJson.toString() << endl;
    cout << configXml.toString() << endl;

    nodeTag = dynamic_pointer_cast<ConfigNodeTagData>(configJson.findChildNode("city", "postalCode", "101101")->parent);

    ASSERT_TRUE(str.compare((nodeTag->name)) == 0);

}

TEST(ConfigNodeTest, FindChildNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    ConfigNodeTag 	nodeTag;
    nodeTag = std::dynamic_pointer_cast<ConfigNodeTagData>(configJson.findChildNode("city", "postalCode", "101101"));
    ASSERT_TRUE((nodeTag->name).compare("city") == 0);
}

TEST(ConfigNodeTest, NegativeFindChildNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    ASSERT_TRUE((bool)configJson.findChildNode("nocity"));
}

TEST(ConfigNodeTest, FindNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    ConfigNodeTag 	nodeTag;
    nodeTag = std::dynamic_pointer_cast<ConfigNodeTagData>(configJson.findNode("address#postalCode:101101.city"));
    ASSERT_TRUE((nodeTag->name).compare("city") == 0);
}

TEST(ConfigNodeTest, NegativeFindNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    ASSERT_FALSE((bool)configJson.findNode("nocity"));
}

TEST(ConfigNodeTest, HasNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    ASSERT_TRUE(configJson.hasNode("address#postalCode:101101.city"));
}

TEST(ConfigNodeTest, NegativeHasNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    ASSERT_FALSE(configJson.hasNode("address.nocity"));
}

TEST(ConfigNodeTest,  GetNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    ASSERT_TRUE(configJson.getNode("address#postalCode:101101.city.Ленинград#libs:open").use_count() != 0);
}

TEST(ConfigNodeTest, NegativeGetNodeTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    try
    {
        configJson.getNode("not node");
    }
    catch(std::runtime_error ex)
    {
        ASSERT_TRUE(strcmp("Node not found",ex.what()) == 0);
    }
}

TEST(ConfigNodeTest, GetAttrTest)
{
    ConfigNodeJson configJson;
    configJson.load("jsonExample.json");
    configJson.getAttr("notaddress", std::string("nope"));
}


TEST(ConfigNodeTest, NegativeGetAttrTest)
{
    ConfigNodeJson eqJson;
    eqJson.load("eqJson.json");
    ASSERT_TRUE(eqJson.getAttr("address", std::string("nope")).compare("nope") == 0);
}

TEST(ConfigNodeTest, XMLJsonConfigCompare)
{
    ConfigNodeXml	configXml;
    ConfigNodeJson	configJson;
    configJson.load("jsonExample.json");
    configXml.load("example.xml");
    cout<<configJson.toString()<<endl;
    cout<<configXml.toString()<<endl;
    ASSERT_TRUE(configJson == configXml);
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
    eqConfigXmlTwo.load("negativeExample.xml");
    cout<<eqConfigJsonOne.toString()<<endl;
    cout<<eqConfigXmlTwo.toString()<<endl;
    ASSERT_FALSE(eqConfigJsonOne == eqConfigXmlTwo);
}

