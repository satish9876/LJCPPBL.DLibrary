//
//  Core.cpp
//  ljWayfindingLib
//
//  Created by Srivishnu Pasumarthy on 11/19/18.
//  Copyright © 2018 Srivishnu Pasumarthy. All rights reserved.
//

#include "FuzzySearch.h"
#include "../Utilities/FuzzySearchAlgo_lev.h"
#include "../Models/Models.hpp"
#include <algorithm>
#include <limits>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <sstream>
#include <fstream>

#include <android/log.h>


const char *CPP_BASE_STRING = "cpp says hello to %s";

struct CustomAlphaCmp {
    int table[1 << CHAR_BIT];

    CustomAlphaCmp(const std::string &alpha) {
        std::fill(std::begin(table), std::end(table), INT_MAX);
        int value = 0;
        for (auto x : alpha)
            table[static_cast<unsigned char>(x)] = ++value;
    }

    bool operator()(const std::string &a, const std::string &b) {
        auto lhs = a.begin();
        auto rhs = b.begin();

        for (; lhs != a.end() && rhs != b.end(); ++lhs, ++rhs) {
            int lhs_val = table[static_cast<unsigned char>(*lhs)];
            int rhs_val = table[static_cast<unsigned char>(*rhs)];

            if (lhs_val != rhs_val)
                return lhs_val < rhs_val;
        }

        return (rhs != b.end());
    }
};


std::vector<std::string> getMatchingSubstrings(const std::vector<std::string> &subStrings,
                                               const std::vector<std::string> &fullStrings) {
    std::vector<std::string> unmatchedSubStrings = subStrings;
    std::vector<std::string> matchedSubStrings;

    for (auto fullString : fullStrings) {

        for (auto subString : subStrings) {
            std::transform(subString.begin(), subString.end(), subString.begin(), ::tolower);
            std::transform(fullString.begin(), fullString.end(), fullString.begin(), ::tolower);
            if (fullString.find(subString) != std::string::npos) {

                bool isChk = false;

                if (matchedSubStrings.size() > 0) {
                    for (int k = 0; k < matchedSubStrings.size(); k++) {

                        if (matchedSubStrings[k].compare(fullString) == 0) {
                            isChk = true;
                            break;
                        }

                    }
                }
                if (isChk == false) {
                    matchedSubStrings.push_back(fullString);
                }

            }
        }

    }

    return matchedSubStrings;
}

std::vector<LJCPPBL_Models::MenuAndDestinationItem>
getMatchingSubstringsNew(const std::vector<std::string> &subStrings,
                         const std::vector<LJCPPBL_Models::MenuAndDestinationItem> &fullStrings) {
    std::vector<std::string> unmatchedSubStrings = subStrings;
    std::vector<LJCPPBL_Models::MenuAndDestinationItem> matchedSubStrings;

    for (auto fullString : fullStrings) {// eadia // e, ea, ead, eadi, eadia > a, ad, adi

        for (auto subString : subStrings) {
            std::transform(subString.begin(), subString.end(), subString.begin(), ::tolower);
            std::transform(fullString.destinationName.begin(), fullString.destinationName.end(),
                           fullString.destinationName.begin(), ::tolower);
            if (fullString.destinationName.find(subString) != std::string::npos) {

/*                bool isChk = false;

                if (matchedSubStrings.size() > 0) {
                    for (int k = 0; k < matchedSubStrings.size(); k++) {

                        if (matchedSubStrings[k].destinationName.compare(
                                fullString.destinationName) == 0) {
                            isChk = true;
                            break;
                        }

                    }
                }
                if (isChk == false) {
                    matchedSubStrings.push_back(fullString);
                }*/
                matchedSubStrings.push_back(fullString);
                break;
            }

        }

    }

    return matchedSubStrings;
}


const char *concatenateMyStringWithCppString(const char *myString) {
    char *concatenatedString = new char[strlen(CPP_BASE_STRING) + strlen(myString)];
    sprintf(concatenatedString, CPP_BASE_STRING, myString);
    return concatenatedString;
}



