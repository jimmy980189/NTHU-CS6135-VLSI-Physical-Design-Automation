#ifndef FLOORPLAN_H
#define FLOORPLAN_H
#include "header.h"

class FloorPlan{
    private:
        double whiteSpaceRatio;
        int numHardRectilinearBlocks;
        int numTerminals;
        map<string, HardBlock*> hardblocks; 
        map<string, Terminal*> terminals;
        vector<Net*> nets;

        int wirelength = 0;
        int numNets;
        int numPins;
        
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

            return width; 
        }

        int HPWL();

        void Initial();
        void M1();
        void M2();
        void M3();
        bool IsOverlapping(string i, string j);
};

#endif
