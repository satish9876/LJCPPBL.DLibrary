//
//  Core.hpp
//  ljWayfindingLib
//
//  Created by Srivishnu Pasumarthy on 11/19/18.
//  Copyright © 2018 Srivishnu Pasumarthy. All rights reserved.
//

#ifndef Core_h
#define Core_h

#include <stdio.h>
#include "../Models/Models.hpp"
#include <iostream>
#include <unordered_map>


const char *concatenateMyStringWithCppString(const char *myString);


const std::vector<LJCPPBL_Models::MenuAndDestinationItem>
performFuzzySearchNew(const std::vector<LJCPPBL_Models::MenuAndDestinationItem>,
                      const std::string pattern);

/* prepare temp data ( no need of all attributes ) to perform fuzzy search with */
const std::vector<LJCPPBL_Models::MenuAndDestinationItem>
prepareDestData(unordered_map<string, LJCPPBL_Models::Destination> data);

const std::string
performFuzzySearchTag(const std::string pattern, const std::string destinationName,
                      const std::string tagName);


#endif /* defined(__HelloCpp__Core__) */
