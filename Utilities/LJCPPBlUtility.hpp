//
//  DijKastra.cpp
//  LJAdminTools
//
//  Created by User on 26/11/18.
//  Copyright © 2018 User. All rights reserved.
//

#ifndef LJCPPBLUtility_cpp
#define LJCPPBLUtility_cpp

#include "../Models/Models.hpp"


#include "../Plugins/JsonParser/json.hpp"
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

using namespace std;
using json = nlohmann::json;
using namespace LJCPPBL_Models;

namespace LJCPPBL_Utilities {

    class LJCPPBLUtility {
    public:
        static std::vector<std::string> TokeNize(std::string const &str, const char delim) {
            std::vector<std::string> out;
            size_t start;
            size_t end = 0;

            while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
                end = str.find(delim, start);
                out.push_back(str.substr(start, end - start));
            }

            return out;
        }

    public:
        static string
        ReplaceSubstring(string originalString, string wordTobeReplaced, string wordToReplace) {
            size_t index = originalString.find(wordTobeReplaced);

            while (index != string::npos) {
                originalString.replace(index, wordTobeReplaced.length(), wordToReplace);
                index = originalString.find(wordTobeReplaced);
            }

            return originalString;
        }

    public:
        static double GetAngleBetweenEdges(Edge edge1, Edge edge2) {
            double y2 = edge2.B->Y - edge2.A->Y;
            double x2 = edge2.B->X - edge2.A->X;
            double y1 = edge1.B->Y - edge1.A->Y;
            double x1 = edge1.B->X - edge1.A->X;

            auto finalAngle = atan2(y2, x2) - atan2(y1, x1);
            finalAngle = finalAngle * (180 / M_PI);

            return finalAngle;
        }

    public:
        static double GetAngleBetweenEdges(Point A, Point B, Point C) {
            double y2 = C.Y - B.Y;
            double x2 = C.X - B.X;
            double y1 = B.Y - A.Y;
            double x1 = B.X - A.X;

            auto finalAngle = atan2(y2, x2) - atan2(y1, x1);
            finalAngle = finalAngle * (180 / M_PI);

            return finalAngle;
        }

    public:
        static double GetDistanceBetweenPoints(Point pt1, Point pt2, float pixelRatio) {
            auto x_square = (pt1.X - pt2.X) * (pt1.X - pt2.X);
            auto y_square = (pt1.Y - pt2.Y) * (pt1.Y - pt2.Y);
            auto sum = x_square + y_square;
            return (sqrt(sum) * pixelRatio);
        }

    public:
        static long GetFloorIDFromPointIDentifier(string identifier) {
            return atoll(TokeNize(identifier, ':')[3].c_str());
        }

    public:
        static long GetVersionIDFromPointIDentifier(string identifier) {
            return atoll(TokeNize(identifier, ':')[0].c_str());
        }

    public:
        static unordered_map<long, FloorPlan> DeSerializeJsonToFloorPlans(string json) {
            unordered_map<long, FloorPlan> mapFloorPlans;
            auto jsonObj = json::parse(json);
            FloorPlan *floorPlan;

            for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
                if (itr.key() == "Data") {
                    for (json::iterator itr2 = itr->begin(); itr2 != itr->end(); ++itr2) {
                        if (itr2.key() == "MapFloorEdgeListViewModels") {
                            for (auto &element : *itr2) {
                                floorPlan = new FloorPlan();

                                for (json::iterator itr1 = element.begin();
                                     itr1 != element.end(); ++itr1) {
                                    if (itr1.key() == "FloorId") {
                                        if (itr1.value().is_number())
                                            floorPlan->FloorId = itr1.value().get<long>();
                                    } else if (itr1.key() == "FloorPlanId") {
                                        if (itr1.value().is_number())
                                            floorPlan->FloorPlanId = itr1.value().get<long>();
                                    } else if (itr1.key() == "IAFloorPlanId") {
                                        if (itr1.value().is_string())
                                            floorPlan->IAFloorPlanId = itr1.value().get<string>();
                                    } else if (itr1.key() == "IAFloorLevel") {
                                        if (itr1.value().is_number_integer())
                                            floorPlan->IAFloorLevel = itr1.value().get<int>();
                                    } else if (itr1.key() == "FloorName") {
                                        if (itr1.value().is_string())
                                            floorPlan->FloorName = itr1.value().get<string>();
                                    } else if (itr1.key() == "FloorOrder") {
                                        if (itr1.value().is_number_integer())
                                            floorPlan->FloorOrder = itr1.value().get<int>();
                                    } else if (itr1.key() == "MapUrl") {
                                        if (itr1.value().is_string())
                                            floorPlan->MapUrl = itr1.value().get<string>();
                                    } else if (itr1.key() == "Abbreviation") {
                                        if (itr1.value().is_string())
                                            floorPlan->Abbreviation = itr1.value().get<string>();
                                    }
                                }

                                mapFloorPlans.insert(
                                        pair<long, FloorPlan>(floorPlan->FloorPlanId, *floorPlan));
                            }
                        }
                    }
                }
            }

