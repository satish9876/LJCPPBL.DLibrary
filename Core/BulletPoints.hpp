//
//  BulletPoints.cpp
//
//
//  Created by User on 31/01/19.
//

#ifndef BulletPoints_cpp
#define BulletPoints_cpp


#include "../Plugins/JsonParser/json.hpp"
#include "../Models/Models.hpp"
#include "../DAL/DataAccess.hpp"
#include "../Utilities/LJCPPBlUtility.hpp"
#include "SPC.hpp"
#include "GlobalValues.hpp"

#include <math.h>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <vector>
#include <iomanip>

using namespace std;
using json = nlohmann::json;
using namespace LJCPPBL_Models;
using namespace LJCPPBL_Utilities;
using namespace EnumParkingOrder;

namespace LJCPPBL_CORE {
    //Implements methods for converting path points into direction
    class BulletPoint {


        //Converts float to string by turncating decimal values upto percision
    public:
        static string ConvertFloatToString(float f, int precision) {

            stringstream X_P;
            X_P << fixed << setprecision(precision) << f;
            return X_P.str();
        }

        /* convert ft to mtr and return result in string*/
    public:
        static string ConvertFeetToMtr(float f, int precision) {

            stringstream X_P;
            X_P << fixed << setprecision(precision) << f * 0.3048;
            return X_P.str();
        }

