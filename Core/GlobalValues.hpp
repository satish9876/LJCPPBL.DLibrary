////
////  GlobalValues.hpp
////
////
////  Created by User on 28/01/19.
////
//
#ifndef GlobalValues_hpp
#define GlobalValues_hpp

#include "../Models/Models.hpp"
#include <iostream>
#include <map>
#include <unordered_map>

using namespace LJCPPBL_Models;

class GlobalValues {
public:
    unordered_map <string, Destination> MapDestinations;
public:
    unordered_map <string, Destination> MapDestinationsNew;


public:
    unordered_map<long, FloorPlan> FloorPlans;
public:
    unordered_map<long, Parking> ParkingLots;
public:
    unordered_map <string, DropOffPoint> DropOffPoints;
public:
    Graph SPCGraph;
public:
    unordered_map <string, Edge> ElevatorEdges;
//    public: multimap<string, Edge> AdjacentEdgesA;
//    public: multimap<string, Edge> AdjacentEdgesB;
//    public: multimap<long, Edge> StartElevatorEdges;
//    public: multimap<long, Edge> EndElevatorEdges;
public:
    unordered_map<int, CustomEdge> CustomEdges;
public:
    bool IsGraphInitialized;
public:
    float PixelToFeetRatio;
public:
    string ClientAPIKey;
public:
    LJCPPBLCache TempCachedJsonData;
public:
    string APIToken;
public:
    string APIBaseUri;

public:
    GlobalValues(string apiKEY, string baseUri) {
        auto cachedJsonData = new LJCPPBLCache();
        TempCachedJsonData = *cachedJsonData;
        ClientAPIKey = apiKEY;
        APIToken = "";
        APIBaseUri = baseUri;
        PixelToFeetRatio = 0;
    }

public:
    GlobalValues() {
        auto cachedJsonData = new LJCPPBLCache();
        TempCachedJsonData = *cachedJsonData;
        ClientAPIKey = "";
        APIToken = "";
        APIBaseUri = "";
        PixelToFeetRatio = 0;
    }

public:
    GlobalValues *getGlobalValues() {
        return this;
    }

public:
    GlobalValues(GlobalValues *gl) {
        auto cachedJsonData = new LJCPPBLCache();
        TempCachedJsonData = *cachedJsonData;
        ClientAPIKey = gl->ClientAPIKey;
        APIToken = gl->APIToken;
        APIBaseUri = gl->APIBaseUri;
        PixelToFeetRatio = gl->PixelToFeetRatio;
        CustomEdges = gl->CustomEdges;
        MapDestinations = gl->MapDestinations;
        MapDestinationsNew = gl->MapDestinationsNew;
        FloorPlans = gl->FloorPlans;
        ParkingLots = gl->ParkingLots;
        DropOffPoints = gl->DropOffPoints;
        SPCGraph = gl->SPCGraph;
        ElevatorEdges = gl->ElevatorEdges;
        CustomEdges = gl->CustomEdges;

    }

};

extern GlobalValues *LJCPPBLGlobalValues;

#endif /* GlobalValues_hpp */

