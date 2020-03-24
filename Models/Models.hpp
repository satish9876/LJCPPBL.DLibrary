//
//  Models.hpp
//  LJAdminTools
//
//  Created by User on 27/11/18.
//  Copyright © 2018 User. All rights reserved.
//

#ifndef Models_hpp
#define Models_hpp

#include <iostream>
#include <list>
#include <map>
#include <vector>

using namespace std;

namespace EnumParkingOrder {
    enum ParkingOrder {
        None = 0,
        First = 1,
        Last = 2
    };
}

namespace PointStates {
    enum PointStates {
        Unactive = 0,
        Active = 1,
        Neighbour = 2,
        Selected = 3,
        Chosen = 4
    };
}

namespace EdgeStates {
    enum EdgeStates {
        Unactive = 0,
        Active = 1,
        Selected = 2,
        Chosen = 3
    };
}

namespace LJCPPBL_Models {


    template<class T, class U>
    class KeyValuePair {
    public:
        T Key;
    public:
        U Value;

    public:
        KeyValuePair() {}

    public:
        KeyValuePair(T _key, U _value) {
            Key = _key;
            Value = _value;
        }
    };


    class NeighourNode;

    class Point {
    public:
        bool operator==(const Point &s) const { return Identifier == s.Identifier; };
    public:
        bool operator!=(const Point &s) const { return !operator==(s); }

    public:
        bool operator==(string const &s) const {
            if (this->Identifier.compare(s) == 0)
                return true;
            else
                return false;

        };

    public:
        string Identifier;
    public:
        float X;
    public:
        float Y;
    public:
        int FloorPlanId;
    public:
        bool IsElevator;
    public :
        float Cost;
    public:
        int MapCustomEdgeId;
    public:
        vector<NeighourNode> NeighourNodes;

    public:
        Point() {}

    public:
        Point(string _identifier, float x, float y) {
            Identifier = _identifier;
            X = x;
            Y = y;
            IsElevator = false;
        }

    public:
        Point(string _identifier, float x, float y, int customEdgeID, int floorPlanId) {
            Identifier = _identifier;
            X = x;
            Y = y;
            FloorPlanId = floorPlanId;
            MapCustomEdgeId = customEdgeID;
            IsElevator = customEdgeID > 0;
            NeighourNodes = {};
        }

    public:
        Point(string identifier, float x, float y, bool isElevator) {
            Identifier = identifier;
            X = x;
            Y = y;
            IsElevator = isElevator;
        }

    public:
        void add_neighourNode(NeighourNode &node) {
            NeighourNodes.push_back(node);
        }
    };

    class NeighourNode {
    public:
        Point *NeighourPoint;
    public:
        float Distance;

    public:
        NeighourNode() {}

    public:
        NeighourNode(Point *point, float _distance) {
            NeighourPoint = point;
            Distance = _distance;
        }
    };

    class Edge {

    public:
        bool operator==(const Edge &s) const {
            return A->Identifier + "_" + B->Identifier == s.A->Identifier + "_" + s.B->Identifier;
        };
    public:
        bool operator!=(const Edge &s) const { return !operator==(s); }

    public:
        Point *A;
    public:
        Point *B;
    public:
        std::vector<pair<Point *, Point *> *> AtoB;
    public:
        float Distance;
    public:
        float Weight;
    public:
        float Duration;
    public:
        int FloorPlanId;
    public:
        int StartElevatorFloorId;
    public:
        int EndElevatorFloorId;
    public:
        long CustomEdgeID;
    public:
        bool Selected = false;
    public:
        string Name;

    public:
        Edge() {}

    public:
        Edge(Point *a, Point *b, string edgeName, float distance, float weight, float duration,
             int StartElevatorFloorId, int EndElevatorFloorId, int floorPlanId, int _customEdgeID) {
            A = a;
            B = b;
            FloorPlanId = floorPlanId;
            Name = edgeName;
            Distance = distance;
            Weight = weight;
            Duration = duration;
            CustomEdgeID = _customEdgeID;
            StartElevatorFloorId = StartElevatorFloorId;
            EndElevatorFloorId = EndElevatorFloorId;
        }
    };

    class Graph {
    public:
        vector<Point *> Points;
    public:
        vector<Edge *> Edges;
//    public: multimap<string, Edge> AdjacentEdgesA;
//    public: multimap<string, Edge> AdjacentEdgesB;
//    public: multimap<string, NeighourNode> NeighourNodes;
    public:
        multimap<long, Edge> StartElevatorEdges;
    public:
        multimap<long, Edge> EndElevatorEdges;
    public:
        map<string, Edge> MapGraphEdges;
    public:
        map<string, Point> MapGraphPoints;

        //Constructors
    public:
        Graph() {}

        //pre: nodes & edges contain no duplicates
    public:
        Graph(vector<Point *> points, vector<Edge *> edges) {
            Points = points;
            Edges = edges;
        }

