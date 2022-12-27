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
        pair<double, double> originalCoor;  //global
        pair<double, double> coordinates;   //legal

        //for Cluster
        int clusterWidth;   // initial will be the same as the width of this node
        int clusterWeight = 1;
        bool isClusterHead = false;
        Node* left = NULL;
        Node* right = NULL;

        //backup for simulation
        int backupClusterWidth;
        int backupClusterWeight = 1;
        Node* backupLeft = NULL;
        Node* backupRight = NULL;
        pair<double, double> backupCoordinates;

    public:
        Node(string name, int width, int height) : name(name), width(width), height(height) {
            clusterWidth = width;
        }
        ~Node() {}

        string GetName() { return name; }
        int GetWidth() { return width; }
        int GetHeight() { return height; }
        int GetX() { return coordinates.first; }
        int GetY() { return coordinates.second; }
        int GetRightX() { return coordinates.first + width; }
        int GetOrigianlX() { return originalCoor.first; }
        int GetOriginalY() { return originalCoor.second; }
        int GetClusterWidth() { return clusterWidth; }
        int GetClusterWeight() { return clusterWeight; }
        bool IsClusterHead() { return isClusterHead; }
        moveType GetType() { return type; }
        pair<double, double> GetCoordinate() { return coordinates; }
        pair<double, double> GetOriginalCoor() { return originalCoor; }
        double GetDisplacement() { 
            return sqrt(pow(coordinates.first - originalCoor.first, 2) + pow(coordinates.second - originalCoor.second, 2));
        }

        Node* GetLeft() { return left; }
        Node* GetRight() { return right; }
        void SetLeft(Node* l) { left = l; }
        void SetRight(Node* r) { right = r; }

        void SetClusterWidth(int w) { clusterWidth = w; }
        void SetClusterWeight(int w) { clusterWeight = w; }
        void SetIsClusterHead() { isClusterHead = true; }
        void ResetIsClusterHead() { isClusterHead = false; }

        void SetWidth(int w) { width = w; }
        void SetMoveType(moveType t) { type = t; }
        void SetX(double x) { coordinates.first = x; }
        void SetY(double y) { coordinates.second = y; }
        void SetCoordinate(pair<double, double> c) { coordinates = c; }
        void SetOriginalCoor(pair<double, double> c) { 
            coordinates = c;
            originalCoor = c; 
        }

        inline void Store() {
            backupClusterWeight = clusterWeight;
            backupClusterWidth = clusterWidth;
            backupLeft = left;
            backupRight = right;
            backupCoordinates = coordinates;
        }
        inline void Restore() {
            clusterWeight = backupClusterWeight;
            clusterWidth = backupClusterWidth;
            left = backupLeft;
            right = backupRight;
            coordinates = backupCoordinates;
        }
};

class Row {
    private:
        vector<Node*> cells;
        pair<int, int> coordinates;
        int height;
        int siteWidth;
        int numSites;
        int whiteSpace;

        pair<double, double> lowerRight = { 0, 0 };

    public:
        Row(pair<int, int> c, int h, int s, int n) : coordinates(c), height(h), siteWidth(s), numSites(n) {
            whiteSpace = n * s;
        }
        ~Row() {}

        int size() { return cells.size(); }

        int GetX() { return coordinates.first; }
        int GetY() { return coordinates.second; }
        int GetRightX() { return coordinates.first + numSites * siteWidth; }
        int GetHeight() { return height; }
        int GetNumSites() { return numSites; }
        int GetSiteWidth() { return siteWidth; }
        int GetWhiteSpace() { return whiteSpace; }
        pair<int, int> GetCoordinate() { return coordinates; }
        pair<double, double> GetLowerRight() { return lowerRight; }
        vector<Node*>& GetCells() { return cells; }

        void UpdateLowerRight(pair<double, double> c) { lowerRight = c; }

        void AddCell(Node* n) { 
            whiteSpace -= n->GetWidth();
            cells.push_back(n); 
        }

        Node* GetFront() {
            if (cells.size() > 0)
                return cells.front();
            else
                return NULL;
        }
        Node* GetLast() { 
            if (cells.size() > 0)
                return cells.back(); 
            else 
                return NULL;
        }

        void Store() {
            for (auto cell : cells) {
                cell->Store();
            }
        }
        void Restore() {
            for (auto cell : cells) {
                cell->Restore();
            }
        }

        void AlignCluster() {
            //cout << "AlignCluster(): " << endl;
            Node* current = NULL;

            for (int i = 0; i < cells.size(); ++i) {
                int move = 0;
                current = cells[i];
                while (current->GetRight() != NULL) {
                    current->GetRight()->SetCoordinate({ current->GetX() + current->GetWidth(), current->GetY() });
                    ++move;
                    current = current->GetRight();
                }
                i += move;
            }
        }

        void Print() {
            cout << " ==============" << endl;
            cout << "row.Print(): (" << coordinates.first << ", " << coordinates.second << ") " << endl;
            int cnt = 0;
            for (int i = 0; i < cells.size(); ++i) {
                int move = 0;
                Node* head = cells[i];
                cout << i << ": " << head->GetName() << ", (" << head->GetX() << ", " << head->GetY() << ")";
                cout << " | width: " << head->GetWidth();
                cout << " | right: " << head->GetX() + head->GetWidth() << endl;
            }
            cout << endl << " ==============" << endl;
        }
};
