//
//  FuzzySearchAlgo_lev.h
//  ljWayfindingLib
//
//  Created by Srivishnu Pasumarthy on 11/19/18.
//  Copyright © 2018 Srivishnu Pasumarthy. All rights reserved.
//

#ifndef FuzzySearchAlgo_lev_h
#define FuzzySearchAlgo_lev_h

#include <string>
#include <vector>
#include <cstring>
#include "StringUtil.h"
#include <numeric> 


long levenshtein_distanceV1(const std::string s1, const std::string s2)
{
    const std::size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));
    
    d[0][0] = 0;
    for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
    for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;
    
    for(unsigned int i = 1; i <= len1; ++i)
        for(unsigned int j = 1; j <= len2; ++j)
            // note that std::min({arg1, arg2, arg3}) works only in C++11,
            // for C++98 use std::min(std::min(arg1, arg2), arg3)
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
    return d[len1][len2];
}

long levenshtein_distanceV2(const std::string s1, const std::string s2)
{
    // To change the type this function manipulates and returns, change
    // the return type and the types of the two variables below.
    long s1len = s1.size();
    long s2len = s2.size();
    
    auto column_start = (decltype(s1len))1;
    
    auto column = new decltype(s1len)[s1len + 1];
    std::iota(column + column_start - 1, column + s1len + 1, column_start - 1);
    
    for (auto x = column_start; x <= s2len; x++) {
        column[0] = x;
        auto last_diagonal = x - column_start;
        for (auto y = column_start; y <= s1len; y++) {
            auto old_diagonal = column[y];
            auto possibilities = {
                column[y] + 1,
                column[y - 1] + 1,
                last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
            };
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
        }
    }
    auto result = column[s1len];
    delete[] column;
    return result;
}


long levenshteinDistanceByPhrase(const std::string source, const std::string target) {
    
    return levenshtein_distanceV1(source , target);
}

long levenshteinDistanceByWords(const std::string source, const std::string target) {
    
    std::string RegEx= "\\s+";
    long WordsTotal =0,ValueWords =0,WordBest =0;
    std::vector<std::string> WordsS1 = getArrayOdStringByRegex(source, RegEx);
    std::vector<std::string> WordsS2 = getArrayOdStringByRegex(target, RegEx);
    for(std::string strS : WordsS1){
        long wordBest = source.size();
        for(std::string strT : WordsS1){
           long thisD =  levenshtein_distanceV1(strS , strT);
            if(thisD < WordBest) WordBest = thisD;
            if(thisD == 0) {
                WordsTotal = WordsTotal + wordBest;
                ValueWords = WordsTotal;
                break;
            }
        }
    }
    return ValueWords;
}




#endif /* FuzzySearchAlgo_lev_h */