    public:
        Graph(vector<Point *> points, vector<Edge *> edges, multimap<long, Edge> startElevatorEdges,
              multimap<long, Edge> endElevatorEdges, map<string, Edge> mapGraphEdges,
              map<string, Point> mapGraphPoints) {
            Points = points;
            Edges = edges;
            StartElevatorEdges = startElevatorEdges;
            EndElevatorEdges = endElevatorEdges;
            MapGraphEdges = mapGraphEdges;
            MapGraphPoints = mapGraphPoints;
        }

        //Public methods
    public:
        void AddPoint(Point *n) {
            Points.push_back(n);
        }

    public:
        void AddEdge(Edge *e) {
            //check the edge doesn't alreay exist.
            //        if (!Edges.Contains(e, new EdgeComparer()))
            //        {
            Edges.push_back(e);
            //        }
        }
    };

    class Destination {
    public:
        bool operator==(const Destination &s) const { return QuickLink == s.QuickLink; };
    public:
        bool operator!=(const Destination &s) const { return !operator==(s); }

    public:
        int ID;
    public:
        int DestinationTypeId;
    public:
        string DestinationName;
    public:
        string QuickLink;
    public:
        string VisibleColumnNames;
    public:
        string PointGuid;
    public:
        float X;
    public:
        float Y;
    public:
        int FloorPlanId;
    public:
        string tags;
    };

    class FloorPlan {
    public:
        long FloorId;
    public:
        long FloorPlanId;
    public:
        string IAFloorPlanId;
    public:
        int IAFloorLevel;
    public:
        int FloorOrder;
    public:
        string FloorName;
    public:
        string MapUrl;
    public:
        string Abbreviation;
    };

    class ShortestPathFloor {
    public:
        string Title;
    public:
        string Url;
    public:
        string Description;
    public:
        string PathPoints;
    public:
        long FloorId;
    };

    class WalkingDirection {
    public:
        float Distance;

    public:
        string Direction;
    public:
        string Description;
    public:
        string DescriptionMtr;
    public:
        string Path;
    };

    class FloorDirectionDetail {
    public:
        string Title;
    public:
        string Url;
    public:
        string Description;
    public:
        string Pathpoints;
    public:
        long FloorID;
    };

    class Direction {
    public:
        multimap<string, vector<FloorDirectionDetail>> ShortestPath;
    public:
        multimap<string, vector<WalkingDirection>> WalkingDirections;
    public:
        string ShortNote;
    public:
        string Note;
    };

    class Parking {
    public:
        long LotId;
    public:
        string ParkingType;
    public:
        string LotName;
    public:
        string TransportType;
    public:
        string AccessType;
    public:
        string ParkingQuickLinkId;
    public:
        string QuickLink;
    public:
        int MapDropOffPointId;
    public:
        string PointGuid;
    public:
        string DropOffPoint;
    };

    class DropOffPoint {
    public:
        long Id;
    public:
        string DropOffPoint;
    public:
        string QuickLinkId;
    public:
        string PointGuid;
    };

    class CustomEdge {
    public:
        int Id;
    public:
        int ClientEdgeTypeId;
    public:
        bool IsUsed;
    public:
        string Name;
    public:
        string UniqueName;
    };

    class LJCPPBLCache {
    public:
        string MapPointsAndPathJSON;
    public:
        string MapDestinationJSON;

    public:
        string ParkingLotDetailsJSON;
    public:
        string DropOffPointsJSON;
    public:
        string ElevatorEdgesJsSON;
    public:
        string CustomEdgesJsSON;

    public:
        LJCPPBLCache() {
            MapPointsAndPathJSON = "";
            MapDestinationJSON = "";
            ParkingLotDetailsJSON = "";
            DropOffPointsJSON = "";
            ElevatorEdgesJsSON = "";
            CustomEdgesJsSON = "";
        }
    };

    class DestinationItem {

    public:
        DestinationItem() {

        }

    public:
        int id;
    public:
        int destinationTypeId;


    public:
        string destinationName;
    public:
        string visibleColumnNames;


    public:
        string quickLink;
    public:
        float X;
    public:
        float Y;

    public:
        string pointGUID;


    public:
        string iaFloorPlanId;

    public:
        string visibleColumsNames;

    public:
        int floorPlanId;

    };

    class MenuAndDestinationItem {

    public:
        MenuAndDestinationItem() {

        }

    public:
        string id;

    public:
        string destinationName;

    public:
        string tags;
/*    public:
        string visibleColumnNames;


    public:
        string quickLink;
    public:
        float X;
    public:
        float Y;

    public:
        string pointGUID;


    public:
        string iaFloorPlanId;

    public:
        string visibleColumsNames;

    public:
        int floorPlanId;*/

    };

}
#endif /* Models_hpp */
