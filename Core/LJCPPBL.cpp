
#include "LJCPPBL.hpp"
#include "../Models/Models.hpp"
#include "../Plugins/JsonParser/json.hpp"
#include "../Utilities/LJCPPBlUtility.hpp"
#include "SPC.hpp"
#include "GlobalValues.hpp"
#include "Direction.hpp"
#include "../DAL/DataAccess.hpp"
#include <future>

using namespace std;
using json = nlohmann::json;
using namespace LJCPPBL_Utilities;
using namespace LJCPPBL_CORE;
using namespace LJCPPBL_DAL;
GlobalValues *LJCPPBLGlobalValues;

void LJCPPBL_CORE::LJCPPBL::Initialize(string clientAPIKEY, string apiBaseUri) {
    LJCPPBLGlobalValues = new GlobalValues(clientAPIKEY, apiBaseUri);
//    LJCPPBLGlobalValues = *globalValues;
//    LJCPPBLGlobalValues -> ClientAPIKey = clientAPIKEY;
//    LJCPPBLGlobalValues -> APIBaseUri = apiBaseUri;
}

string LJCPPBL_CORE::LJCPPBL::GetJsonString() {
    return LJCPPBLAPIDAL::GetJosnFromAPI();
    //    LJCPPBLGlobalValues = *globalValues;
    //    LJCPPBLGlobalValues -> ClientAPIKey = clientAPIKEY;
    //    LJCPPBLGlobalValues -> APIBaseUri = apiBaseUri;
}

void LJCPPBL_CORE::LJCPPBL::InitializeByObject(GlobalValues *gl) {
    LJCPPBLGlobalValues = new GlobalValues(gl);
    //    LJCPPBLGlobalValues = *globalValues;
    //    LJCPPBLGlobalValues -> ClientAPIKey = clientAPIKEY;
    //    LJCPPBLGlobalValues -> APIBaseUri = apiBaseUri;
}

GlobalValues *LJCPPBL_CORE::LJCPPBL::GetGlobalValuesObject() {
    return LJCPPBLGlobalValues;
}


void LJCPPBL_CORE::LJCPPBL::Initialize(string jsonString) {
//    auto globalValues = new GlobalValues();
//    LJCPPBLGlobalValues = *globalValues;
    auto jsonObj = json::parse(jsonString);
    try {

        for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
            if (itr.key() == "MapPointsAndPathJSON") {
                future<Graph> _spcGraph = async(launch::async,
                                                LJCPPBLUtility::DeSerializeJsonToGraph,
                                                itr.value().get<string>());
                future<unordered_map<long, FloorPlan>> _floorplans = async(launch::async,
                                                                           LJCPPBLUtility::DeSerializeJsonToFloorPlans,
                                                                           itr.value().get<string>());

                LJCPPBLGlobalValues->SPCGraph = _spcGraph.get();
                LJCPPBLGlobalValues->FloorPlans = _floorplans.get();
                //LJCPPBLGlobalValues -> SPCGraph = LJCPPBLUtility::DeSerializeJsonToGraph(itr.value().get<string>());
                LJCPPBLGlobalValues->IsGraphInitialized = true;
                //LJCPPBLGlobalValues -> FloorPlans = LJCPPBLUtility::DeSerializeJsonToFloorPlans(itr.value().get<string>());
                LJCPPBLGlobalValues->PixelToFeetRatio = LJCPPBLUtility::DeSerializeJsonToPixelToFeetRatio(
                        itr.value().get<string>());
            } else if (itr.key() == "MapDestinationJSON") {


                future<unordered_map<string, Destination>> _mapDestination = async(launch::async,
                                                                                   LJCPPBLUtility::DeSerializeJsonToDestinations,
                                                                                   itr.value().get<string>());
                LJCPPBLGlobalValues->MapDestinations = _mapDestination.get();

                /*future<vector<LJCPPBL_Models::MenuAndDestinationItem>> _mapDestination= async(launch::async,LJCPPBLUtility::DeSerializeJsonToDirectoryDestination,itr.value().get<string>());
                LJCPPBLGlobalValues -> MapDestinationsNew = _mapDestination;*/
                // DeSerializeJsonToDirectoryDestinationNew(itr.value().get<string>());

                //LJCPPBLGlobalValues -> MapDestinations = LJCPPBLUtility::DeSerializeJsonToDestinations(itr.value().get<string>());
            } else if (itr.key() == "ParkingLotDetailsJSON") {
                future<unordered_map<long, Parking>> _parkingLots = async(launch::async,
                                                                          LJCPPBLUtility::DeSerializeJsonToParkings,
                                                                          itr.value().get<string>());
                LJCPPBLGlobalValues->ParkingLots = _parkingLots.get();
                //LJCPPBLGlobalValues -> ParkingLots = LJCPPBLUtility::DeSerializeJsonToParkings(itr.value().get<string>());
            } else if (itr.key() == "DropOffPointsJSON") {

                future<unordered_map<string, DropOffPoint>> _dropOffPoints = async(launch::async,
                                                                                   LJCPPBLUtility::DeSerializeJsonToDropOffPoints,
                                                                                   itr.value().get<string>());
                LJCPPBLGlobalValues->DropOffPoints = _dropOffPoints.get();
                //LJCPPBLGlobalValues -> DropOffPoints = LJCPPBLUtility::DeSerializeJsonToDropOffPoints(itr.value().get<string>());
            } else if (itr.key() == "CustomEdgesJsSON") {
                future<unordered_map<int, CustomEdge>> _customEdges = async(launch::async,
                                                                            LJCPPBLUtility::DeSerializeJsonToCustomEdges,
                                                                            itr.value().get<string>());
                LJCPPBLGlobalValues->CustomEdges = _customEdges.get();
                //LJCPPBLGlobalValues -> CustomEdges = LJCPPBLUtility::DeSerializeJsonToCustomEdges(itr.value().get<string>());
            } else if (itr.key() == "ElevatorEdgesJsSON") {
                future<unordered_map<string, Edge>> _elevatorEdges = async(launch::async,
                                                                           LJCPPBLUtility::DeSerializeJsonToElevatorEdges,
                                                                           itr.value().get<string>());
                LJCPPBLGlobalValues->ElevatorEdges = _elevatorEdges.get();
                //LJCPPBLGlobalValues -> ElevatorEdges = LJCPPBLUtility::DeSerializeJsonToElevatorEdges(itr.value().get<string>());
            }
        }

    }
    catch (const std::exception &e) {
        throw e; // exception("Asynch Download issue - unable to download data");
    }
}

/* added new function */
unordered_map<string, Destination> LJCPPBL::GetDestinationsList() {
    return LJCPPBLGlobalValues->MapDestinationsNew;
}

unordered_map<string, Destination> LJCPPBL::GetDirectoryData() {
    return LJCPPBLGlobalValues->MapDestinationsNew;
}


string LJCPPBL_CORE::LJCPPBL::GetDirection(string source, string destination) {
    return Direction::GetDirection(source, destination, 0);
}

string LJCPPBL_CORE::LJCPPBL::GetDirectionXYF(float x, float y, int floorid, string destination) {
    return Direction::GetDirectionByXYF(x, y, floorid, destination, 0);
}

string LJCPPBL_CORE::LJCPPBL::GetDirectionXYXY(float x1, float y1, float x2, float y2, int floorid1,
                                               int floorid2) {
    return Direction::GetDirectionByXYXY(x1, y1, x2, y2, floorid1, floorid2, 0);
}

//list<Point> LJCPPBL::GetShortestPath(string source, string destination, float* costOfPath)
//{
//    return SPC::GetShortestPath(source, destination, costOfPath);
//}

