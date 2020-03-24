//
//  DataAccess.hpp
//  LJCPPBL.SLibrary
//
//  Created by User on 25/01/19.
//  Copyright © 2019 User. All rights reserved.
//

#ifndef LJCPPBL_hpp
#define LJCPPBL_hpp

#include "../Models/Models.hpp"
#include "GlobalValues.hpp"
#include <iostream>
#include <map>
#include <unordered_map>

using namespace std;
using namespace LJCPPBL_Models;

namespace LJCPPBL_CORE {

    class LJCPPBL {
    public:
        static void Initialize(string clientAPIKEY, string apiBaseUri);

    public:
        static void InitializeByObject(GlobalValues *gl);

    public:
        static GlobalValues *GetGlobalValuesObject();

    public:
        static void Initialize(string jsonString);

    public:
        static string GetJsonString();

    public:
        static int readWrite(std::string filename);

    public:
        static unordered_map<string, Destination> GetDestinationsList();

    public:
        static unordered_map<string, Destination> GetDirectoryData();

    public:
        static string GetDirection(string source, string destination);

    public:
        static string GetDirectionXYF(float x, float y, int floorid, string destination);

    public:
        static string
        GetDirectionXYXY(float x1, float y1, float x2, float y2, int floorid1, int floorid2);
        //public: static list<Point> GetShortestPath(string source, string destination, float* costOfPath);

    public:
        static string DeSerializeJsonToDirectoryDestination(string source, string destination);
    };
}

#endif /* DataAccess_hpp */

