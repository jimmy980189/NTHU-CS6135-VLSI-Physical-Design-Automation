#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cmath>
#include <map>
#include <set>
#include <unordered_map>
#include <climits>
#include <chrono>
#include <cstring>
#include "color.h"

#define LIMIT 250

using namespace std;
using namespace chrono;

enum moveType { MOVABLE, TERMINAL };

class Node {
    private:
        string name;
        int width;
        int height;
        moveType type = MOVABLE;
        pair<double, double> originalCoor;
        pair<double, double> coordinates;

    public:
        Node(string name, int width, int height) : name(name), width(width), height(height) {}
        ~Node() {}

        string GetName() { return name; }
        int GetWidth() { return width; }
        int GetHeight() { return height; }
        int GetX() { return coordinates.first; }
        int GetY() { return coordinates.second; }
        moveType GetType() { return type; }
        pair<double, double> GetCoordinate() { return coordinates; }
        pair<double, double> GetOriginalCoor() { return originalCoor; }
        double GetDisplacement() { 
            return abs(coordinates.first - originalCoor.first) + abs(coordinates.second - originalCoor.second);
        }

        void SetMoveType(moveType t) { type = t; }
        void SetX(double x) { coordinates.first = x; }
        void SetY(double y) { coordinates.second = y; }
        void SetCoordinate(pair<double, double> c) { coordinates = c; }
        void SetOriginalCoor(pair<double, double> c) { 
            coordinates = c;
            originalCoor = c; 
        }
};

class Cluster : public Node {
    private:
        vector<string> include;

    public:
        void AddInclude(string c) { include.push_back(c); }
        vector<string>& GetInclude() { return include; }
        /*
         *Cluster() {}
         *~Cluster() {}
         */
};

class Row {
    private:
        vector<Node*> cells;
        pair<int, int> coordinates;
        int height;
        int siteWidth;
        int numSites;

        pair<double, double> lowerRight = { 0, 0 };

    public:
        Row(pair<int, int> c, int h, int s, int n) : coordinates(c), height(h), siteWidth(s), numSites(n) {}
        ~Row() {}

        int size() { return cells.size(); }

        int GetX() { return coordinates.first; }
        int GetY() { return coordinates.second; }
        int GetHeight() { return height; }
        int GetSiteWidth() { return siteWidth; }
        int GetNumSites() { return numSites; }
        pair<int, int> GetCoordinate() { return coordinates; }
        vector<Node*>& GetCells() { return cells; }

        void AddCell(Node* n) { cells.push_back(n); }

        pair<double, double> GetLowerRight() { return lowerRight; }
        void UpdateLowerRight(pair<double, double> c) { lowerRight = c; }
        Node* GetLast() { return cells.back(); }
};
