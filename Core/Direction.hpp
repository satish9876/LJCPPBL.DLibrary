

#ifndef Direction_cpp
#define Direction_cpp

#include "../Models/Models.hpp"
#include "../DAL/DataAccess.hpp"
#include "../Plugins/JsonParser/json.hpp"
#include "SPC.hpp"
#include "GlobalValues.hpp"
#include "BulletPoints.hpp"

#include <math.h>
#include <iostream>
#include <list>
#include <map>

using namespace std;
using json = nlohmann::json;
using namespace LJCPPBL_DAL;
using namespace LJCPPBL_Models;
using namespace EnumParkingOrder;

namespace LJCPPBL_CORE {
    class Direction {
    public:
        static string ConvertFloatToString(float f, int precision) {

            stringstream X_P;
            X_P << fixed << setprecision(precision) << f;
            return X_P.str();
        }

    public:
        static string GetDirection(string source, string destination, int parkingTypeOrLotID) {
            string directionJSON = "";

            if (source != "0" && destination != "0") {
                auto QBeg = LJCPPBLAPIDAL::GetDestination(source);
                auto QEnd = LJCPPBLAPIDAL::GetDestination(destination);

                if (QBeg != nullptr && QEnd != nullptr) {
                    auto start = std::chrono::system_clock::now();
                    // Some computation here
                    SPC *ptr = new SPC();
                    auto shortestPath = ptr->GetShortestPath(QBeg->PointGuid, QEnd->PointGuid,
                                                             NULL);
                    auto end = std::chrono::system_clock::now();

                    std::chrono::duration<double> elapsed_seconds = end - start;
                    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

                    std::cout << "finished computation at " << std::ctime(&end_time)
                              << "elapsed time: " << elapsed_seconds.count() << "s\n";
                    auto bulletPoints = BulletPoint::BuildBulletPointsFromPathPoints(shortestPath,
                                                                                     1, None);
                    auto shortestPathDesc = BulletPoint::BuildShortestPathFromPathPoints(
                            shortestPath, None);
                    directionJSON = ConvertDirectionDataToJson(bulletPoints, shortestPathDesc);
                }
            } else if (source == "0" && destination != "0") {
                auto QEnd = LJCPPBLAPIDAL::GetDestination(destination);

                if (QEnd != nullptr) {
                    auto QMid = LJCPPBLAPIDAL::GetParkingLotFromParkingType(parkingTypeOrLotID,
                                                                            QEnd->QuickLink);

                    if (QMid != nullptr) {
                        SPC *ptr = new SPC();
                        auto dropOffPoint = LJCPPBLAPIDAL::GetDroppOffPoint(QMid->DropOffPoint);
                        auto shortestPath1 = ptr->GetShortestPath(dropOffPoint->PointGuid,
                                                                  QMid->PointGuid, NULL);
                        auto bulletPoints1 = BulletPoint::BuildBulletPointsFromPathPoints(
                                shortestPath1, 1, First);
                        auto shortestPath2 = ptr->GetShortestPath(dropOffPoint->PointGuid,
                                                                  QEnd->PointGuid, NULL);
                        auto bulletPoint2 = BulletPoint::BuildBulletPointsFromPathPoints(
                                shortestPath2, bulletPoints1.size(), First);
                    }
                }
            } else if (source != "0" && destination == "0") {
                auto QBeg = LJCPPBLAPIDAL::GetDestination(source);

                if (QBeg != nullptr) {
                    auto QMid = LJCPPBLAPIDAL::GetParkingLotFromLotID(parkingTypeOrLotID);

                    if (QMid != nullptr) {
                        SPC *ptr = new SPC();
                        auto dropOffPoint = LJCPPBLAPIDAL::GetDroppOffPoint(QMid->DropOffPoint);
                        auto shortestPath1 = ptr->GetShortestPath(QBeg->PointGuid, QMid->PointGuid,
                                                                  NULL);
                        auto bulletPoints1 = BulletPoint::BuildBulletPointsFromPathPoints(
                                shortestPath1, 1, Last);
                        auto shortestPath2 = ptr->GetShortestPath(QMid->PointGuid,
                                                                  dropOffPoint->PointGuid, NULL);
                        auto bulletPoint2 = BulletPoint::BuildBulletPointsFromPathPoints(
                                shortestPath2, bulletPoints1.size(), Last);
                    }
                }
            }

            return directionJSON;
        }


    public:
        static string GetDirectionByXYF(float x, float y, int floorid, string destination,
                                        int parkingTypeOrLotID) {
            string directionJSON = "";

            try {
                if (x > 0 && y > 0 && floorid > 0 && destination != "0") {
                    if (LJCPPBLAPIDAL::GetFloorPlan(floorid) != NULL) {
                        auto QEnd = LJCPPBLAPIDAL::GetDestination(destination);
                        SPC *ptr = new SPC();
                        auto shortestPath = ptr->GetShortestPathByXYF(x, y, floorid,
                                                                      QEnd->PointGuid, NULL);
                        auto bulletPoints = BulletPoint::BuildBulletPointsFromPathPoints(
                                shortestPath, 1, None);
                        auto shortestPathDesc = BulletPoint::BuildShortestPathFromPathPoints(
                                shortestPath, None);
                        directionJSON = ConvertDirectionDataToJson(bulletPoints, shortestPathDesc);
                    } else {
                        directionJSON = "Invalid floor id";
                    }
                }
            }
            catch (...) {
                directionJSON = "Could not find direction as an exception occured";
            }

            return directionJSON;
        }