        //Builds bullet points from path points
    public:
        static multimap<string, vector<WalkingDirection>>
        BuildBulletPointsFromPathPoints(list<Point> lstPathPoints, int stepCounter,
                                        ParkingOrder parkingOrder, bool includeDistance = true) {
            map<string, string> mapDirectionStateDescriptions = {{"GO STRAIGHT",     "Go straight"},
                                                                 {"RIGHT",           "Turn right"},
                                                                 {"SLIGHT RIGHT",    "Turn slight right"},
                                                                 {"IMMEDIATE RIGHT", "Turn immediate right"},
                                                                 {"CONTINUE",        "Continue"},
                                                                 {"LEFT",            "Turn left"},
                                                                 {"SLIGHT LEFT",     "Turn slight left"},
                                                                 {"IMMEDIATE LEFT",  "Turn immediate left"},
                                                                 {"START",           "Start"},
                                                                 {"FLOOR-FLOOR",     "Continue"}}; // Dictionary that has direction state as keys and therir respective descriptions as values

            multimap<string, vector<WalkingDirection>> mapDirectionLegends; // Dictrionary that will have bullet points
            string currentLegendKey;
            vector<WalkingDirection> walkingDirections;
            bool addNewStepLegend = true;

            if (lstPathPoints.size() > 0) {
                float prevDistance = 0;
                string prevPath = "", prevEdgeName = "", prevDirectionState = "", lastDirectionState = "";

                //iterate over path points
                for (int i = 0; i < lstPathPoints.size() - 1; i++) {
                    list<Point>::iterator itrA = lstPathPoints.begin();
                    list<Point>::iterator itrB = lstPathPoints.begin();
                    advance(itrA, i); // Access point at position i (Point A)
                    advance(itrB, i + 1); // Access point at position i + 1(Point B)
                    string edgeName = "";
                    float currentDirectionDistance = 0;
                    string currentDirectionPath = "", directionState = "", directionDesc = "";
                    long floorIDA = LJCPPBLUtility::GetFloorIDFromPointIDentifier(
                            itrA->Identifier); // Get floorID from point identifier
                    long floorIDB = LJCPPBLUtility::GetFloorIDFromPointIDentifier(itrB->Identifier);
                    edgeName = LJCPPBLAPIDAL::GetMapEdge(itrA->Identifier, itrB->Identifier)->Name;
                    long floorIDC = 0;

                    //Check if adding a new legend is needed
                    if (addNewStepLegend) {
                        auto floorPlan = ((LJCPPBLGlobalValues->FloorPlans).find(floorIDA))->second;
                        string floorName = floorPlan.FloorName;
                        string stepDesc = GetStepDescription(floorName, "", stepCounter);
                        currentLegendKey = stepDesc;
                    }

                    if (floorIDA == floorIDB) {
                        float finalAngle = 0;
                        float pixelToFeetRatio = LJCPPBLGlobalValues->PixelToFeetRatio;

                        if ((i + 2) < lstPathPoints.size()) {
                            list<Point>::iterator itrC = lstPathPoints.begin();
                            advance(itrC, i + 2);
                            floorIDC = LJCPPBLUtility::GetFloorIDFromPointIDentifier(
                                    itrC->Identifier);
                            finalAngle = LJCPPBLUtility::GetAngleBetweenEdges(*itrA, *itrB, *itrC);
                            finalAngle = finalAngle < 0 ? 360 + round(finalAngle) : finalAngle;
                            directionState = (edgeName == "Elevator") ? "RIGHT"
                                                                      : GetDirectionStateFromAngle(
                                            finalAngle);
                        }

                        currentDirectionDistance = round(
                                LJCPPBLUtility::GetDistanceBetweenPoints(*itrA, *itrB,
                                                                         pixelToFeetRatio));

                        if (prevPath == "") {
                            currentDirectionPath = (ConvertFloatToString(itrA->X, 2) + "," +
                                                    ConvertFloatToString(itrA->Y, 2) +
                                                    " " + ConvertFloatToString(itrB->X, 2) + "," +
                                                    ConvertFloatToString(itrB->Y, 2));
                        } else
                            currentDirectionPath = (" " + ConvertFloatToString(itrB->X, 2) + "," +
                                                    ConvertFloatToString(itrB->Y, 2));

                        addNewStepLegend = false;
                    } else {
                        directionState = "FLOOR-FLOOR";
                        currentDirectionDistance = 0;
                        directionDesc = GetElevatorEdgeDescription(*itrA, *itrB, floorIDB,
                                                                   edgeName);
                        addNewStepLegend = true;
                        stepCounter++;
                    }

                    if ((directionState == "GO STRAIGHT" || directionState == "CONTINUE")
                        && (floorIDB == floorIDC || floorIDC == 0)) {
                        prevDistance += currentDirectionDistance;
                        prevPath += currentDirectionPath;

                        if (i > 0 && lastDirectionState == "") {
                            list<Point>::iterator itrD = lstPathPoints.begin();
                            advance(itrD, i - 1);
                            float finalAngle = LJCPPBLUtility::GetAngleBetweenEdges(*itrD, *itrA,
                                                                                    *itrB);
                            finalAngle = finalAngle < 0 ? 360 + round(finalAngle) : finalAngle;
                            lastDirectionState = ((prevEdgeName == "Elevator") ? "RIGHT"
                                                                               : GetDirectionStateFromAngle(
                                            finalAngle));
                        }

                        if (edgeName != "" && edgeName != "SKIP")
                            prevEdgeName = edgeName;
                    } else {
                        WalkingDirection *direction = new WalkingDirection();

                        if ((stepCounter == 1 || (parkingOrder == None || parkingOrder == Last))
                            && walkingDirections.size() == 0 && mapDirectionLegends.size() == 0)
                            direction->Direction = "START";
                        else if (prevDirectionState == "FLOOR-FLOOR")
                            direction->Direction = "CONTINUE";
                        else if (directionState == "FLOOR-FLOOR")
                            direction->Direction = "FLOOR-FLOOR";
                        else
                            direction->Direction = (lastDirectionState == "" ? (
                                    prevDirectionState == "" ? directionState : prevDirectionState)
                                                                             : lastDirectionState);

                        direction->Distance = (prevDistance + currentDirectionDistance);
                        float tempDistance = (prevDistance + currentDirectionDistance);

                        direction->Path = (prevPath + currentDirectionPath);
                        directionDesc = ((directionDesc != "") ? directionDesc : (edgeName == "" ||
                                                                                  edgeName ==
                                                                                  "SKIP")
                                                                                 ? prevEdgeName
                                                                                 : edgeName);

                        if (directionState != "FLOOR-FLOOR" || directionDesc == "")
                            directionDesc = GetDirectionDescByDirectionState(directionDesc,
                                                                             direction->Direction,
                                                                             mapDirectionStateDescriptions);


                        /*   if (includeDistance == true && (direction->Distance) > 0)
                               directionDesc = (directionDesc + " (Go " +
                                       ConvertFloatToString(direction->Distance, 0) + "ft.)");*/
                        string directionDescTemp = directionDesc;
                        if (includeDistance == true && (direction->Distance) > 0) {
                            //directionDescTemp = directionDesc;
                            directionDesc = (directionDesc + " (Go " +
                                             ConvertFloatToString(direction->Distance, 0) + "ft.)");

                            direction->Description = LJCPPBLUtility::ReplaceSubstring(directionDesc,
                                                                                      "$", "");

                            direction->DescriptionMtr = LJCPPBLUtility::ReplaceSubstring(
                                    directionDescTemp + " (Go " +
                                    ConvertFeetToMtr(direction->Distance, 0) + "mtr.)",
                                    "$", "");
                        } else {


                            direction->Description = LJCPPBLUtility::ReplaceSubstring(directionDesc,
                                                                                      "$", "");
                            direction->DescriptionMtr = LJCPPBLUtility::ReplaceSubstring(
                                    directionDescTemp,
                                    "$", "");


                        }

                        walkingDirections.push_back(*direction);
                        prevDistance = 0;
                        prevPath = "";
                        lastDirectionState = "";
                        prevDirectionState = directionState;
                        prevEdgeName = "";

                    }

                    if (directionState == "FLOOR-FLOOR") {
                        mapDirectionLegends.insert(
                                pair<string, vector<WalkingDirection>>(currentLegendKey,
                                                                       walkingDirections));
                        walkingDirections.clear();
                    }
                }
            }

            if (parkingOrder == None || parkingOrder == First) {
                WalkingDirection *direction = new WalkingDirection();
                direction->Direction = "Arrive at Destination";
                direction->Distance = 0;
                direction->Path = "";
                direction->Description = "Arrive at Destination";
                direction->DescriptionMtr = "Arrive at Destination";
                walkingDirections.push_back(*direction);
            }

            if (lstPathPoints.size() > 0)
                mapDirectionLegends.insert(pair<string, vector<WalkingDirection>>(currentLegendKey,
                                                                                  walkingDirections));

            return mapDirectionLegends;
        }

