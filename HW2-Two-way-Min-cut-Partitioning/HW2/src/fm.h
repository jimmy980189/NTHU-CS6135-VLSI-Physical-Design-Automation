#ifndef FM_H
#define FM_H
#include "header.h"

using namespace std;

class FM {
    private:
        unordered_map<string, CELL*> cells;
        unordered_map<string, NET*> nets;

        PARTITION A;
        PARTITION B;

        set<NET*> cutSet;
        int cutSize;

    public:
        FM() {
            A.SetName("A");
            B.SetName("B");
        }
        ~FM() {
            for (auto cell : cells) 
                delete cell.second;
            for (auto net : nets)
                delete net.second;
        }
        void ReadInputFile(const char* cellInputName, const char* netInputName);
        void GenOutputFile(const char* outputName);

        pair<int, int> GetArea() {
            return { A.GetArea(), B.GetArea() };
        }
        pair<int, int> GetCnt() {
            return { A.GetCnt(), B.GetCnt() };
        }

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
        bool IsBalanced(CELL* cell);

        void Initial();
        void CalAllGain();
        void PrintAllGain();
        void UpdateGain();

        void SetCut();
        int GetCutSize() { return cutSize; }
        int CalCutSize();

        void VerbosePartition();
        void InitBucket();
        void PrintBucket();

        int Pass();
        int Test();
        void MoveCell(CELL* cell);
        CELL* GetCell(string name) { return cells[name]; }

        void PrintDistribution();

        bool IsAllLocked() {
            for (auto c : cells)
                if (!c.second->IsLocked())
                    return false;
            return true;
        }

        void UnlockAll() {
            for (auto c : cells)
                c.second->Unlock();
        }
};

#endif
