/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE yamlcpp tests
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "yaml-cpp/yaml.h"

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
        for (const auto power: powers)
        {
            rhs.powers.push_back(power.as<Power>());
        }
        return true;
    }
};
} // namespace YAML

BOOST_AUTO_TEST_CASE(test_yaml_parser)
{
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
    }
    BOOST_CHECK(monsters.size() == 3);
    auto ogre = monsters[0];
    BOOST_CHECK(ogre.name == "Ogre");
    BOOST_CHECK(ogre.position.x == 0);
    BOOST_CHECK(ogre.position.y == 5);
    BOOST_CHECK(ogre.position.z == 0);
    auto ogre_powers = ogre.powers;
    BOOST_CHECK(ogre_powers.size() == 2);
    auto ogre_power_Club = ogre_powers[0];
    BOOST_CHECK(ogre_power_Club.name == "Club");
    BOOST_CHECK(ogre_power_Club.damage == 10);
}
