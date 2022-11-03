#ifndef FM_H
#define FM_H
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
#include "header.h"

using namespace std;

class FM {
    private:
        unordered_map<string, CELL*> cells;
        unordered_map<string, NET*> nets;

        /*
         *int W; // AreaA + AreaB
         *int Smax = 0;
         *double r = 0.5;
         */

        PARTITION A;
        PARTITION B;

        set<NET*> cutSet;

    public:
        FM() {}
        ~FM() {
            for (auto cell : cells) 
                delete cell.second;
            for (auto net : nets)
                delete net.second;
        }
        void ReadInputFile(const char* cellInputName, const char* netInputName);
        void GenOutputFile(const char* outputName);

        int No_Cutset() { return cutSet.size(); }
        int No_Cells() { return cells.size(); }
        int No_Nets() { return nets.size(); }
        int No_Pins() {
            int sumN = 0;
            int sumC = 0;

            for (auto i : nets)
                sumN += i.second->No_Cells();

            for (auto i : cells)
                sumC += i.second->No_Pins();

            if (sumN != sumC)
                return -1;
            return sumN; // either sumN or sumC is ok
        }

        // satisfy constrain & balance
        bool IsBalanced(int areaA, int areaB, int insert); 

        void Initial();
        void CalAllGain();
        void PrintAllGain();
        void UpdateGain();
        void SetCut();
        int CalCutSize();

        void VerbosePartition();
        void InitBucket();
        void PrintBucket();

        int Pass();
        void MoveCell(CELL* cell);
        CELL* GetCell(string name) { return cells[name]; }

        void PrintDistribution();
};

#endif
