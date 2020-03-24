//
//  LJCPPInterface.hpp
//  LJCPPBL.Console.Test
//
//  Created by User on 12/04/19.
//  Copyright © 2019 User. All rights reserved.
//

#ifndef LJCPPInterface_hpp
#define LJCPPInterface_hpp

#include "Models/Models.hpp"
#include <iostream>
#include <map>
#include <vector>
#include "Core/GlobalValues.hpp"

using namespace std;
using namespace LJCPPBL_Models;

void Initialize(string clientAPIKEY, string apiBaseUri);

void InitializeJson(string jsonString);

string *GetJsonString();

GlobalValues *GetGlobalValues_DROID();

void SetGlobalValues_DROID(GlobalValues *gl);

string GetJsonString_DROID();

string readWrite_DROID(std::string filename, std::string jsonfile);

//multimap<string, vector<WalkingDirection>>* GetDirection(string source, string destination);
string GetDirection_DROID(string source, string destination);

string GetDirectionByXYF_DROID(float x, float y, int floorid, string destination);

string GetDirectionByXYXY_DROID(float x1, float y1, float x2, float y2, int floorid1, int floorid2);

bool deleteCachedFiles_Common(std::string filename);

bool isFileExists(std::string filename);

/* new */
unordered_map<string, Destination> GetDestinationsList();

unordered_map<string, Destination> GetDirectoryList();

//extern "C" list<Point>* GetShortestPath(string source, string destination, float* costOfPath);
#endif /* LJCPPInterface_hpp */
