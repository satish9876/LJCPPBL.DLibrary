//
//  DijKastra.cpp
//  LJAdminTools
//
//  Created by User on 26/11/18.
//  Copyright © 2018 User. All rights reserved.
//
#ifndef SPC_cpp
#define SPC_cpp
#define  LOG_TAG    "testjni"
#define  ALOG( ...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#include "../Models/Models.hpp"
#include "../DAL/DataAccess.hpp"
#include "GlobalValues.hpp"
#include "../Utilities/LJCPPBlUtility.hpp"
#include <list>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <set>
//#include <android/log.h>

using namespace std;
using namespace LJCPPBL_Models;
using namespace LJCPPBL_Utilities;
using namespace LJCPPBL_DAL;

namespace LJCPPBL_CORE {
    
    class SPC
    {
        Graph spcGraph;
        list<Point> lstMapPoints;
        unordered_map<string, float> mapPointDistanceDictionary;
        
    private: list<Point>  FindShortestPath(string source, string destination, Graph g, float* costOfPath)
        {
            Point pt;
            int currentFloorID = 0;
            lstMapPoints.clear();
            spcGraph = g;
            unordered_map<string, Point> mapPreviousPoints;
            mapPointDistanceDictionary.clear();
            list<Point> lstShortestPathPoints;
            Point targetNode;
            Point sourceNode;
            Point currentPoint;
            Point* mappoint;
            float pointDistance = numeric_limits<float>::max();
            
            try{
                for (Point* & point : g.Points)
                {
                    mapPreviousPoints.insert(pair<string, Point>(point -> Identifier, pt));
                    mapPointDistanceDictionary.insert(pair<string, float>(point -> Identifier, numeric_limits<float>::max()));
                    mappoint = new Point(point -> Identifier, point -> X, point -> Y, point -> MapCustomEdgeId, point -> FloorPlanId);
                    
                    for (NeighourNode & neighournode : point -> NeighourNodes)
                    {
                        mappoint -> NeighourNodes.push_back(neighournode);
                    }
                    
                    lstMapPoints.push_back(*mappoint);
                    
                    if(point -> Identifier == destination)
                        targetNode = *point;
                    else if(point -> Identifier == source)
                    {
                        sourceNode = *point;
                    }
                }
                
                auto ptrSource = mapPointDistanceDictionary.find(source);
                ptrSource->second = 0;
                
                while (lstMapPoints.size() > 0)
                {
                    currentPoint = GetPointWithSmallestDist(&pointDistance);
                    long sourceFloorId = LJCPPBLUtility::GetFloorIDFromPointIDentifier(currentPoint.Identifier);
                    
                    if(currentFloorID != sourceFloorId)
                    {
                        if(currentPoint.IsElevator)
                        {
                            auto reverseEdgeRange = g.EndElevatorEdges.equal_range(sourceFloorId);
                            auto forwardEdgeRange = g.StartElevatorEdges.equal_range(sourceFloorId);
                            
                            for (multimap<long, Edge>::iterator itr = reverseEdgeRange.first; itr != reverseEdgeRange.second; ++itr)
                            {
                                if((itr -> second).CustomEdgeID == currentPoint.MapCustomEdgeId)
                                {
                                    for (multimap<long, Edge>::iterator itr1 = forwardEdgeRange.first; itr != forwardEdgeRange.second; ++itr)
                                    {
                                        if((itr1 -> second).CustomEdgeID == currentPoint.MapCustomEdgeId)
                                        {
                                            if((itr -> second).A -> Identifier == (itr1 -> second).B -> Identifier
                                               && (itr -> second).B -> Identifier == (itr1 -> second).A -> Identifier )
                                            {
                                                (itr -> second).Distance = (itr1 -> second).Distance;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    currentFloorID = sourceFloorId;
                    if (pointDistance == numeric_limits<float>::max() || currentPoint.Identifier == "")
                        break;
                    
                    //Remove the first observed point since we are currently visiting it and no need for it in the list
                    lstMapPoints.remove(currentPoint);
                    
                    //if the point is also target, we have arrived and we can break from the loop
                    if (currentPoint.Identifier == destination)
                        break;
                    
                    //iterate through the points and find their edges and find the distances of each
                    for (NeighourNode & neighournode : currentPoint.NeighourNodes)
                    {
                        auto ptr = mapPointDistanceDictionary.find(neighournode.NeighourPoint -> Identifier);
                        
                        if (ptr != mapPointDistanceDictionary.end()){
                            float pathLength = pointDistance + neighournode.Distance;
                            float neighourPointDistance = ptr->second;
                            
                            if(pathLength < neighourPointDistance){
                                ptr->second = pathLength;
                                auto ptr1 = mapPreviousPoints.find(neighournode.NeighourPoint -> Identifier);
                                ptr1->second = currentPoint;
                            }
                        }
                    }
                }
                
                while (mapPreviousPoints.find(targetNode.Identifier)->second != pt)
                {
                    if(lstShortestPathPoints.size() <= 0)
                        lstShortestPathPoints.push_back(targetNode);
                    else
                        lstShortestPathPoints.insert(lstShortestPathPoints.begin(), targetNode);
                    
                    auto ptr = mapPreviousPoints.find(targetNode.Identifier);
                    targetNode = ptr->second;
                }
                
                auto ptr = mapPointDistanceDictionary.find(destination);
                
                if(costOfPath != NULL){
                    *costOfPath = ptr->second;
                }
                
                lstShortestPathPoints.insert(lstShortestPathPoints.begin(), sourceNode);
                lstMapPoints.clear();
                //ALOG("directions from cpp SPC completed");
                return lstShortestPathPoints;
                
            }
            catch(exception& ex){
               // ALOG("Exception from cpp SPC %s.", ex.what());
            }
            return lstShortestPathPoints;
            
        }
        
    public: list<Point> GetShortestPath(string source, string destination, float* costOfPath){
        auto graph = LJCPPBLAPIDAL::GetGraph();
        return FindShortestPath(source, destination, graph, costOfPath);
    }
        
    public: list<Point> GetShortestPathByXYF( float x, float y, float floorid, string destination,  float* costOfPath){
        auto graph = LJCPPBLAPIDAL::GetGraph();
        list<Point> emptyList;
        auto newPoint = new Point("infinity", x, y, 0, floorid);
        auto actualPoint = ProjectPoint(*newPoint, graph);
        LJCPPBLAPIDAL::SetGraph(graph);
        return actualPoint.Identifier == "" ? emptyList : FindShortestPath(actualPoint.Identifier, destination, graph, costOfPath);
    }
        
    public: list<Point> GetShortestPathByXYXY( float x1, float y1, float x2, float y2, float floorid1, float floorid2,  float* costOfPath){
        auto graph = LJCPPBLAPIDAL::GetGraph();
        list<Point> emptyList;
        auto newPointS = new Point("infinity", x1, y1, 0, floorid1);
        auto actualPointS = ProjectPoint(*newPointS, graph);
        auto newPointD = new Point("infinity", x2, y2, 0, floorid2);
        auto actualPointD = ProjectPoint(*newPointD, graph);
        LJCPPBLAPIDAL::SetGraph(graph);
        return (actualPointS.Identifier == "" || actualPointD.Identifier == "")  ? emptyList
        : FindShortestPath(actualPointS.Identifier, actualPointD.Identifier, graph, costOfPath);
    }

        
    private: Point GetPointWithSmallestDist(float* pointDistance)
        {
            Point minNode;

            if(lstMapPoints.size() > 1){
                float minDist = numeric_limits<float>::max();


                for (Point & point : lstMapPoints)
                {
                    auto ptr = mapPointDistanceDictionary.find(point.Identifier);
                    float pointMinDistance = ptr->second;

                    if(pointMinDistance < minDist){
                        minDist = pointMinDistance;
                        minNode = point;
                        *pointDistance = pointMinDistance;
                    }
                }
            }
            else if(lstMapPoints.size() == 1){
                list<Point> :: iterator itr = lstMapPoints.begin();
                minNode = *itr;
            }

            return minNode;
        }
        

        
    private: bool ComparePaths(list<Point> path1, list<Point> path2){
        bool isPathsEqual = true;
        
        if(path1.size() == path2.size()){
            for (int i = 0; i < path1.size(); i++) {
                list<Point>::iterator itr = path1.begin();
                advance(itr, i);
                list<Point>::iterator itr1 = path2.begin();
                advance(itr1, i);
                
                if(itr->Identifier != itr1->Identifier)
                    isPathsEqual = false;
            }
        }
        else{
            isPathsEqual = false;
        }
        
        return isPathsEqual;
    }
        
    public: static Point ProjectPoint(Point actualPoint, Graph& spcGraph)
        {
            float pixelToFeetRatio = LJCPPBLGlobalValues -> PixelToFeetRatio;
            Point* startPoint = NULL;
            Point* endPoint = NULL;
            list<Edge*> lstEdges;
            auto distanceDiff = -1;
            auto dist_StartPoint = 0;
            auto dist_EndPoint = 0;
//            bool foundTheEdgeOnWhichPointExistsIs = false;
            Point pt;

            
//            if(foundTheEdgeOnWhichPointExistsIs != true)
//            {
                for (Edge* & edge : spcGraph.Edges)
                {
                    if(edge -> CustomEdgeID <= 0 && actualPoint.FloorPlanId == edge -> FloorPlanId){
                        
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
                            startPoint = edge -> A;
                            endPoint = edge -> B;
                            dist_StartPoint = dist_start_actual;
                            dist_EndPoint = dist_end_actual;
                        }
                    }
                    else
                    {
                        distanceDiff = abs((dist_start_actual + dist_end_actual) - dist_start_end);
                        startPoint = edge -> A;
                        endPoint = edge -> B;
                        dist_StartPoint = dist_start_actual;
                        dist_EndPoint = dist_end_actual;
                    }
                  }
                }
//            }
            
            if(startPoint == NULL || endPoint == NULL)
            {
                return pt;
            }
                
            auto pointOnLinex = GetPointOnLine(*startPoint, *endPoint, actualPoint);
            
            if (pointOnLinex.Identifier != "")
            {
                string newPointIdentity = to_string(LJCPPBLUtility::GetVersionIDFromPointIDentifier(startPoint->Identifier))
                + ":" + to_string(pointOnLinex.X) + ":" + to_string(pointOnLinex.Y) + ":"
                + to_string(LJCPPBLUtility::GetFloorIDFromPointIDentifier(startPoint->Identifier));
                pointOnLinex.Identifier = newPointIdentity;
                
                auto dist_start_actual = (float)sqrt((startPoint->X - pointOnLinex.X) * (startPoint->X- pointOnLinex.X) + (startPoint->Y - pointOnLinex.Y) * (startPoint->Y - pointOnLinex.Y));
                auto dist_end_actual = (float)sqrt((endPoint->X - pointOnLinex.X) * (endPoint->X- pointOnLinex.X) + (endPoint->Y - pointOnLinex.Y) * (endPoint->Y - pointOnLinex.Y));
                
                
                //NewNode Neighbors
                NeighourNode* nNodeNewPointToA= new NeighourNode(startPoint, dist_start_actual);
                NeighourNode* nNodeNewPointToB= new NeighourNode(endPoint, dist_end_actual);
                
                Point* pointOnLine = new Point(pointOnLinex.Identifier, pointOnLinex.X, pointOnLinex.Y, pointOnLinex.MapCustomEdgeId, pointOnLinex.FloorPlanId);
                
                pointOnLine->add_neighourNode(*nNodeNewPointToA);
                pointOnLine->add_neighourNode(*nNodeNewPointToB);
               
                spcGraph.Points.push_back(pointOnLine);
                
                NeighourNode* nNodeAToNewPoint= new NeighourNode(pointOnLine, dist_start_actual);
                NeighourNode* nNodeBToNewPoint= new NeighourNode(pointOnLine, dist_end_actual);
                
                startPoint->add_neighourNode(*nNodeAToNewPoint);
                endPoint->add_neighourNode(*nNodeBToNewPoint);
                
                
    
                Edge* edge1 = new Edge(startPoint, pointOnLine, "", dist_start_actual, 1, 0, 0, 0, actualPoint.FloorPlanId, 0);
                Edge* edge2 = new Edge(pointOnLine, endPoint, "", dist_end_actual, 1, 0, 0, 0, actualPoint.FloorPlanId, 0);
                
                spcGraph.Edges.push_back(edge1);
                spcGraph.Edges.push_back(edge2);
                
                return *pointOnLine;
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
            Point pointOnLine("infinity", 0, 0, 0, startPoint.FloorPlanId);
            
            
            if (startPoint.X == endPoint.X && startPoint.Y == endPoint.Y)
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
            
            return isValid ? pointOnLine : actualPoint;
        }
    };


    
}
#endif


/*---------------------------------------Seudo Algo To Find K-Shortest Path From Source Node To Target Node---------------------------------------------------------------------------------------
 
 function K-ShortestPath(Graph, source, destination, K):
 // Determine the shortest path from the source to the destination.
 A[0] = Dijkstra(Graph, source, destination);
 // Initialize the set to store the potential kth shortest path.
 B = [];
 
 for k from 1 to K:
 // The spur node ranges from the first node to the next to last node in the previous k-shortest path.
 for i from 0 to size(A[k − 1]) − 2:
 
 // Spur node is retrieved from the previous k-shortest path, k − 1.
 spurNode = A[k-1].node(i);
 // The sequence of nodes from the source to the spur node of the previous k-shortest path.
 rootPath = A[k-1].nodes(0, i);
 
 for each path p in A:
 if rootPath == p.nodes(0, i):
 // Remove the links that are part of the previous shortest paths which share the same root path.
 remove p.edge(i,i + 1) from Graph;
 
 for each node rootPathNode in rootPath except spurNode:
 remove rootPathNode from Graph;
 
 // Calculate the spur path from the spur node to the destination.
 spurPath = Dijkstra(Graph, spurNode, destination);
 
 // Entire path is made up of the root path and spur path.
 totalPath = rootPath + spurPath;
 // Add the potential k-shortest path to the heap.
 B.append(totalPath);
 
 // Add back the edges and nodes that were removed from the graph.
 restore edges to Graph;
 restore nodes in rootPath to Graph;
 
 if B is empty:
 // This handles the case of there being no spur paths, or no spur paths left.
 // This could happen if the spur paths have already been exhausted (added to A),
 // or there are no spur paths at all - such as when both the source and destination vertices
 // lie along a "dead end".
 break;
 // Sort the potential k-shortest paths by cost.
 B.sort();
 // Add the lowest cost path becomes the k-shortest path.
 A[k] = B[0];
 B.pop();
 
 return A;
 
 --------------------------------------------------------------------------END-----------------------------------------------------------------------------------------------------------------
 */




