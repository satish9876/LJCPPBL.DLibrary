//
//  StringUtil.h
//  ljWayfindingLib
//
//  Created by Srivishnu Pasumarthy on 11/19/18.
//  Copyright © 2018 Srivishnu Pasumarthy. All rights reserved.
//

#ifndef StringUtil_h
#define StringUtil_h

#include <string>
#include <vector>
#include <regex>
#include <iostream>

std::vector<std::string> getArrayOdStringByRegex(std::string original, std::string RegExStr)
{
    //RegExStr
    //std::regex ws_re("\\s+");
    std::regex ws_re(RegExStr);
    std::vector<std::string> result{
        std::sregex_token_iterator(original.begin(), original.end(), ws_re, -1), {}
    };
    return result;
}
#endif /* StringUtil_h */
