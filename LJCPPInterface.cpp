//
//  LJCPPInterface.cpp
//  LJCPPBL.Console.Test
//
//  Created by User on 12/04/19.
//  Copyright © 2019 User. All rights reserved.
//

#include "LJCPPInterface.hpp"
#include "Models/Models.hpp"
#include "Core/LJCPPBL.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>


using namespace std;
using namespace LJCPPBL_Models;
using namespace LJCPPBL_CORE;

#define EXPORT __attribute__((visibility("default")))


void Initialize(string clientAPIKEY, string apiBaseUri) {
    LJCPPBL::Initialize(clientAPIKEY, apiBaseUri);
}

EXPORT
void InitializeJson(string jsonString) {
    LJCPPBL::Initialize(jsonString);
}

EXPORT
string *GetJsonString() {
    string *strPointer = nullptr;
    string jsonString = LJCPPBL::GetJsonString();
    strPointer = &jsonString;
    return strPointer;
}
/* added new function*/

EXPORT
unordered_map<string, Destination> GetDestinationsList() {


    unordered_map<string, Destination> data = LJCPPBL::GetDestinationsList();

    return data;
}

EXPORT
unordered_map<string, Destination> GetDirectoryList() {
    unordered_map<string, Destination> data = LJCPPBL::GetDirectoryData();

    return data;
}

/*new */

EXPORT
GlobalValues *GetGlobalValues_DROID() {
    return LJCPPBL::GetGlobalValuesObject();

}

EXPORT
void SetGlobalValues_DROID(GlobalValues *gl) {
    LJCPPBL::InitializeByObject(gl);

}

EXPORT
bool deleteCachedFiles_Common(std::string filename) {

    const char *fileName = filename.c_str();;
    std::fstream f;
    string nA;
    bool areFilesDeleted = false;
    std::string line;
    f.open(filename);

    if (f.is_open()) //File has not been created!
    {
        std::remove(fileName); // delete file
        areFilesDeleted = !std::ifstream(filename);
        f.clear();
        f.close();
    }

    return areFilesDeleted;
}

EXPORT

string readWrite_DROID(std::string filename, std::string jsonfile) {
    std::fstream f;
    string nA;
    std::string line;

    try {

        f.open(filename);
        if (!f.is_open()) //File has not been created!
        {
            std::cout << filename << " not found. Creating if data is not null..." << std::endl;
            if (!jsonfile.empty()) {
                std::ofstream file(filename);
                file << jsonfile;
                //Try again to open
                f.open(filename);
                if (!f.is_open()) {
                    std::cout << "Error creating file " << filename << "!" << std::endl;
                    f.close(); //Is this necessary since the is_open() returns false?
                    return jsonfile;
                }
            }

        } else //File already exists, meaning it has a number in it
        {
            while (std::getline(f, line)) {
                nA = line.c_str();
            }
            f.clear();
            f.close();
        }
        if (!nA.empty()) return nA; else return jsonfile;
    } catch (exception &ex) {
        throw ex;
    }

    return "";
}


EXPORT



EXPORT
string GetJsonString_DROID() {
    string *strPointer = nullptr;
    string jsonString = LJCPPBL::GetJsonString();
    return jsonString;
}

//EXPORT
//multimap<string, vector<WalkingDirection>>* GetDirection(string source, string destination){
//    multimap<string, vector<WalkingDirection>>* ptr = nullptr;
//    multimap<string, vector<WalkingDirection>> directions = LJCPPBL::GetDirection(source, destination);
//    ptr = &directions;
//    return ptr;
//}

EXPORT
string
GetDirection_DROID(string source, string destination) { // To get directon from quicklink quicklink
    string directionJSON = LJCPPBL::GetDirection(source, destination);
    return directionJSON;
}

EXPORT
string GetDirectionByXYF_DROID(float x, float y, int floorid,
                               string destination) { // To get direction from xy to quicklink
    string directionJSON = LJCPPBL::GetDirectionXYF(x, y, floorid, destination);
    return directionJSON;

}

EXPORT
string GetDirectionByXYXY_DROID(float x1, float y1, float x2, float y2, int floorid1,
                                int floorid2) { // To get direction from xy to xy
    string directionJSON = LJCPPBL::GetDirectionXYXY(x1, y1, x2, y2, floorid1, floorid2);
    return directionJSON;
}

EXPORT
bool isFileExists(std::string filename) {

    //const char *fileName = filename.c_str();;
    std::fstream f;
    string nA;
    //bool areFilesDeleted = false;
    std::string line;
    f.open(filename);

    if (f.is_open()) //File exists
    {
        f.close();
        return true;
    }

    return false;
}
