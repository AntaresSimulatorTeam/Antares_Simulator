/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_CONSTRAINTSBUILDER_BUILDER_CBUILDER_H__
#define __ANTARES_CONSTRAINTSBUILDER_BUILDER_CBUILDER_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <antares/study.h>
#include "../../libs/antares/study/area/constants.h"
#include "grid.h"

#define CB_PREFIX "@UTO_"

namespace Antares
{
class areaInfo
{
public:
    areaInfo(Data::Area* area)
    {
        ptr = area;
        nodeName = area->name.to<std::string>();
    }
    std::string getName()
    {
        return nodeName;
    }

public:
    Data::Area* ptr;
    std::string nodeName;
};

class linkInfo
{
public:
    double weight;
    bool enabled = true;
    Data::AreaLinkName name;
    Antares::Data::AssetType type;
    double angleLB = 0;
    double angleUB = 0;
    bool hasPShiftsEqual = true;

    uint nImpedanceChanges;
    double avgImpedance;
    Data::AreaLink* ptr;

    Matrix<double>* dataLink;

public:
    double getWeightWithImpedance() const;
    Yuni::String getName() const;

    linkInfo()
    {
        dataLink = new Matrix<>;
    }

    ~linkInfo()
    {
        delete dataLink;
    }

    struct comparepWeight
    {
        inline bool operator()(const linkInfo* lhs, const linkInfo* rhs) const
        {
            return lhs->getWeightWithImpedance() < rhs->getWeightWithImpedance();
        }
    };
    struct comparepWeightWithImpedance
    {
        inline bool operator()(const linkInfo* lhs, const linkInfo* rhs) const
        {
            return lhs->getWeightWithImpedance() < rhs->getWeightWithImpedance();
        }
    };
    struct addpWeight
    {
        double operator()(double i, const linkInfo* o) const
        {
            return (o->getWeightWithImpedance() + i);
        }
    };
    struct addpWeightWithImpedance
    {
        double operator()(double i, const linkInfo* o) const
        {
            return (o->getWeightWithImpedance() + i);
        }
    };
    bool operator<(const linkInfo& other) const
    {
        return getWeightWithImpedance() < other.getWeightWithImpedance()
                 ? true
                 : (getWeightWithImpedance() > other.getWeightWithImpedance()
                      ? false
                      : getName() < other.getName());
    }
}; // class linkInfo

class State
{
public:
    State(std::vector<double> impedancesList, uint time, double infinite = 1000000) :
     secondMember(3, time), impedances(impedancesList)
    {
        secondMember.fillColumn(1, -1 * infinite);
        secondMember.fillColumn(0, infinite);
    };

    Matrix<double, double> secondMember;
    std::vector<double> impedances;
    std::map<linkInfo*, double> WeightMap;
};

class Cycle
{
public:
    Cycle(const std::vector<linkInfo*>& linkList, double infinite = 1000000) :
     time(0), loop(linkList), opType(Data::BindingConstraint::opEquality), pInfinite(infinite)
    {
        uint columnImpedance = (uint)Antares::Data::fhlImpedances;

        std::vector<double> impedances;
        Data::AreaLink* previousLine;
        double currentLineSign;
        for (auto line = linkList.begin(); line != linkList.end(); line++)
        {
            if ((*line)->nImpedanceChanges > 0
                || ((*line)->type == Antares::Data::atAC && (!(*line)->hasPShiftsEqual)))
                opType = Data::BindingConstraint::opBoth;
            impedances.push_back((*line)->dataLink->entry[columnImpedance][0]);

            time = (*line)->dataLink->height; /*BC loading always expects 8786 values heigth will
                                                 have to be resized*/

            if (line == linkList.begin())
            {
                currentLineSign = 1;
            }
            else
            {
                if (previousLine->with == (*line)->ptr->from
                    || previousLine->from
                         == (*line)->ptr->with) //[(A/B),(B/C),... ] or [(A,B),(C/A),... ]
                {
                }
                else if (previousLine->from == (*line)->ptr->from
                         || previousLine->with == (*line)->ptr->with)
                {
                    currentLineSign *= -1;
                }
                else
                    assert(0 and "links of the loops do not connect or are not in the right order");
            }

            sign.push_back(currentLineSign);
            previousLine = (*line)->ptr;
        }
        State st(impedances, time, pInfinite);
        states.push_back(st);
    }

    State& getState(std::vector<double>& impedances)
    {
        std::vector<State>::iterator stIT
          = std::find_if(states.begin(), states.end(), [&impedances](State& s) -> bool {
                return s.impedances == impedances;
            });

        if (stIT == states.end())
        {
            State state(impedances, time, pInfinite);
            states.push_back(state);
            stIT = std::find_if(states.begin(), states.end(), [&impedances](State& s) -> bool {
                return s.impedances == impedances;
            });
        }
        return *stIT;
    }