    public:
        static multimap<string, FloorDirectionDetail>
        BuildShortestPathFromPathPoints(list<Point> lstPathPoints, ParkingOrder parkingOrder) {
            multimap<string, FloorDirectionDetail> mapFloorDirectionLegends;
            bool addNewLegend = true;

            if (lstPathPoints.size() > 0) {
                int counter = 1;
                FloorDirectionDetail *floorDirectionLegend;
                string legendKey;

                for (int i = 0; i < lstPathPoints.size() - 1; i++) {
                    list<Point>::iterator itrA = lstPathPoints.begin();
                    list<Point>::iterator itrB = lstPathPoints.begin();
                    advance(itrA, i);
                    advance(itrB, i + 1);
                    long floorIDA = LJCPPBLUtility::GetFloorIDFromPointIDentifier(itrA->Identifier);
                    long floorIDB = LJCPPBLUtility::GetFloorIDFromPointIDentifier(itrB->Identifier);

                    if (addNewLegend) {
                        floorDirectionLegend = new FloorDirectionDetail();
                        auto floorPlan = ((LJCPPBLGlobalValues->FloorPlans).find(floorIDA))->second;
                        floorDirectionLegend->Title = floorPlan.FloorName;
                        floorDirectionLegend->Url = floorPlan.MapUrl;
                        floorDirectionLegend->Description = "";
                        floorDirectionLegend->Pathpoints = "";
                        floorDirectionLegend->FloorID = floorPlan.FloorPlanId;
                        legendKey = "f" + to_string(counter);
                        addNewLegend = false;
                    }

//                    if(floorIDA == floorIDB)
//                    {
//                        floorDirectionLegend -> Pathpoints = ((floorDirectionLegend -> Pathpoints) +
//                                                              (ConvertFloatToString(itrA -> X,2) + "," + ConvertFloatToString(itrA -> Y,2) + " "));
//                    }
//                    else
//                    {
//                        mapFloorDirectionLegends.insert(pair<string, FloorDirectionDetail>(legendKey, *floorDirectionLegend));
//                        addNewLegend = true;
//                        counter++;
//                    }
                    //This is to fix the problem, where end points were not being processed incase of elevator
                    floorDirectionLegend->Pathpoints = ((floorDirectionLegend->Pathpoints) +
                                                        (ConvertFloatToString(itrA->X, 2) + "," +
                                                         ConvertFloatToString(itrA->Y, 2) + " "));

                    //This is to fix the problem, where end points are removed on the last floor
                    if (i == lstPathPoints.size() - 2) {
                        floorDirectionLegend->Pathpoints = ((floorDirectionLegend->Pathpoints) +
                                                            (ConvertFloatToString(itrB->X, 2) +
                                                             "," +
                                                             ConvertFloatToString(itrB->Y, 2) +
                                                             " "));
                    }
                    if (floorIDA != floorIDB) {
                        mapFloorDirectionLegends.insert(
                                pair<string, FloorDirectionDetail>(legendKey,
                                                                   *floorDirectionLegend));
                        addNewLegend = true;
                        counter++;
                    }

                }

                mapFloorDirectionLegends.insert(
                        pair<string, FloorDirectionDetail>(legendKey, *floorDirectionLegend));
            }

            return mapFloorDirectionLegends;
        }