    public:
        static string
        GetDirectionByXYXY(float x1, float y1, float x2, float y2, int floorid1, int floorid2,
                           int parkingTypeOrLotID) {
            string directionJSON = "";

            try {
                if (x1 > 0 && y1 > 0 && x2 > 0 && y2 > 0 && floorid1 > 0 && floorid2 > 0) {
                    if (LJCPPBLAPIDAL::GetFloorPlan(floorid1) != NULL &&
                        LJCPPBLAPIDAL::GetFloorPlan(floorid2) != NULL) {
                        SPC *ptr = new SPC();
                        auto shortestPath = ptr->GetShortestPathByXYXY(x1, y1, x2, y2, floorid1,
                                                                       floorid2, NULL);
                        auto bulletPoints = BulletPoint::BuildBulletPointsFromPathPoints(
                                shortestPath, 1, None);
                        auto shortestPathDesc = BulletPoint::BuildShortestPathFromPathPoints(
                                shortestPath, None);
                        directionJSON = ConvertDirectionDataToJson(bulletPoints, shortestPathDesc);
                    } else {
                        directionJSON = "Invalid floor id";
                    }
                }
            }
            catch (...) {
                directionJSON = "Could not find direction as an exception occured";
            }

            return directionJSON;
        }


    public:
        static float CalculateWalkingTime(multimap<string, vector<WalkingDirection>> nDirections) {
            float totalDistance = 0;

            for (auto itr = nDirections.begin(); itr != nDirections.end(); ++itr) {
                for (auto &step : itr->second) {
                    totalDistance += step.Distance;
                }
            }

            return (totalDistance / 180);
        }

    public:
        static vector<string> split(string strToSplit, char delimeter) {
            stringstream ss(strToSplit);
            string item;
            vector<string> splittedStrings;
            while (getline(ss, item, delimeter)) {
                splittedStrings.push_back(item);
            }
            return splittedStrings;
        }

    public:
        static string
        ConvertDirectionDataToJson(multimap<string, vector<WalkingDirection>> directions,
                                   multimap<string, FloorDirectionDetail> shortestPath) {
            if (directions.size() > 0) {
                string jsonString = "{\"ShortestPath\":{";
                int counter = 0;
                multimap<string, FloorDirectionDetail>::iterator itr1;

                for (itr1 = shortestPath.begin(); itr1 != shortestPath.end(); ++itr1) {
                    jsonString += ("\"" + itr1->first + "\":{");
                    jsonString += ("\"title\":\"" + (itr1->second).Title + "\",");
                    jsonString += ("\"url\":\"" + (itr1->second).Url + "\",");
                    jsonString += ("\"description\":\"" + (itr1->second).Description + "\",");
                    auto pathPts = split((itr1->second).Pathpoints, ' ');
                    if (pathPts.size() == 1) {
                        jsonString += ("\"pathpoints\":\"" + (itr1->second).Pathpoints +
                                       (itr1->second).Pathpoints + "\",");
                    } else
                        jsonString += ("\"pathpoints\":\"" + (itr1->second).Pathpoints + "\",");
                    jsonString += ("\"floorid\":" + to_string((itr1->second).FloorID) + "}");
                    counter++;

                    if (counter < shortestPath.size())
                        jsonString += ",";
                }

                jsonString += "},\"WalkingDirections\":{";
                counter = 0;
                int counter2 = 0;

                for (auto itr = directions.begin(); itr != directions.end(); ++itr) {
                    jsonString += ("\"" + itr->first + "\":[");

                    for (auto &step : itr->second) {
                        jsonString += ("{\"distance\":" + to_string(step.Distance) + ",");

                        jsonString += ("\"Direction\":\"" + step.Direction + "\",");
                        jsonString += ("\"Description\":\"" + step.Description + "\",");
                        jsonString += ("\"DescriptionMtr\":\"" + step.DescriptionMtr + "\",");
                        jsonString += ("\"path\":\"" + step.Path + "\"}");
                        counter++;

                        if (counter < itr->second.size())
                            jsonString += ",";
                    }

                    jsonString += "]";
                    counter = 0;
                    counter2++;

                    if (counter2 < directions.size())
                        jsonString += ",";
                }

                auto walkingTime = ConvertFloatToString(CalculateWalkingTime(directions), 0);
                jsonString += "},\"Note\":\"Estimated walking time is " + walkingTime
                              + " minutes\", \"ResponseId\":\"000000-0000\",";
                jsonString += "\"ShortNote\":\"Walking time: " + walkingTime + " minutes\"}";
                return jsonString;
            } else {
                return "Not able to find directions";
            }
        }
    };
}

#endif
