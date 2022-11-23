#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cmath>
#include <climits>

using namespace std;

enum type { hardblock, terminal };

struct cmpPos {
    bool operator() (const pair<int, int>& a, const pair<int, int>& b) const {
        return a.first > b.first;
    }
};

class HardBlock {
    private:
        string name;
        vector<pair<int, int>> positions; // four corner
        //pair<int, int> lowerLeftCorner = { 0, 0 };
        pair<int, int> lowerLeftCorner = { 200, 200 };
        int area;
        bool rotated = false;
        type t = hardblock;

    public:
        HardBlock(string name) : name(name) {}
        HardBlock(string name, vector<pair<int, int>> positions) {
            this->name = name;
            this->positions = positions;
        }
        ~HardBlock() {}
        type GetType() { return t; }

        string GetName() { return name; }
        vector<pair<int, int>> GetPositions() { return positions; }
        pair<int, int> GetPosition(int i) { return positions[i]; }
        void AddPosition(pair<int, int> p) { positions.push_back(p); }

        void SetRotated(bool r) { rotated = r; }
        bool GetRotated() { return rotated; }
        int GetArea() { return positions[2].first * positions[2].second; }

        pair<int, int> GetWidthAndHeight() {
            if (!rotated) 
                return { positions[2].first, positions[2].second };
            else
                return { positions[2].second, positions[2].first };
        }
        pair<int, int> GetWidthAndHeight(bool r) {
            if (!r) 
                return { positions[2].first, positions[2].second };
            else
                return { positions[2].second, positions[2].first };
        }

        pair<int, int> GetCenter() {
            int x, y;
            if (!rotated) {
                x = lowerLeftCorner.first + positions[2].first / 2;
                y = lowerLeftCorner.second + positions[2].second / 2;
            }
            else {
                x = lowerLeftCorner.first + positions[2].second / 2;
                y = lowerLeftCorner.second + positions[2].first / 2;
            }
            return { x, y };
        }
        pair<int, int> GetLowerLeftCorner() { return lowerLeftCorner; }
        void SetLowerLeftCorner(pair<int, int> l) { lowerLeftCorner = l; }
};

class Terminal {
    private:
        string name;
        pair<int, int> position;
        type t = terminal;

    public:
        Terminal(string name) : name(name) {}
        ~Terminal() {}
        type GetType() { return t; }

        pair<int, int> GetPosition() { return position; }
        void SetPosition(pair<int, int> p) { position = p; }
};

class Net {
    private:
        int idx;
        int degree;
        vector<string> pins;

    public:
        Net(int idx, int degree) : idx(idx), degree(degree) {}
        ~Net() {}

        int GetIdx() { return idx; }
        vector<string> GetPins() { return pins; }
        string GetPin(int i) { return pins[i]; }
        void AddPin(string p) { pins.push_back(p); }
        int HPWL();
};
