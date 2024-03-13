#include "yaml-cpp/yaml.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "antlr4-runtime.h"
// #include "TLexer.h"
// #include "TParser.h"

// #include <Windows.h>

#pragma execution_character_set("utf-8")

using namespace antlr4;

// our data types
struct Vec3
{
    float x, y, z;
};
struct Power
{
    std::string name;
    int damage;
};

struct Monster
{
    std::string name;
    Vec3 position;
    std::vector<Power> powers;
};
namespace YAML
{
template<>
struct convert<Vec3>
{
    static Node encode(const Vec3& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node& node, Vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
        {
            return false;
        }

        rhs.x = node[0].as<double>();
        rhs.y = node[1].as<double>();
        rhs.z = node[2].as<double>();
        return true;
    }
};
template<>
struct convert<Power>
{
    static Node encode(const Power& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["damage"] = rhs.damage;
        return node;
    }

    static bool decode(const Node& node, Power& rhs)
    {
        rhs.name = node["name"].as<std::string>();
        rhs.damage = node["damage"].as<int>();
        return true;
    }
};
template<>
struct convert<Monster>
{
    static Node encode(const Monster& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        // node["position"] = node["position"].as<Vec3>();
        node["position"] = rhs.position;
        node["powers"] = rhs.powers;

        return node;
    }

    static bool decode(const Node& node, Monster& rhs)
    {
        rhs.name = node["name"].as<std::string>();
        rhs.position = node["position"].as<Vec3>();
        const YAML::Node& powers = node["powers"];
        for (const auto power : powers)
        {
            rhs.powers.push_back(power.as<Power>());
        }
        return true;
    }
};
} // namespace YAML

// // now the extraction operators for these types
// void operator>>(const YAML::Node& node, Vec3& v)
// {
//     node[0] >> v.x;
//     node[1] >> v.y;
//     node[2] >> v.z;
// }

// void operator>>(const YAML::Node& node, Power& power)
// {
//     node["name"] >> power.name;
//     node["damage"] >> power.damage;
// }

// void operator>>(const YAML::Node& node, Monster& monster)
// {
//     node["name"] >> monster.name;
//     node["position"] >> monster.position;
//     const YAML::Node& powers = node["powers"];
//     for (unsigned i = 0; i < powers.size(); i++)
//     {
//         Power power;
//         powers[i] >> power;
//         monster.powers.push_back(power);
//     }
// }

// int main(int argc, char** argv)
int main()
{
    // YAML::Node doc = YAML::LoadFile(argv[1]);
    std::string my_yaml = R"(- name: Ogre
  position: [0, 5, 0]
  powers:
    - name: Club
      damage: 10
    - name: Fist
      damage: 8
- name: Dragon
  position: [1, 0, 10]
  powers:
    - name: Fire Breath
      damage: 25
    - name: Claws
      damage: 15
- name: Wizard
  position: [5, -3, 0]
  powers:
    - name: Acid Rain
      damage: 50
    - name: Staff
      damage: 3)";
    YAML::Node doc = YAML::Load(my_yaml);
    std::vector<Monster> monsters;
    for (unsigned i = 0; i < doc.size(); i++)
    {
        monsters.push_back(doc[i].as<Monster>());
        const Monster& monster = monsters.back();
        std::cout << monster.name << "\n";
        std::cout << "Position: " << monster.position.x << ", " << monster.position.y << ", "
                  << monster.position.z << "\n";
        std::cout << "Powers: \n";
        for (const auto power : monster.powers)
        {
            std::cout << power.name << " " << power.damage << "\n";
        }
    }
    // YAML::Node doc_to_write = ;
    ANTLRInputStream input("a = b + \"c\";(((x * d))) * e + f; a + (x * (y ? 0 : 1) + z);");
    // TLexer lexer(&input);
    // CommonTokenStream tokens(&lexer);

    // TParser parser(&tokens);
    // tree::ParseTree* tree = parser.main();

    // auto s = tree->toStringTree(&parser);
    // std::cout << "Parse Tree: " << s << std::endl;
    return 0;
}