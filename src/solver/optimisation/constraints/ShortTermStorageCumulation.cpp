/*
* Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include "antares/solver/optimisation/constraints/ShortTermStorageCumulation.h"

void ShortTermStorageCumulation::Injection(unsigned int index,
const ::ShortTermStorage::PROPERTIES& input
                                                       ){
    builder.ShortTermStorageInjection(index, 1.0);
                                                       }
void ShortTermStorageCumulation::Withdrawal(unsigned int index,
const ::ShortTermStorage::PROPERTIES& input
                                                  ){
    builder.ShortTermStorageWithdrawal(index, 1.0);}
void ShortTermStorageCumulation::Netting(unsigned int index,
                                                      const ::ShortTermStorage::PROPERTIES& input){


    builder.ShortTermStorageInjection(index, input.injectionEfficiency ).
    ShortTermStorageWithdrawal(index, -input.withdrawalEfficiency);
                                                       }
auto getMemberFunction = [](const std::string& name)
-> void (ShortTermStorageCumulation::*)(unsigned int,
                                              const ::ShortTermStorage::PROPERTIES& ) {
            if (name == "withdrawal") {
                return &ShortTermStorageCumulation::Withdrawal;
            } else if (name == "injection") {
                return &ShortTermStorageCumulation::Injection;
            }
            else if (name == "netting"){
                return &ShortTermStorageCumulation::Netting;
        }
            return nullptr; // Return null if no match
        };


char ConvertSign(const std::string& sign){
    if (sign == "greater") {
        return '>';
    }
   else if (sign == "less") {
       return '<';
   }
   else {
       return '=';
   }
}

void ShortTermStorageCumulation::add(int pays){

    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);

    for (const auto& storage: data.ShortTermStorage[pays])
    {
        // TODO  global index for constraints for data.CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation[index]
        for(const auto& constraint: storage.additional_constraints){

            //sum (var[h]) sign rhs, h in list provied by user

            namer.ShortTermStorageLevel(builder.data.nombreDeContraintes, storage.name);
            const auto index = storage.clusterGlobalIndex;
            //TODO
            data.CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation[index]
              = builder.data.nombreDeContraintes;
            auto memberFunction = getMemberFunction(constraint.variable);
            for (const auto& hour: constraint.hours){
                const int hourInTheYear = builder.data.weekInTheYear * 168 + hour-1;
        builder.updateHourWithinWeek(hour-1);
         (this->*memberFunction)(index, storage);
          builder.SetOperator(ConvertSign(constraint.operatorType))
          .build();
            }
        }
    }
}
