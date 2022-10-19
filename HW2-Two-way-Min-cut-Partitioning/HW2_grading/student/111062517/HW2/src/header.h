#ifndef HEADER_H
#define HEADER_H
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

enum {PARTITIONA, PARTITIONB};
using namespace std;

class CELL {
    private:
        string name;
        char position;
        bool locked;
        int sizeA;
        int sizeB;
        int pins;
        int gain;

    public:
        CELL() { cout << "empty CELLP(): " << endl; }
        CELL(string name, int sizeA, int sizeB) {
            this->name = name;
            this->sizeA = sizeA;
            this->sizeB = sizeB;
            this->pins = 0;
            this->locked = false;
        }
        ~CELL() {}

        //operator=()

        void PrintCellSize() {
            cout << name << " " << sizeA << " " << sizeB << endl;
        };

        string GetName() { return name; }
        int GetSizeA() { return sizeA; }
        int GetSizeB() { return sizeB; }
        int GetGain() { return gain; }
        int No_Pins() { return pins; }
        char GetPosition() { return position; }

        void SetPosition(char p) { position = p; }
        void IncrPin() { ++pins; }

        void Lock() { locked = true; }
        void Unlock() { locked = false; }

        void CalGain();
        void UpdateGain();
};

class NET {
    private:
        bool cutState = false;
        string name;
        vector<CELL*> connectedCells;

    public:
        NET(string name) {
            this->name = name;
        }
        ~NET() {}

        int No_Cells() { return connectedCells.size(); } 
        vector<CELL*> GetCells() { return connectedCells; }
        void InsertCell(CELL* cell);
};

class PARTITION {
    private:
        int area = 0; 
        vector<CELL*> cells;
        map<int, list<CELL*>> bucket;

    public:
        PARTITION() {}
        ~PARTITION() {}
        int GetArea() { return area; }
        int No_Cells() { return cells.size(); }
        vector<CELL*> GetCells() { return cells; }
        void InsertCell(CELL* cell, bool left);
};

#endif
