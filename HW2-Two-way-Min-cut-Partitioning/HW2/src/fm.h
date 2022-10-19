#ifndef FM_H
#define FM_H
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "header.h"

using namespace std;

class FM {
    private:
        unordered_map<string, pair<int, int>> cellMap;
        unordered_map<string, vector<string>> netMap;

        unordered_map<string, CELL*> cells;
        unordered_map<string, NET*> nets;

        /*
         *cellArray
         *netArray
         */

        // For Balance
        //int W; // AreaA + AreaB
        int Smax = 0;
        double r = 0.5;

        PARTITION A;
        PARTITION B;

        vector<NET*> cutSet;

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

        int SizeOfCutset() { return cutSet.size(); }
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

        bool IsBalanced(int areaA, int areaB, int insert); // satisfy constrain & balance
        void Initial();
        int CalCutSize();
        //int CalW() { return A.GetArea() + B.GetArea(); };

};

#endif
