#ifndef HEADER_H
#define HEADER_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <chrono>
#include "color.h"

#define LIMIT 270

using namespace std;
using namespace chrono;

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
        multimap<int, CELL*, greater<int>>::iterator iter;

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
        set<CELL*> UpdateGain();
        int CalCutSize();

        multimap<int, CELL*, greater<int>>::iterator GetIter() { return iter; }
        void SetIter(multimap<int, CELL*, greater<int>>::iterator iter) {
            this->iter = iter;
        }
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
        string name;
        multimap<int, CELL*, greater<int>> bucket;
        int cnt = 0;

    public:
        PARTITION() {}
        ~PARTITION() {}
        int GetArea() { return area; }
        string GetName() { return name; }
        void SetName(string n) { name = n; }

        int GetCnt() { return cnt; }
        void IncrCnt() { ++cnt; }
        void DecrCnt() { --cnt; }

        void AddArea(int add) { area += add; }
        void SubArea(int sub) { area -= sub; }

        void InsertCell(CELL* cell, bool pos);
        void DeleteCell(CELL* cell);

        int No_CellsBucket() { return bucket.size(); }
        void OutputBucket(ofstream& output) {
            output << name << " " << bucket.size() << endl;
            for (auto i : bucket) { 
                output << i.second->GetName() << endl; 
            }
        }
        
        multimap<int, CELL*, greater<int>>& GetBucket() { return bucket; }
        void InsertBucket(CELL* cell);
        void DeleteBucket(CELL* cell);
        void ResetBucket() { bucket.clear(); cnt = 0; }
        void UpdateBucket();

        void PrintBucket();

        CELL* TopBucket();
        void Remove(multimap<int, CELL*>::iterator it) { 
            bucket.erase(it); 
            --cnt;
        }
};

#endif