    public:
        static map<string, list<WalkingDirection>>
        BuildBulletPointsFromPathPoints(list<Point> lstPathPoints, enum ParkingOrder parkingOrder,
                                        string parkingLotName, int stepCounter) {
            map<string, list<WalkingDirection>> walkingDirections;

            if (lstPathPoints.size() > 0) {
                if (stepCounter <= 1) {
                    list<WalkingDirection> stepWalkingDirections;

                    if (parkingOrder == First) {
                        string stepDesc = GetStepDescription("", parkingLotName, stepCounter);
                        walkingDirections.insert(pair<string, list<WalkingDirection>>(stepDesc,
                                                                                      stepWalkingDirections));
                    } else if (parkingOrder == None) {
                        auto firstPathPoint = lstPathPoints.front();
                        long floorID = LJCPPBLUtility::GetFloorIDFromPointIDentifier(
                                firstPathPoint.Identifier);
                        string floorName = (LJCPPBLGlobalValues->FloorPlans.find(
                                floorID)->second).FloorName;
                        string stepDesc = GetStepDescription(floorName, "", stepCounter);
                        walkingDirections.insert(pair<string, list<WalkingDirection>>(stepDesc,
                                                                                      stepWalkingDirections));
                    }
                }

                for (int i = 0; i < lstPathPoints.size(); i++) {

                }
            }

            return walkingDirections;
        }

        static string GetStepDescription(string floorName, string parkingLotName, int stepCounter) {
            if (floorName != "" && parkingLotName == "") {
                return "Step " + to_string(stepCounter) + " - " +
                       LJCPPBLUtility::ReplaceSubstring(floorName, "the", "");
            } else if (parkingLotName != "" && floorName == "") {
                return "Step " + to_string(stepCounter) + " - " + parkingLotName;
            } else {
                return "";
            }
        }

        static string GetDirectionStateFromAngle(double finalAngle) {
            if (((finalAngle >= 0) && (finalAngle <= 22)) ||
                ((finalAngle <= 360) && (finalAngle > 338)))
                return "GO STRAIGHT";
            else if (((finalAngle >= 90) && (finalAngle <= 111)) ||
                     ((finalAngle < 90) && (finalAngle > 66)))
                return "RIGHT";
            else if (((finalAngle > 22) && (finalAngle <= 66)))
                return "SLIGHT RIGHT";
            else if (((finalAngle < 157) && (finalAngle > 111)))
                return "IMMEDIATE RIGHT";
            else if (((finalAngle >= 180) && (finalAngle <= 202)) ||
                     ((finalAngle < 180) && (finalAngle >= 157)))
                return "CONTINUE";
            else if (((finalAngle >= 270) && (finalAngle <= 292)) ||
                     ((finalAngle < 270) && (finalAngle >= 258)))
                return "LEFT";
            else if (((finalAngle > 292) && (finalAngle <= 338)))
                return "SLIGHT LEFT";
            else if (((finalAngle < 258) && (finalAngle > 202)))
                return "IMMEDIATE LEFT";
            else
                return "";
        }

        static string GetDirectionDescByDirectionState(string directionDesc, string directionState,
                                                       map<string, string> mapdirectionStateDescs) {
            map<string, string>::iterator itr;
            itr = mapdirectionStateDescs.find(directionState);

            vector<size_t> positions; // holds all the positions that sub occurs within str

            size_t pos = directionDesc.find("$", 0);
            while (pos != string::npos) {
                positions.push_back(pos);
                pos = directionDesc.find("$", pos + 1);
            }

            if (positions.size() > 1) {

                directionDesc = directionDesc.substr(1, directionDesc.size() - 2);

            } else {

                if (itr != mapdirectionStateDescs.end())
                    directionDesc = (itr->second) + " " + directionDesc;
            }

            return directionDesc;
        }

        static string
        GetElevatorEdgeDescription(Point pointA, Point pointB, long floorPlanID, string edgeName) {
            string elevatorDescription = "";
            auto customEdge = LJCPPBLAPIDAL::GetCustomEdge(pointA);

            if (floorPlanID <= 0)
                floorPlanID = LJCPPBLUtility::GetFloorIDFromPointIDentifier(pointB.Identifier);

            auto floorPan = LJCPPBLAPIDAL::GetFloorPlan(floorPlanID);

            if (customEdge == NULL) {
                customEdge = LJCPPBLAPIDAL::GetCustomEdge(pointB);
            }

            if (customEdge != NULL) {
                elevatorDescription = "Take " + customEdge->Name + " to " + floorPan->FloorName;
            } else if (floorPan != NULL) {
                elevatorDescription = "Take " + edgeName + " to " + floorPan->FloorName;
            }

            return elevatorDescription;
        }
    };
}
#endif