/* fuzzy search with vector*/
const int SearchString(string text, string pattern, int k) {
    int result = -1;
    int m = pattern.size();
    unsigned long *R;
    unsigned long patternMask[CHAR_MAX + 1];
    int i, d;

    if (pattern[0] == '\0') return 0;
    if (m > 31) return -1; //Error: The pattern is too long!

    R = new unsigned long[(k + 1) * sizeof *R];
    for (i = 0; i <= k; ++i)
        R[i] = ~1;

    for (i = 0; i <= CHAR_MAX; ++i)
        patternMask[i] = ~0;

    for (i = 0; i < m; ++i)
        patternMask[pattern[i]] &= ~(1UL << i);

    for (i = 0; text[i] != '\0'; ++i) {
        unsigned long oldRd1 = R[0];

        R[0] |= patternMask[text[i]];
        R[0] <<= 1;

        for (d = 1; d <= k; ++d) {
            unsigned long tmp = R[d];

            R[d] = (oldRd1 & (R[d] | patternMask[text[i]])) << 1;
            oldRd1 = tmp;
        }

        if (0 == (R[k] & (1UL << m))) {
            result = (i - m) + 1;
            break;
        }
    }

    free(R);
    return result;
}

// Private implementation
static bool fuzzy_match_recursive(const char *pattern, const char *str, int &outScore,
                                  const char *strBegin, uint8_t const *srcMatches, uint8_t *matches,
                                  int maxMatches,
                                  int nextMatch, int &recursionCount, int recursionLimit,
                                  bool isFromTag) {
    // Count recursions
    ++recursionCount;
    if (recursionCount >= recursionLimit)
        return false;

    // Detect end of strings
    if (*pattern == '\0' || *str == '\0')
        return false;

    // Recursion params
    bool recursiveMatch = false;
    uint8_t bestRecursiveMatches[256];
    int bestRecursiveScore = 0;

    // Loop through pattern and str looking for a match
    bool first_match = true;
    while (*pattern != '\0' && *str != '\0') {

        // Found match
        if (tolower(*pattern) == tolower(*str)) {

            // Supplied matches buffer was too short
            if (nextMatch >= maxMatches)
                return false;

            // "Copy-on-Write" srcMatches into matches
            if (first_match && srcMatches) {
                memcpy(matches, srcMatches, nextMatch);
                first_match = false;
            }

            // Recursive call that "skips" this match
            uint8_t recursiveMatches[256];
            int recursiveScore;
            if (fuzzy_match_recursive(pattern, str + 1, recursiveScore, strBegin, matches,
                                      recursiveMatches, sizeof(recursiveMatches), nextMatch,
                                      recursionCount, recursionLimit, isFromTag)) {

                // Pick best recursive score
                if (!recursiveMatch || recursiveScore > bestRecursiveScore) {
                    memcpy(bestRecursiveMatches, recursiveMatches, 256);
                    bestRecursiveScore = recursiveScore;
                }
                recursiveMatch = true;
            }

            // Advance
            matches[nextMatch++] = (uint8_t) (str - strBegin);
            ++pattern;
        }
        ++str;
    }

    // Determine if full pattern was matched
    bool matched = *pattern == '\0' ? true : false;

    // Calculate score
    if (matched) {
        const int sequential_bonus = 35;//30;//26; //30;//15;            // bonus for adjacent matches
        const int separator_bonus = 15; //30;             // bonus if match occurs after a separator
        const int camel_bonus = 15; //30;                 // bonus if match is uppercase and prev is lower
        const int first_letter_bonus = 22;//20;//15;          // bonus if the first letter is matched

        int leading_letter_penalty = 0;
        int max_leading_letter_penalty = 0;
        int unmatched_letter_penalty = 0;
        if (isFromTag) {
            leading_letter_penalty = -5;      // penalty applied for every letter in str before the first match
            max_leading_letter_penalty = -20;//-15; // maximum penalty for leading letters
            unmatched_letter_penalty = -1;    // penalty for every letter that doesn't matter
        }

        // Iterate str to end
        while (*str != '\0')
            ++str;

        // Initialize score
        outScore = 100;

        // Apply leading letter penalty
        int penalty = leading_letter_penalty * matches[0];
        if (penalty < max_leading_letter_penalty)
            penalty = max_leading_letter_penalty;
        outScore += penalty;

        // Apply unmatched penalty
        int unmatched = (int) (str - strBegin) - nextMatch;
        outScore += unmatched_letter_penalty * unmatched;

        // Apply ordering bonuses
        for (int i = 0; i < nextMatch; ++i) {
            uint8_t currIdx = matches[i];

            if (i > 0) {
                uint8_t prevIdx = matches[i - 1];

                // Sequential
                if (currIdx == (prevIdx + 1))
                    outScore += sequential_bonus;
            }

            // Check for bonuses based on neighbor character value
            if (currIdx > 0) {
                // Camel case
                char neighbor = strBegin[currIdx - 1];
                char curr = strBegin[currIdx];
                if (::islower(neighbor) && ::isupper(curr))
                    outScore += camel_bonus;

                // Separator
                bool neighborSeparator = neighbor == '_' || neighbor == ' ';
                if (neighborSeparator)
                    outScore += separator_bonus;
            } else {
                // First letter
                outScore += first_letter_bonus;
            }
        }
    }

    // Return best result
    if (recursiveMatch && (!matched || bestRecursiveScore > outScore)) {
        // Recursive score is better than "this"
        memcpy(matches, bestRecursiveMatches, maxMatches);
        outScore = bestRecursiveScore;
        return true;
    } else if (matched) {
        // "this" score is better than recursive
        return true;
    } else {
        // no match
        return false;
    }
}