            return mapFloorPlans;
        }

    public:
        static unordered_map<int, CustomEdge> DeSerializeJsonToCustomEdges(string json) {
            unordered_map<int, CustomEdge> mapCustomEdges;
            auto jsonObj = json::parse(json);
            CustomEdge *customEdge;

            for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
                if (itr.key() == "Data") {
                    for (auto &element : *itr) {
                        customEdge = new CustomEdge();

                        for (json::iterator itr1 = element.begin(); itr1 != element.end(); ++itr1) {
                            if (itr1.key() == "Id") {
                                if (itr1.value().is_number())
                                    customEdge->Id = itr1.value().get<int>();
                            } else if (itr1.key() == "ClientEdgeTypeId") {
                                if (itr1.value().is_number())
                                    customEdge->ClientEdgeTypeId = itr1.value().get<int>();
                            } else if (itr1.key() == "IsUsed") {
                                if (itr1.value().is_boolean())
                                    customEdge->IsUsed = itr1.value().get<bool>();
                            } else if (itr1.key() == "Name") {
                                if (itr1.value().is_string())
                                    customEdge->Name = itr1.value().get<string>();
                            } else if (itr1.key() == "UniqueName") {
                                if (itr1.value().is_string())
                                    customEdge->UniqueName = itr1.value().get<string>();
                            }
                        }

                        mapCustomEdges.insert(pair<int, CustomEdge>(customEdge->Id, *customEdge));
                    }
                }
            }

            return mapCustomEdges;
        }

    public:
        static unordered_map<string, Edge> DeSerializeJsonToElevatorEdges(string json) {
            unordered_map<string, Edge> mapElevatorEdges;
            auto jsonObj = json::parse(json);
            Point *pointA = nullptr;
            Point *pointB = nullptr;
            string _name = "";
            float _distance = 0, _weight = 0, _duration = 0;
            int _startElevatorFloorId = 0, _endElevatorFloorId = 0, _currentFloorPlanId = 0;
            Edge *edgeAB;

            for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
                if (itr.key() == "Data") {
                    for (auto &element : *itr) {
                        for (json::iterator itr1 = element.begin(); itr1 != element.end(); ++itr1) {

                            if (itr1.key() == "FloorPlanId") {
                                _currentFloorPlanId = itr1.value().get<int>();
                            }
                            if (itr1.key() == "Edges") {
                                for (auto &itemEdge : *itr1) {
                                    _name = "";
                                    _distance = 0;
                                    _weight = 0;
                                    _duration = 0;
                                    _startElevatorFloorId = 0;
                                    _endElevatorFloorId = 0;

                                    for (json::iterator itr2 = itemEdge.begin();
                                         itr2 != itemEdge.end(); ++itr2) {
                                        if (itr2.key() == "A") {
                                            string identifier;
                                            float xCoOrdinate = 0, yCoordinate = 0;
                                            int customEdgeID = 0;

                                            for (json::iterator itr3 = itr2.value().begin();
                                                 itr3 != itr2.value().end(); ++itr3) {
                                                if (itr3.key() == "Identifier") {
                                                    if (itr3.value().is_string())
                                                        identifier = itr3.value().get<string>();
                                                } else if (itr3.key() == "x") {
                                                    if (itr3.value().is_number())
                                                        xCoOrdinate = itr3.value().get<float>();
                                                } else if (itr3.key() == "y") {
                                                    if (itr3.value().is_number())
                                                        yCoordinate = itr3.value().get<float>();
                                                } else if (itr3.key() == "MapCustomEdgeId") {
                                                    if (itr3.value().is_number())
                                                        customEdgeID = itr3.value().get<int>();
                                                }
                                            }

                                            pointA = new Point(identifier, xCoOrdinate, yCoordinate,
                                                               customEdgeID, _currentFloorPlanId);
                                        } else if (itr2.key() == "B") {
                                            string identifier;
                                            float xCoOrdinate = 0, yCoordinate = 0;
                                            int customEdgeID = 0;

                                            for (json::iterator itr3 = itr2.value().begin();
                                                 itr3 != itr2.value().end(); ++itr3) {
                                                if (itr3.key() == "Identifier") {
                                                    if (itr3.value().is_string())
                                                        identifier = itr3.value().get<string>();
                                                } else if (itr3.key() == "x") {
                                                    if (itr3.value().is_number())
                                                        xCoOrdinate = itr3.value().get<float>();
                                                } else if (itr3.key() == "y") {
                                                    if (itr3.value().is_number())
                                                        yCoordinate = itr3.value().get<float>();
                                                } else if (itr3.key() == "MapCustomEdgeId") {
                                                    if (itr3.value().is_number())
                                                        customEdgeID = itr3.value().get<int>();
                                                }
                                            }

                                            pointB = new Point(identifier, xCoOrdinate, yCoordinate,
                                                               customEdgeID, _currentFloorPlanId);
                                        } else if (itr2.key() == "Distance") {
                                            if (itr2.value().is_number())
                                                _distance = itr2.value().get<float>();
                                        } else if (itr2.key() == "Duration") {
                                            if (itr2.value().is_number_integer())
                                                _duration = itr2.value().get<int>();
                                        } else if (itr2.key() == "EndElevatorFloorId") {
                                            if (itr2.value().is_number_integer())
                                                _endElevatorFloorId = itr2.value().get<int>();
                                        } else if (itr2.key() == "StartElevatorFloorId") {
                                            if (itr2.value().is_number_integer())
                                                _startElevatorFloorId = itr2.value().get<int>();
                                        } else if (itr2.key() == "Weight") {
                                            if (itr2.value().is_number_integer())
                                                _weight = itr2.value().get<int>();

                                        } else if (itr2.key() == "Name") {
                                            if (itr2.value().is_string())
                                                _name = itr2.value().get<string>();

                                        }
                                    }

                                    edgeAB = new Edge(pointA, pointB, _name, _distance, _weight,
                                                      _duration, _startElevatorFloorId,
                                                      _endElevatorFloorId, _currentFloorPlanId,
                                                      pointA->MapCustomEdgeId);
                                    mapElevatorEdges.insert(pair<string, Edge>(
                                            (pointA->Identifier) + (pointB->Identifier), *edgeAB));
                                }
                            }
                        }
                    }
                }
            }

            return mapElevatorEdges;
        }

    public:
        static Graph DeSerializeJsonToGraph(string json) {
            Graph gph;
            auto jsonObj = json::parse(json);
            unordered_map<string, Point *> points;
            unordered_map<int, json::iterator> FloorWithData;
            string _name = "";
            string identifierA, identifierB;
            float xCoOrdinateA = 0, yCoordinateA = 0, xCoOrdinateB = 0, yCoordinateB = 0;
            int customEdgeIDA = 0, customEdgeIDB = 0;
            float _distance = 0, _weight = 0, _duration = 0;
            int _startElevatorFloorId = 0, _endElevatorFloorId = 0;
            Point *pointA = nullptr;
            Point *pointB = nullptr;
            NeighourNode *nNodeA = nullptr;
            NeighourNode *nNodeB = nullptr;
            unordered_map<string, Point *>::iterator itrPoint;
            unordered_map<string, Point *>::iterator itrPoint1;
            Edge *edgeAB = nullptr;

            for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
                if (itr.key() == "Data") {
                    for (json::iterator itr3 = itr->begin(); itr3 != itr->end(); ++itr3) {
                        if (itr3.key() == "MapFloorEdgeListViewModels") {
                            for (auto &element : *itr3) {
                                json::iterator curr_iterator;
                                int _currentFloorPlanId = 0;
                                for (json::iterator itr1 = element.begin();
                                     itr1 != element.end(); ++itr1) {


                                    if (itr1.key() == "FloorPlanId") {
                                        _currentFloorPlanId = itr1.value().get<int>();
                                    }
                                    if (itr1.key() == "Edges") {
                                        curr_iterator = itr1;
                                    }
                                }
                                FloorWithData.insert(pair<int, json::iterator>(_currentFloorPlanId,
                                                                               curr_iterator));
                            }
                        }
                    }
                }
            }

            for (auto &_FloorWithData : FloorWithData) {
                _name = "";
                identifierA = "";
                identifierB = "";
                xCoOrdinateA = 0;
                yCoordinateA = 0;
                xCoOrdinateB = 0;
                yCoordinateB = 0;
                customEdgeIDA = 0;
                customEdgeIDB = 0;
                _distance = 0;
                _weight = 0;
                _duration = 0;
                _startElevatorFloorId = 0;
                _endElevatorFloorId = 0;
                int _currentFloorPlanId = _FloorWithData.first;
                json::iterator LjsonObj = _FloorWithData.second;

                for (auto &itemEdge : *LjsonObj) {
                    for (json::iterator itr2 = itemEdge.begin(); itr2 != itemEdge.end(); ++itr2) {
                        if (itr2.key() == "A") {
                            for (json::iterator itr3 = itr2.value().begin();
                                 itr3 != itr2.value().end(); ++itr3) {
                                if (itr3.key() == "Identifier") {
                                    if (itr3.value().is_string())
                                        identifierA = itr3.value().get<string>();
                                } else if (itr3.key() == "x") {
                                    if (itr3.value().is_number())
                                        xCoOrdinateA = itr3.value().get<float>();
                                } else if (itr3.key() == "y") {
                                    if (itr3.value().is_number())
                                        yCoordinateA = itr3.value().get<float>();
                                } else if (itr3.key() == "MapCustomEdgeId") {
                                    if (itr3.value().is_number())
                                        customEdgeIDA = itr3.value().get<int>();
                                }
                            }
                        } else if (itr2.key() == "B") {
                            for (json::iterator itr3 = itr2.value().begin();
                                 itr3 != itr2.value().end(); ++itr3) {
                                if (itr3.key() == "Identifier") {
                                    if (itr3.value().is_string())
                                        identifierB = itr3.value().get<string>();
                                } else if (itr3.key() == "x") {
                                    if (itr3.value().is_number())
                                        xCoOrdinateB = itr3.value().get<float>();
                                } else if (itr3.key() == "y") {
                                    if (itr3.value().is_number())
                                        yCoordinateB = itr3.value().get<float>();
                                } else if (itr3.key() == "MapCustomEdgeId") {
                                    if (itr3.value().is_number())
                                        customEdgeIDB = itr3.value().get<int>();
                                }
                            }


                        } else if (itr2.key() == "Distance") {
                            if (itr2.value().is_number())
                                _distance = itr2.value().get<float>();
                        } else if (itr2.key() == "Duration") {
                            if (itr2.value().is_number_integer())
                                _duration = itr2.value().get<int>();
                        } else if (itr2.key() == "EndElevatorFloorId") {
                            if (itr2.value().is_number_integer())
                                _endElevatorFloorId = itr2.value().get<int>();
                        } else if (itr2.key() == "StartElevatorFloorId") {
                            if (itr2.value().is_number_integer())
                                _startElevatorFloorId = itr2.value().get<int>();
                        } else if (itr2.key() == "Weight") {
                            if (itr2.value().is_number_integer())
                                _weight = itr2.value().get<int>();

                        } else if (itr2.key() == "Name") {
                            if (itr2.value().is_string())
                                _name = itr2.value().get<string>();

                        }
                    }

                    pointA = new Point(identifierA, xCoOrdinateA, yCoordinateA, customEdgeIDA,
                                       _currentFloorPlanId);
                    pointB = new Point(identifierB, xCoOrdinateB, yCoordinateB, customEdgeIDB,
                                       _currentFloorPlanId);

                    nNodeA = new NeighourNode(pointB, _distance);
                    nNodeB = new NeighourNode(pointA, _distance);

                    itrPoint = points.find(pointA->Identifier);
                    //
                    if (gph.Points.size() == 0 ||
                        itrPoint == points.end()) {
                        pointA->add_neighourNode(*nNodeA);
                        gph.Points.push_back(pointA);
                        points.insert(pair<string, Point *>((pointA->Identifier), pointA));
                    } else {
                        (*itrPoint).second->add_neighourNode(*nNodeA);
                        pointA = (*itrPoint).second;
                    }

                    itrPoint1 = points.find(pointB->Identifier);

                    if (gph.Points.size() == 0 ||
                        itrPoint1 == points.end()) {
                        pointB->add_neighourNode(*nNodeB);
                        gph.Points.push_back(pointB);
                        points.insert(pair<string, Point *>((pointB->Identifier), pointB));
                    } else {
                        (*itrPoint1).second->add_neighourNode(*nNodeB);
                        pointB = (*itrPoint1).second;
                    }

                    edgeAB = new Edge(pointA, pointB, _name, _distance, _weight, _duration,
                                      _startElevatorFloorId, _endElevatorFloorId,
                                      _currentFloorPlanId, customEdgeIDA);
                    gph.Edges.push_back(edgeAB);
                }
            }

            return gph;
        }

    public:
        static float DeSerializeJsonToPixelToFeetRatio(string json) {
            auto jsonObj = json::parse(json);
            float pixelToFeetRatio = 0;

            for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
                if (itr.key() == "Data") {
                    for (json::iterator itr2 = itr->begin(); itr2 != itr->end(); ++itr2) {
                        if (itr2.key() == "PixelRatio") {
                            if (itr2.value().is_number())
                                pixelToFeetRatio = itr2.value().get<float>();
                        }
                    }
                }
            }

            return pixelToFeetRatio;
        }

    public:
        static unordered_map<long, Parking> DeSerializeJsonToParkings(string json) {
            auto jsonObj = json::parse(json);
            unordered_map<long, Parking> mapParkings;
            Parking *_parking = nullptr;

            for (auto &parkingDetail : jsonObj) {
                _parking = new Parking();

                for (json::iterator itr = parkingDetail.begin();
                     itr != parkingDetail.end(); ++itr) {
                    if (itr.key() == "LotId") {
                        if (itr.value().is_number())
                            _parking->LotId = itr.value().get<long>();
                    } else if (itr.key() == "ParkingType") {
                        if (itr.value().is_string())
                            _parking->ParkingType = itr.value().get<string>();
                    } else if (itr.key() == "LotName") {
                        if (itr.value().is_string())
                            _parking->LotName = itr.value().get<string>();
                    } else if (itr.key() == "TransportType") {
                        if (itr.value().is_string())
                            _parking->TransportType = itr.value().get<string>();
                    } else if (itr.key() == "AccessType") {
                        if (itr.value().is_string())
                            _parking->AccessType = itr.value().get<string>();
                    } else if (itr.key() == "ParkingQuickLinkId") {
                        if (itr.value().is_string())
                            _parking->ParkingQuickLinkId = itr.value().get<string>();
                    } else if (itr.key() == "QuickLink") {
                        if (itr.value().is_string())
                            _parking->QuickLink = itr.value().get<string>();
                    } else if (itr.key() == "MapDropOffPointId") {
                        _parking->MapDropOffPointId = itr.value().get<int>();
                    } else if (itr.key() == "PointGuid") {
                        if (itr.value().is_string())
                            _parking->PointGuid = itr.value().get<string>();
                    } else if (itr.key() == "DropOffPoint") {
                        if (itr.value().is_string())
                            _parking->DropOffPoint = itr.value().get<string>();
                        else
                            _parking->DropOffPoint = "";
                    }
                }

                mapParkings.insert(pair<long, Parking>(_parking->LotId, *_parking));
            }

            return mapParkings;
        }

    public:
        static unordered_map<string, DropOffPoint> DeSerializeJsonToDropOffPoints(string json) {
            auto jsonObj = json::parse(json);
            unordered_map<string, DropOffPoint> mapDropOffPoints;
            DropOffPoint *_dropOffPoint = nullptr;

            for (auto &dropOffPoint : jsonObj) {
                _dropOffPoint = new DropOffPoint();

                for (json::iterator itr = dropOffPoint.begin(); itr != dropOffPoint.end(); ++itr) {
                    if (itr.key() == "Id") {
                        if (itr.value().is_number())
                            _dropOffPoint->Id = itr.value().get<long>();
                    } else if (itr.key() == "DropOffPoint") {
                        if (itr.value().is_string())
                            _dropOffPoint->DropOffPoint = itr.value().get<string>();
                    } else if (itr.key() == "QuickLinkId") {
                        if (itr.value().is_string())
                            _dropOffPoint->QuickLinkId = itr.value().get<string>();
                    } else if (itr.key() == "PointGuid") {
                        if (itr.value().is_string())
                            _dropOffPoint->PointGuid = itr.value().get<string>();
                    }
                }

                mapDropOffPoints.insert(
                        pair<string, DropOffPoint>(_dropOffPoint->DropOffPoint, *_dropOffPoint));
            }

            return mapDropOffPoints;
        }

    public:
        static unordered_map<string, Destination> DeSerializeJsonToDestinations(string json) {
            auto jsonObj = json::parse(json);
            unordered_map<string, Destination> mapDestinations;
            unordered_map<string, Destination> mapDestinationsNew;

            Destination *ds = nullptr;
            for (json::iterator itr = jsonObj.begin(); itr != jsonObj.end(); ++itr) {
                if (itr.key() == "DestinationList") {
                    for (auto &element : *itr) {
                        ds = new Destination();

                        for (json::iterator itr1 = element.begin(); itr1 != element.end(); ++itr1) {
                            if (itr1.key() == "id") {
                                if (itr1.value().is_number_integer())
                                    ds->ID = itr1.value().get<int>();

                            } else if (itr1.key() == "destinationtypeid") {
                                if (itr1.value().is_number_integer())
                                    ds->DestinationTypeId = itr1.value().get<int>();
                            } else if (itr1.key() == "destinationname") {
                                if (itr1.value().is_string())
                                    ds->DestinationName = itr1.value().get<string>();
                            } else if (itr1.key() == "quicklink") {
                                if (itr1.value().is_string())
                                    ds->QuickLink = itr1.value().get<string>();
                            } else if (itr1.key() == "VisibleColumnNames") {
                                if (itr1.value().is_string())
                                    ds->VisibleColumnNames = itr1.value().get<string>();
                            } else if (itr1.key() == "pointguid") {
                                if (itr1.value().is_string())
                                    ds->PointGuid = itr1.value().get<string>();
                            } else if (itr1.key() == "x") {
                                if (itr1.value().is_number())
                                    ds->X = itr1.value().get<float>();
                            } else if (itr1.key() == "y") {
                                if (itr1.value().is_number())
                                    ds->Y = itr1.value().get<float>();
                            } else if (itr1.key() == "floorplanid") {
                                if (itr1.value().is_number_integer())
                                    ds->FloorPlanId = itr1.value().get<int>();
                            } else if (itr1.key() == "destinationDetails") {

                                json::iterator curr_iterator;
                                // for (auto &element1 : *itr1) {
                                curr_iterator = itr1;

                                for (json::iterator itr2 = curr_iterator->begin();
                                     itr2 != curr_iterator->end(); ++itr2) {

                                    if (itr2.key() == "Tags") {
                                        if (itr2.value().is_string())
                                            ds->tags = itr2.value().get<string>();
                                    }

                                }
                                // }
                            }
                        }
                        mapDestinationsNew.insert(
                                pair<string, Destination>(to_string(ds->ID), *ds)); // QuickLink

                        mapDestinations.insert(
                                pair<string, Destination>(ds->QuickLink, *ds)); // QuickLink
                    }
                }
            }
            LJCPPBLGlobalValues->MapDestinationsNew=mapDestinationsNew;// = mapDestinationsNew.get();
            return mapDestinations;
        }


    };
}
#endif



