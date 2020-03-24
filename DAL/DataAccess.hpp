//
//  DataAccess.cpp
//  AdminTool.Test
//
//  Created by User on 22/01/19.
//  Copyright © 2019 User. All rights reserved.
//

#include "../Models/Models.hpp"
#include "../Plugins/RestClient/restclient-cpp/restclient.h"
#include "../Plugins/RestClient/restclient-cpp/connection.h"
#include "../Plugins/JsonParser/json.hpp"
#include "../Core/GlobalValues.hpp"
#include <list>
#include <iostream>
#include <iterator>
#include <unordered_map>

using namespace std;
using json = nlohmann::json;
using namespace LJCPPBL_Models;

#ifndef LJCPPBL_DAL_LJCPPBLAPIDAL
#define LJCPPBL_DAL_LJCPPBLAPIDAL

namespace LJCPPBL_DAL {
    class LJCPPBLAPIDAL {

    public:
        static string GetJosnFromAPI() {
            json jsonObj;
            jsonObj = nullptr;
            try {
                jsonObj =
                        {
                                {"MapPointsAndPathJSON",  GetMapsAndPointsJSON()},
                                {"MapDestinationJSON",    GetMapDestinationsJSON()},
                                {"ParkingLotDetailsJSON", GetParkingDetailsJSON()},
                                {"DropOffPointsJSON",     GetDropOffPointsJSON()},
                                {"CustomEdgesJsSON",      GetCustomEdgesJSON()}
                        };
            }
            catch (exception &ex) {
                return "{internet_error}";
            }

            return jsonObj.dump().c_str();
        }

    public:
        static Graph GetGraph() {
            return LJCPPBLGlobalValues->SPCGraph;
        }

    public:
        static void SetGraph(Graph spcGraph) {
            LJCPPBLGlobalValues->SPCGraph = spcGraph;
        }

    public:
        static string GetIdentifier(string quickLink) {
            if (LJCPPBLGlobalValues->MapDestinations.size() > 0) {
                Destination ds = (LJCPPBLGlobalValues->MapDestinations.find(quickLink)->second);
                return ds.PointGuid;
            } else {
                return "";
            }
        }

    public:
        static Destination *GetDestination(string quickLink) {
            Destination *ds = nullptr;

            if (LJCPPBLGlobalValues->MapDestinations.size() > 0) {
                ds = &(LJCPPBLGlobalValues->MapDestinations.find(quickLink)->second);
            }

            return ds;
        }

    public:
        static Parking *GetParkingLotFromLotID(long parkingLotID) {
            Parking *pl = nullptr;

            if (LJCPPBLGlobalValues->ParkingLots.size() > 0) {
                pl = &(LJCPPBLGlobalValues->ParkingLots.find(parkingLotID)->second);
            }

            return pl;
        }

    public:
        static DropOffPoint *GetDroppOffPoint(string dropOffPointId) {
            DropOffPoint *dp = nullptr;

            if (LJCPPBLGlobalValues->DropOffPoints.size() > 0) {
                dp = &(LJCPPBLGlobalValues->DropOffPoints.find(dropOffPointId)->second);
            }

            return dp;
        }

    public:
        static Parking *GetParkingLotFromParkingType(long parkingTypeID, string quickLink) {
            Parking *pl = nullptr;

            for (auto &parkinglot : LJCPPBLGlobalValues->ParkingLots) {
                if (parkinglot.second.ParkingType == to_string(parkingTypeID) &&
                    parkinglot.second.QuickLink == quickLink) {
                    pl = &parkinglot.second;
                }
            }

            return pl;
        }

    public:
        static Edge *GetMapEdge(string pointAId, string pointBId) {
            Edge *edge = NULL;

            for (Edge *&mapedge : (LJCPPBLGlobalValues->SPCGraph).Edges) {
                if ((mapedge->A->Identifier == pointAId && mapedge->B->Identifier == pointBId) ||
                    (mapedge->A->Identifier == pointBId && mapedge->B->Identifier == pointAId)) {
                    edge = mapedge;
                }
            }

            return edge;
        }

    public:
        static CustomEdge *GetCustomEdge(Point p) {
            CustomEdge *edge = NULL;
            unordered_map<int, CustomEdge>::iterator itr;
            itr = (LJCPPBLGlobalValues->CustomEdges).find(p.MapCustomEdgeId);

            if (itr != (LJCPPBLGlobalValues->CustomEdges).end())
                edge = &(itr->second);

            return edge;
        }

    public:
        static FloorPlan *GetFloorPlan(long floorplanID) {
            FloorPlan *floorPlan = NULL;
            unordered_map<long, FloorPlan>::iterator itr;
            itr = (LJCPPBLGlobalValues->FloorPlans).find(floorplanID);

            if (itr != (LJCPPBLGlobalValues->FloorPlans).end())
                floorPlan = &(itr->second);

            return floorPlan;
        }

