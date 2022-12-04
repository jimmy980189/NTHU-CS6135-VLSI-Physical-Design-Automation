#ifndef FLOORPLAN_H
#define FLOORPLAN_H
#include "header.h"

class FloorPlan{
    private:
        double whiteSpaceRatio;
        int numHardRectilinearBlocks;
        int numTerminals;
        map<string, HardBlock*> hardblocks; 
        vector<string> hbName;
        map<string, Terminal*> terminals;
        vector<Net*> nets;

        int wirelength = 0;
        int numNets;
        int numPins;
        
        int area;
        // width = height 
        int width = 0;
        int totalBlockArea = 0;
        pair<int, int> lowerLeftCorner = { 0, 0 };
        pair<int, int> upperRightCorner;
        pair<int, int> boundary = { 0, 0 };
        vector<int> partial;

    public:
        FloorPlan() {}
        ~FloorPlan() {
            for (auto i : hardblocks) delete i.second;
            for (auto i : terminals) delete i.second;
            for (auto i : nets) delete i;
        }

        void ReadInputFile(const char* hardBlocks, const char* nets, const char* pl, const double ratio);
            void ReadInputHardBlocks(const char* filename);
            void ReadInputNets(const char* filename);
            void ReadInputPl(const char* filename);
            void ReadInputWhiteSpaceRatio(const double ratio) { whiteSpaceRatio = ratio; }
        void GenOutputFloorPlan(const char* filename);

        int GetNumHardRectilinearBlocks() { return numHardRectilinearBlocks; }
        int GetNumTerminals() { return numTerminals; }
        double GetWhiteSpaceRatio() { return whiteSpaceRatio; }
        int GetNumNets() { return numNets; }
        int GetNumPins() { return numPins; }
        int GetTotalBlockArea() { return totalBlockArea; }

        int GetWidth() { 
            int width = sqrt(totalBlockArea * (1 + whiteSpaceRatio));
            upperRightCorner = {width, width};

            this->partial = vector<int>(width, 0);
            this->width = width;
            this->area = width * width;

            return width; 
        }

        int HPWL();

        //B*-tree version
        void InitialBStartTree();

        void InitialPolish();
        //int Cost(vector<string>& polishExpression);
        pair<int, pair<int, int>> Cost(vector<string>& polishExpression);
        pair<double, pair<int, int>> CostArea(vector<string>& polishExpression);
        pair<int, int> Polish2FloorPlan(vector<string> polishExpression);
        pair<int, int> M11(vector<string>& expr);
        void M1(vector<string>& expr, int idx);
        void M2(vector<string>& expr, int idx, int n);
        int M3(vector<string>& expr, int idx);
        void SA(vector<string>& expr);
        void SAarea(vector<string>& expr);

        bool CheckBoundary();
        pair<int, int> InitialSlicingTree(vector<string>& polishExpression);
};

#endif
