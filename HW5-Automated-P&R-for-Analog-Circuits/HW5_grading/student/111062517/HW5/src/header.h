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

const int CS_WIDTH = 7100;
const int CS_HEIGHT = 6600;
const int M3_WIDTH = 440;    
const int M3_SPACING = 310;
const int M4_WIDTH = 1000;
const int M4_SPACING = 490;
const int CS_X1_TO_DRAIN = 1260;
const int CS_Y1_TO_DRAIN = 4100;
const string CS_LIB_NAME = string("MSBCS");
const string VIA34_LIB_NAME = string("Via34");

class Component {
    private:
        string libName;
        string instName;
        int _x;
        int _y;

    public:
        Component() {}
        Component(string l, string i, int x, int y) {
            this->libName = l;
            this->instName = i;
            this->_x = x;
            this->_y = y;
        }
        ~Component() {}
        int GetX() { return _x; }
        int GetY() { return _y; }
        string GetInstName() { return instName; }
        string GetLibName() { return libName; }
        void SetLibName(string l) { libName = l; }
        void SetInstName(string i) { instName = i; }
        void SetPos(int x, int y) {
            this->_x = x;
            this->_y = y;
        }
};

class SpecialNet {
    private:
        string instName;
        string layer;
        int _x1;
        int _y1;
        int _x2;
        int _y2;

    public:
        SpecialNet(string i, string l, int x1, int y1, int x2, int y2) {
            this->instName = i;
            this->layer = l;
            this->_x1 = x1;
            this->_y1 = y1;
            this->_x2 = x2;
            this->_y2 = y2;
        }
        ~SpecialNet() {}
        int GetX1() { return _x1; }
        int GetX2() { return _x2; }
        int GetY1() { return _y1; }
        int GetY2() { return _y2; }
        string GetInstName() { return instName; }
        string GetLayer() { return layer; }
        void SetPos(int x1, int y1, int x2, int y2) {
            this->_x1 = x1;
            this->_y1 = y1;
            this->_x2 = x2;
            this->_y2 = y2;
        }
};

class Die {
    private:
        string designName;
        int _x1;
        int _y1;
        int _x2;
        int _y2;
        int numCurrentSources;
        int numRow;
        int numM3;
        int numM4Port;
        Component*** cs_array;
        Component*** Via34_drain2ME3;
        Component**** Via34_port2ME3;
        SpecialNet*** ME3_specialnet;
        SpecialNet*** ME4_specialnet_drain;
        SpecialNet*** ME4_specialnet_port;

        vector<Component*> component_list;
        vector<SpecialNet*> specialnet_list;

    public:
        Die(string n, int numCurrentSources) {
            numRow = sqrt(numCurrentSources) * 2;
            numM3 = numRow / 2;
            numM4Port = numM3 / 2;

            int die_x2 = CS_WIDTH * numRow + M3_SPACING * ((numM3 + 1) * numRow - 1) + M3_WIDTH * numM3 * numRow;
            int die_y2 = CS_HEIGHT * numRow + M4_SPACING * ((numM4Port + 1) * numRow - 1) + M4_WIDTH * numM4Port * numRow;

            this->designName = n;
            this->_x1 = 0;
            this->_y1 = 0;
            this->_x2 = die_x2;
            this->_y2 = die_y2;
            this->numCurrentSources = numCurrentSources;
        }
        ~Die() {}
        void SetPos(int x1, int y1, int x2, int y2) {
            this->_x1 = x1;
            this->_y1 = y1;
            this->_x2 = x2;
            this->_y2 = y2;
        }
        void AddList();
        void WriteDef(const char* filename);
        void CreateCSarray();
        void CreateVerticalME3();
        void CreateME4Drain();
        void CreateME4Port();
        void CreateVia43FromME4Drain();
        void CreateVia34ToME4Port();
};