static bool
fuzzy_match(char const *pattern, char const *str, int &outScore, uint8_t *matches, int maxMatches,
            bool isFromTag) {
    int recursionCount = 0;
    int recursionLimit = 8;

    return fuzzy_match_recursive(pattern, str, outScore, str, nullptr, matches, maxMatches, 0,
                                 recursionCount, recursionLimit, isFromTag);
}

static bool fuzzy_match_test(char const *pattern, char const *str, int &outScore, bool isFromTag) {

    uint8_t matches[256];
    return fuzzy_match(pattern, str, outScore, matches, sizeof(matches), isFromTag);
}

/* perform fuzzy search on destination name and tags*/
const std::vector<LJCPPBL_Models::MenuAndDestinationItem>
performFuzzySearchNew(const std::vector<LJCPPBL_Models::MenuAndDestinationItem> dictionary,
                      const std::string pattern) {

    std::vector<std::pair<int, LJCPPBL_Models::MenuAndDestinationItem const *>> matches;
    std::vector<std::string> returnDataPattern;

    // std::vector<std::string> returnData;
    std::vector<LJCPPBL_Models::MenuAndDestinationItem> returnData;


    for (auto &&entry : dictionary) {

        std::string patternTemp = pattern;
        //string tempTags = entry.destinationName;

        int score;
        bool isDataMatched = fuzzy_match_test(pattern.c_str(), entry.destinationName.c_str(),
                                              score, false);
        if (isDataMatched && score > 49) { // 49
            matches.emplace_back(score + 10, &entry);
        }/* else if (patternTemp.size() > 3 && isDataMatched == false) {
            patternTemp.erase(patternTemp.begin());

            int scoreTag;
            bool isDataMatched = fuzzy_match_test(patternTemp.c_str(),
                                                  entry.destinationName.c_str(),
                                                  scoreTag);

            if (isDataMatched == false) {
                patternTemp.erase(patternTemp.begin());
                isDataMatched = fuzzy_match_test(patternTemp.c_str(), entry.destinationName.c_str(),
                                                 scoreTag);
            }
            if (isDataMatched && scoreTag > 0) {
                matches.emplace_back(scoreTag, &entry);
            }
        }*/ else {

            string tempTags = entry.tags;
            bool isDestinationAndTag = false;

            if (tempTags.size() > 0) {
                stringstream ss(tempTags);
                vector<string> result;

                while (ss.good()) {
                    string substr;
                    getline(ss, substr, ',');
                    result.push_back(substr);
                }

                for (std::string data:  result) {

                    std::string patternTagTemp = pattern;
                    int i = 0;

                    do {
                        i = i + 1;
                        int scoreTag;
                        bool isDataMatched = fuzzy_match_test(patternTagTemp.c_str(), data.c_str(),
                                                              scoreTag, true);

                        patternTagTemp.erase(patternTagTemp.begin());
                        if (isDataMatched && scoreTag > 49) {
                            matches.emplace_back(scoreTag, &entry);
                            isDestinationAndTag = true;
                            break;
                        }


                    } while (isDestinationAndTag == false && patternTagTemp.size() > 2 &&
                             isDataMatched == false && i < 3);
                    /* break foreach when we have more than one tag and result matched
                     * in first tag (isDestinationAndTag = true)
                     * other wise traverse all the tag from 0....n */
                    if (isDestinationAndTag)
                        break;
                    /*      int scoreTag;
                          bool isDataMatched = fuzzy_match_test(pattern.c_str(), data.c_str(),
                                                                scoreTag);
                          if (isDataMatched && scoreTag > 49) {
                              matches.emplace_back(scoreTag, &entry);
                              isDestinationAndTag = true;
                              break;
                          }*/
                }
            }

            /**/
            if (isDestinationAndTag == false && patternTemp.size() > 2 && isDataMatched == false) {
                patternTemp.erase(patternTemp.begin());

                int scoreTag;
                bool isDataMatched = fuzzy_match_test(patternTemp.c_str(),
                                                      entry.destinationName.c_str(),
                                                      scoreTag, true);

                if (isDataMatched == false) {
                    patternTemp.erase(patternTemp.begin());
                    isDataMatched = fuzzy_match_test(patternTemp.c_str(),
                                                     entry.destinationName.c_str(),
                                                     scoreTag, true);
                }
                if (isDataMatched && scoreTag > 49) {
                    matches.emplace_back(scoreTag, &entry);
                }
            }
            /**/

        }


    }
    std::sort(matches.begin(), matches.end(), [](auto &&a, auto &&b) { return a.first > b.first; });


    for (auto result : matches) {
        returnData.push_back(*result.second);
        if (returnData.size() == 70) {
            break;
        }

    }

    return returnData;


}