    public:
        static string GetMapsAndPointsJSON() {

            try {
                if (LJCPPBLGlobalValues->TempCachedJsonData.MapPointsAndPathJSON == "") {

                    std::string data = GetResponse("/GetAllEdges");

                    if (data == "{internet_error}") {
                        throw std::exception();
                    } else {
                        LJCPPBLGlobalValues->TempCachedJsonData.MapPointsAndPathJSON = data;
                    }
                }

            } catch (exception &ex) {
                throw std::exception();
            }

            return LJCPPBLGlobalValues->TempCachedJsonData.MapPointsAndPathJSON;
        }


    public:
        static string GetMapDestinationsJSON() {
            try {
                if (LJCPPBLGlobalValues->TempCachedJsonData.MapDestinationJSON == "") {
                    std::string data = GetResponse(
                            "/api/destination/getdestinationdataiosv2?lang=english");

                    if (data == "{internet_error}") {
                        throw std::exception();
                    } else {
                        LJCPPBLGlobalValues->TempCachedJsonData.MapDestinationJSON = data;
                    }
                }
            } catch (exception &ex) {
                throw std::exception();
            }

            return LJCPPBLGlobalValues->TempCachedJsonData.MapDestinationJSON;
        }

    public:
        static string GetParkingDetailsJSON() {

            try {
                if (LJCPPBLGlobalValues->TempCachedJsonData.ParkingLotDetailsJSON == "") {
                    std::string data = GetResponse("/api/Parking/GetParking");

                    if (data == "{internet_error}") {
                        throw std::exception();
                    } else {
                        LJCPPBLGlobalValues->TempCachedJsonData.ParkingLotDetailsJSON = data;
                    }
                }
            } catch (exception &ex) {
                throw std::exception();
            }
            return LJCPPBLGlobalValues->TempCachedJsonData.ParkingLotDetailsJSON;
        }

    public:
        static string GetDropOffPointsJSON() {

            try {
                if (LJCPPBLGlobalValues->TempCachedJsonData.DropOffPointsJSON == "") {
                    std::string data = GetResponse("/api/Parking/GetDropOffPoints");

                    if (data == "{internet_error}") {
                        throw std::exception();
                    } else {
                        LJCPPBLGlobalValues->TempCachedJsonData.DropOffPointsJSON = data;
                    }
                }
            } catch (exception &ex) {
                throw std::exception();
            }
            return LJCPPBLGlobalValues->TempCachedJsonData.DropOffPointsJSON;
        }

    public:
        static string GetElevatorEdgesJSON() {

            try {
                if (LJCPPBLGlobalValues->TempCachedJsonData.ElevatorEdgesJsSON == "") {
                    std::string data = GetResponse("/GetElevatorEdges");

                    if (data == "{internet_error}") {
                        throw std::exception();
                    } else {
                        LJCPPBLGlobalValues->TempCachedJsonData.ElevatorEdgesJsSON = data;
                    }
                }
            } catch (exception &ex) {
                throw std::exception();
            }

            return LJCPPBLGlobalValues->TempCachedJsonData.ElevatorEdgesJsSON;
        }

    public:
        static string GetCustomEdgesJSON() {
            try {
                if (LJCPPBLGlobalValues->TempCachedJsonData.CustomEdgesJsSON == "") {
                    std::string data = GetResponse("/GetCustomEdges");

                    if (data == "{internet_error}") {
                        throw std::exception();
                    } else {
                        LJCPPBLGlobalValues->TempCachedJsonData.CustomEdgesJsSON = data;
                    }
                }
            } catch (exception &ex) {
                throw std::exception();
            }

            return LJCPPBLGlobalValues->TempCachedJsonData.CustomEdgesJsSON;
        }

    public:
        static string GetAPIToken() {
            try {
                if (LJCPPBLGlobalValues->APIToken == "") {
                    RestClient::Response r = RestClient::post(
                            LJCPPBLGlobalValues->APIBaseUri + "/token",
                            "application/x-www-form-urlencoded",
                            "grant_type=password&username=" + LJCPPBLGlobalValues->ClientAPIKey);

                    auto jsonObj = json::parse(r.body);
                    for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
                        if (itr.key() == "access_token") {
                            LJCPPBLGlobalValues->APIToken = "bearer " + itr.value().get<string>();
                        }
                    }
                }
            } catch (exception &ex) {
                throw std::exception();
            }

            return LJCPPBLGlobalValues->APIToken;
        }

    private:
        static std::string GetResponse(string uri) {
            try {

                string apiToken = GetAPIToken();
                RestClient::init();
                RestClient::Connection *conn = new RestClient::Connection(
                        LJCPPBLGlobalValues->APIBaseUri);
                RestClient::HeaderFields headers;
                conn->SetTimeout(50);
                headers["Accept"] = "application/json";
                conn->SetHeaders(headers);
                conn->AppendHeader("Authorization", apiToken);
                auto response = conn->get(uri);
                RestClient::disable();
                if (response.code == 200) {
                    return response.body;
                } else {
                    return "{internet_error}";
                }

                return response.body;

            } catch (exception &ex) {
                throw std::exception();
            }
        }
    };
}
#endif

