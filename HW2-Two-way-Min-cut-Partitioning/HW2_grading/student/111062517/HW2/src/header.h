#ifndef HEADER_H
#define HEADER_H
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

enum {PARTITIONA, PARTITIONB};
using namespace std;

class NET;

class CELL {
    private:
        string name;
        bool position = true; // true: A, false: B
        bool locked;
        int sizeA;
        int sizeB;
        int pins;
        int gain;
        vector<NET*> connectedNets;

    public:
        CELL() {}
        CELL(string name, int sizeA, int sizeB) {
            this->name = name;
            this->sizeA = sizeA;
            this->sizeB = sizeB;
            this->pins = 0;
            this->locked = false;
        }
        ~CELL() {}

        int No_Nets() { return connectedNets.size(); } 
        vector<NET*> GetNets() { return connectedNets; }
        void InsertNet(NET* net);

        void PrintCellSize() {
            cout << name << " " << sizeA << " " << sizeB << endl;
        };

        string GetName() { return name; }
        int GetSize() { return position ? sizeA : sizeB; }
        int GetSize(bool pos) { return pos ? sizeA : sizeB; }
        int GetGain() { return gain; }
        bool GetPosition() { return position; }

        void SetPosition(bool p) { position = p; }
        void IncrPin() { ++pins; }
        int No_Pins() { return pins; }

        bool IsLocked() { return locked; }
        void Lock() { locked = true; }
        void Unlock() { locked = false; }

        void IncrGain() { ++gain; }
        void DecrGain() { --gain; }
        void CalGain();
        void UpdateGain();
        int CalCutSize();

};

class NET {
    private:
        string name;
        bool cutState = false;
        bool critical = false; //true if moving one of the cell on this net will change its cut state
        pair<int, int> distribution;
        vector<CELL*> connectedCells;

    public:
        NET(string name) {
            this->name = name;
            distribution.first = 0;
            distribution.second = 0;
        }
        ~NET() {}

        int No_Cells() { return connectedCells.size(); } 
        bool GetCutState() { return cutState; }
        bool IsCritical();
        void SetCutState(bool pos) { cutState = pos; }
        int GetDistribution(bool pos);

        void IncrDistribution(bool pos);
        void DecrDistribution(bool pos);
        void PrintDistribution();

        void InsertCell(CELL* cell);
        vector<CELL*> GetCells() { return connectedCells; }
};

class PARTITION {
    private:
        int area = 0; 
        vector<CELL*> cells;
        map<int, list<CELL*>, greater<int>> bucket;
        map<CELL*, list<CELL*>::iterator> cellss;

    public:
        PARTITION() {}
        ~PARTITION() {}
        int GetArea() { return area; }

        int No_Cells() { return cells.size(); }
        vector<CELL*> GetCells() { return cells; }

        void InsertCell(CELL* cell, bool pos);
        void DeleteCell(CELL* cell);
        
        void InsertBucket(CELL* cell);
        void DeleteBucket(CELL* cell, int oldGain);
        void ResetBucket() { bucket.clear(); }

        void PrintBucket();

        void Popbucket();
        CELL* TopBucket();
};

#endif
