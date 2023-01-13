#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cmath>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <climits>
#include <cfloat>
#include <chrono>
#include <cstring>
#include "color.h"

using namespace std;
using namespace chrono;

class Die {
    private:
        string designName;
        int x1;
        int y1;
        int x2;
        int y2;

    public:
        Die(string n, int x1, int y1, int x2, int y2) {
            this->designName = n;
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
        ~Die() {}
        void SetPos(int x1, int y1, int x2, int y2) {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
};

class Component {
    private:
        string libName;
        string instName;
        int x;
        int y;

    public:
        Component(string l, string i, int x, int y) {
            this->libName = l;
            this->instName = i;
            this->x = x;
            this->y = y;
        }
        ~Component() {}
        void SetPos(int x, int y) {
            this->x = x;
            this->y = y;
        }
};

class SpecialNet {
    private:
        string instName;
        string layer;
        int x1;
        int y1;
        int x2;
        int y2;

    public:
        SpecialNet(string i, string l, int x1, int y1, int x2, int y2) {
            this->instName = i;
            this->layer = l;
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
        ~SpecialNet() {}
        void SetPos(int x1, int y1, int x2, int y2) {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
};
