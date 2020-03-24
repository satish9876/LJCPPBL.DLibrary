//
//  Projection.cpp
//  LJCPPBL.Console.Test
//
//  Created by User on 14/03/19.
//  Copyright © 2019 User. All rights reserved.
//

#ifndef Projection_cpp
#define Projection_cpp

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
#include <vector>

using namespace std;
using json = nlohmann::json;
using namespace LJCPPBL_Models;
using namespace LJCPPBL_Utilities;
using namespace EnumParkingOrder;

namespace LJCPPBL_CORE {
    class Projection
    {
        public: static Point ProjectPoint(Point actualPoint, Graph spcGraph)
        {
            float pixelToFeetRatio = LJCPPBLGlobalValues -> PixelToFeetRatio;
            Point startPoint;
            Point endPoint;
            list<Edge*> lstEdges;
            auto distanceDiff = -1;
            auto dist_StartPoint = 0;
            auto dist_EndPoint = 0;
            bool foundTheEdgeOnWhichPointExistsIs = false;
            Point pt;
            
            for (Edge* & edge : spcGraph.Edges)
            {
                if(edge -> A -> Identifier == actualPoint.Identifier || edge -> B -> Identifier == actualPoint.Identifier)
                   lstEdges.push_back(edge);
            }
            
            for (Edge* & edge : lstEdges)
            {
                if(edge -> CustomEdgeID > 0 && actualPoint.  xx)
                    continue;
                
                auto dist_start_actual = (float)sqrt((edge -> A -> X - actualPoint.X) * (edge -> A -> X- actualPoint.X) +
                                                      (edge -> A -> Y - actualPoint.Y) * (edge -> A -> Y - actualPoint.Y));
                auto dist_end_actual = (float)sqrt((edge -> B -> X - actualPoint.X) * (edge -> B -> X - actualPoint.X)
                                                     + (edge -> B -> Y - actualPoint.Y) * (edge -> B -> Y - actualPoint.Y));
                auto dist_start_end = (float)sqrt((edge -> B -> X - edge -> A -> X) * (edge -> B -> X - edge -> A -> X)
                                                      + (edge -> B -> Y - edge -> A -> Y) * (edge -> B -> Y - edge -> A -> Y));
                
                if (abs((dist_start_actual + dist_end_actual) - dist_start_end) <= (5 *pixelToFeetRatio))
                {
                    //We are very near the edge and this is a given
                    startPoint = *(edge -> A);
                    endPoint = *(edge -> B);
                    dist_StartPoint = dist_start_actual;
                    dist_EndPoint = dist_end_actual;
                    foundTheEdgeOnWhichPointExistsIs = true;
                    break;
                }
            }
            
            if(foundTheEdgeOnWhichPointExistsIs != true)
            {
                for (Edge* & edge : lstEdges)
                {
                    if(edge -> CustomEdgeID > 0)
                        continue;
                    
                    auto dist_start_actual = (float)sqrt((edge -> A -> X - actualPoint.X) * (edge -> A -> X- actualPoint.X) +
                                                         (edge -> A -> Y - actualPoint.Y) * (edge -> A -> Y - actualPoint.Y));
                    auto dist_end_actual = (float)sqrt((edge -> B -> X - actualPoint.X) * (edge -> B -> X - actualPoint.X)
                                                       + (edge -> B -> Y - actualPoint.Y) * (edge -> B -> Y - actualPoint.Y));
                    auto dist_start_end = (float)sqrt((edge -> B -> X - edge -> A -> X) * (edge -> B -> X - edge -> A -> X)
                                                      + (edge -> B -> Y - edge -> A -> Y) * (edge -> B -> Y - edge -> A -> Y));
                    
                    if (distanceDiff >= 0)
                    {
                        if (abs((dist_start_actual + dist_end_actual) - dist_start_end) < distanceDiff)
                        {
                            distanceDiff = abs((dist_start_actual + dist_end_actual) - dist_start_end);
                            startPoint = *(edge -> A);
                            endPoint = *(edge -> B);
                            dist_StartPoint = dist_start_actual;
                            dist_EndPoint = dist_end_actual;
                        }
                    }
                    else
                    {
                        distanceDiff = abs((dist_start_actual + dist_end_actual) - dist_start_end);
                        startPoint = *(edge -> A);
                        endPoint = *(edge -> B);
                        dist_StartPoint = dist_start_actual;
                        dist_EndPoint = dist_end_actual;
                    }
                }
            }
            
            auto pointOnLine = GetPointOnLine(startPoint, endPoint, actualPoint);
            
            if (pointOnLine.Identifier != "")
            {
                string newPointIdentity = to_string(LJCPPBLUtility::GetVersionIDFromPointIDentifier(startPoint.Identifier))
                + ":" + to_string(pointOnLine.X) + ":" + to_string(pointOnLine.Y) + ":"
                + to_string(LJCPPBLUtility::GetFloorIDFromPointIDentifier(startPoint.Identifier));
                pointOnLine.Identifier = newPointIdentity;
//                Edge edgeAB(&pointA, &pointB, _name, _distance, _weight, _duration, _startElevatorFloorId, _endElevatorFloorId);
                Edge edge1(&startPoint, &pointOnLine, "", dist_StartPoint, 1, 0, 0, 0);
                Edge edge2(&pointOnLine, &endPoint, "", dist_EndPoint, 1, 0, 0, 0);
                map<string, Edge>::iterator itr = spcGraph.MapGraphEdges.find((startPoint.Identifier + endPoint.Identifier));
                
                if(itr != spcGraph.MapGraphEdges.end())
                {
                    spcGraph.MapGraphEdges.erase((startPoint.Identifier + endPoint.Identifier));
//                    Edge* edge = &(itr -> second);
//                    spcGraph.Edges.erase(edge);
                }
                
                map<string, Point>::iterator itr1 = spcGraph.MapGraphPoints.find(pointOnLine.Identifier);
                
                if(itr1 == spcGraph.MapGraphPoints.end())
                {
                    spcGraph.Points.push_back(&pointOnLine);
                }
                
                spcGraph.Edges.push_back(&edge1);
                spcGraph.Edges.push_back(&edge2);
                return pointOnLine;
            }
            else
            {
                //There is no possible projection from this point to any edge so take the nearest point
                return pt;
            }
        }
        
        public: static Point GetPointOnLine(Point startPoint, Point endPoint, Point actualPoint)
        {
            bool isValid = false;
            Point pt;
            Point pointOnLine("infinity", 0, 0);
            if (startPoint.X == endPoint.X && startPoint.X == endPoint.Y)
                startPoint.X -= 0.0001f;
            
            float U = ((actualPoint.X - startPoint.X) * (endPoint.X - startPoint.X)
                       + (actualPoint.Y - startPoint.Y) * (endPoint.Y - startPoint.Y))
                        / (float)(pow((endPoint.X - startPoint.X), 2)
                                  + pow((endPoint.Y - startPoint.Y), 2));
            
            pointOnLine.X = startPoint.X + (U * (endPoint.X - startPoint.X));
            pointOnLine.Y = startPoint.Y + (U * (endPoint.Y - startPoint.Y));
            auto minx = min(startPoint.X, endPoint.X);
            auto maxx = max(startPoint.X, endPoint.X);
            auto miny = min(startPoint.Y, endPoint.Y);
            auto maxy = max(startPoint.Y, endPoint.Y);
            isValid = (pointOnLine.X >= minx && pointOnLine.X <= maxx) && (pointOnLine.Y >= miny && pointOnLine.Y <= maxy);
            
            return isValid ? pointOnLine : pt;
        }
    };
}
#endif