    uint time;
    std::vector<double> sign;
    const std::vector<linkInfo*>& loop;
    std::vector<State> states;
    Data::BindingConstraint::Operator opType;
    double pInfinite;
};

class CBuilder final
{
public:
    typedef std::vector<linkInfo*> Vector;
    typedef std::map<linkInfo*, double> WeightMap;
    typedef std::map<linkInfo*, double> Pattern;
    typedef std::vector<Pattern> VectorOfPatterns;
    typedef std::map<uint, Pattern*> Map;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    CBuilder(Antares::Data::Study::Ptr);
    //! Destructor
    ~CBuilder();
    //@}

    /*!
    ** \brief Independent function to run the constraint generator from a study
    */
    bool runConstraintsBuilder(bool standalone = false);

    /*!
    ** \brief Complete the settings with the data from the study
    */
    bool completeFromStudy();

    /*!
    ** \brief Delete the network constraints
    */
    bool deletePreviousConstraints();

    bool saveCBuilderToFile(const String& filename = "") const;
    bool completeCBuilderFromFile(const String& filename = "");

    /*!
    ** \brief check if network constraints already exists in the study
    */
    int alreadyExistingNetworkConstraints(const Yuni::String& prefix) const;

    //! find an edge from node names
    linkInfo* findLinkInfoFromNodeNames(Data::AreaName& u, Data::AreaName& v)
    {
        auto linkIT
          = std::find_if(pLink.begin(), pLink.end(), [&u, &v](const linkInfo* edgeP) -> bool {
                if (edgeP->ptr->from->id == u && edgeP->ptr->with->id == v)
                    return true;
                if (edgeP->ptr->from->id == v && edgeP->ptr->with->id == u)
                    return true;
                else
                    return false;
            });
        if (linkIT != pLink.end())
            return *linkIT;

        return nullptr;
    }

    //! build list of edges from area
    void buildAreaToLinkInfosMap()
    {
        areaToLinks.clear();
        auto& study = *Data::Study::Current::Get();
        for (auto& area : study.areas)
        {
            auto a = area.second;
            std::for_each(pLink.begin(), pLink.end(), [&a, this](linkInfo* edgeP) {
                if (edgeP->ptr->from == a || edgeP->ptr->with == a)
                    this->areaToLinks[a].insert(edgeP);
            });
        }
    }

    linkInfo* getLink(uint i)
    {
        if (i < pLink.size())
            return pLink[i];
        return nullptr;
    }

    size_t linkCount()
    {
        return pLink.size();
    }

    bool isCycleDriver(linkInfo*);

    uint cycleCount(linkInfo* lnkI);

    /*!
    ** \brief create all the network constraints based on a mesh
    */
    bool createConstraints(const std::vector<Vector>&);

    double setInfinite(const long value)
    {
        return infiniteSecondMember = value;
    }

    bool setCheckNodalLoopFlow(const bool value)
    {
        return checkNodalLoopFlow = value;
    }

    bool setLoopFlowInclusion(const bool value)
    {
        return includeLoopFlow = value;
    }

    bool setPhaseShiftInclusion(const bool value)
    {
        return includePhaseShift = value;
    }

    double getInfinite()
    {
        return infiniteSecondMember;
    }

    bool getCheckNodalLoopFlow()
    {
        return checkNodalLoopFlow;
    }

    bool getLoopFlowInclusion()
    {
        return includeLoopFlow;
    }

    bool getPhaseShiftInclusion()
    {
        return includePhaseShift;
    }

    bool setUpToDate(const bool value)
    {
        return isUpToDate = value;
    }

    bool getUpToDate()
    {
        return isUpToDate;
    }

    bool update(bool applyCheckBox = false);

    void setCalendarStart(int start)
    {
        calendarStart = start;
    }

    void setCalendarEnd(int end)
    {
        calendarEnd = end;
    }
    uint getCalendarStart()
    {
        return calendarStart;
    }

    uint getCalendarEnd()
    {
        return calendarEnd;
    }

private:
    /*!
    ** \brief add one constraint to the study
    */
    Antares::Data::BindingConstraint* addConstraint(const Data::ConstraintName& name,
                                                    const Yuni::String& op,
                                                    const Yuni::String& type,
                                                    const WeightMap& weights,
                                                    const double& secondMember);

public:
    Vector pLink;

private:
    YString pStudyFolder;
    YString pPrefix;
    YString pPrefixDelete;
    bool pDelete;
    bool includeLoopFlow = false;
    bool includePhaseShift = false;
    bool isUpToDate = false;
    bool checkNodalLoopFlow = true;
    double infiniteSecondMember = 1000000;

    uint calendarStart = 1;
    uint calendarEnd = 8760;

    std::vector<std::vector<linkInfo*>> pMesh;

    std::map<Data::Area*, std::set<linkInfo*>> areaToLinks;

    Antares::Data::Study::Ptr pStudy;
    uint NLinks;

    Graph::Grid<Antares::Data::Area> _grid;

}; // class cbuilder

} // namespace Antares

#endif // __ANTARES_CONSTRAINTSBUILDER_BUILDER_CBUILDER_H__