/* check tag contains*/

const std::string
performFuzzySearchTag(const std::string pattern, const std::string destinationName,
                      const std::string tagName) {

    std::string returnData;
    std::string patternTemp = pattern;

    int score;
    bool isDataMatched = fuzzy_match_test(pattern.c_str(), destinationName.c_str(),
                                          score, false);
    if (isDataMatched && score > 49) { // 49
        returnData = "";
    } else {

        string tempTags = tagName;
        bool isDestinationAndTag = false;

        if (tempTags.size() > 0) {
            stringstream ss(tempTags);
            vector<string> result;

            while (ss.good()) {
                string substr;
                getline(ss, substr, ',');
                result.push_back(substr);
            }

            for (std::string data:  result) {

                std::string patternTagTemp = pattern;
                int i = 0;

                do {
                    i = i + 1;
                    int scoreTag;
                    bool isDataMatched = fuzzy_match_test(patternTagTemp.c_str(), data.c_str(),
                                                          scoreTag, true);

                    patternTagTemp.erase(patternTagTemp.begin());
                    if (isDataMatched && scoreTag > 49) {

                        //returnData = data;
                        returnData.append(data);
                        returnData.append(",");
                        isDestinationAndTag = true;
                        break;
                    }


                } while (isDestinationAndTag == false && patternTagTemp.size() > 2 &&
                         isDataMatched == false && i < 3);
            }
        }

        /**/
        if (isDestinationAndTag == false && patternTemp.size() > 2 && isDataMatched == false) {
            patternTemp.erase(patternTemp.begin());

            int scoreTag;
            bool isDataMatched = fuzzy_match_test(patternTemp.c_str(),
                                                  destinationName.c_str(),
                                                  scoreTag, true);

            if (isDataMatched == false) {
                patternTemp.erase(patternTemp.begin());
                isDataMatched = fuzzy_match_test(patternTemp.c_str(),
                                                 destinationName.c_str(),
                                                 scoreTag, true);
            }
            if (isDataMatched && scoreTag > 49) {
                returnData = "";
            }
        }
        /**/

    }
    if (!returnData.empty()) // returnData.size() > 0
        returnData.pop_back();

    return returnData;


}

/* prepare data to perform fuzzy search*/

const std::vector<LJCPPBL_Models::MenuAndDestinationItem>
prepareDestData(unordered_map<string, LJCPPBL_Models::Destination> data) {

    std::vector<LJCPPBL_Models::MenuAndDestinationItem> vectorDirectoryDestinations;
    LJCPPBL_Models::MenuAndDestinationItem destinationItemTemp;
    for (auto result : data) {

        destinationItemTemp.id = to_string(result.second.ID);
        destinationItemTemp.destinationName = result.second.DestinationName;
        destinationItemTemp.tags = result.second.tags;
        vectorDirectoryDestinations.push_back(destinationItemTemp);


    }

    return vectorDirectoryDestinations;
}









