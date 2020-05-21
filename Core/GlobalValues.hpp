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
    string destinationJson;
public:
    string menuJson;
public:
    string mapFloorBeaconJSON;


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
    string ADALAPIToken;
public:
    string APIBaseUri;

public:
    GlobalValues(string apiKEY, string baseUri) {
        auto cachedJsonData = new LJCPPBLCache();
        TempCachedJsonData = *cachedJsonData;
        ClientAPIKey = apiKEY;
        APIToken = "";
        ADALAPIToken = "";
        APIBaseUri = baseUri;
        PixelToFeetRatio = 0;
    }

public:
    GlobalValues(string apiKEY, string baseUri, string adalToken) {
        auto cachedJsonData = new LJCPPBLCache();
        TempCachedJsonData = *cachedJsonData;
        ClientAPIKey = apiKEY;
        APIToken = "";
        ADALAPIToken = adalToken;
        APIBaseUri = baseUri;
        PixelToFeetRatio = 0;
    }

public:
    GlobalValues() {
        auto cachedJsonData = new LJCPPBLCache();
        TempCachedJsonData = *cachedJsonData;
        ClientAPIKey = "";
        APIToken = "";
        ADALAPIToken = "";
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
        ADALAPIToken = gl->ADALAPIToken;
        APIBaseUri = gl->APIBaseUri;
        PixelToFeetRatio = gl->PixelToFeetRatio;
        CustomEdges = gl->CustomEdges;
        MapDestinations = gl->MapDestinations;
        destinationJson = gl->destinationJson;
        menuJson = gl->menuJson;
        mapFloorBeaconJSON = gl->mapFloorBeaconJSON;
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

