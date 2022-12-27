#include "header.h"

class Placement {
    private:
        int maxDisplacement;
        int numNodes;
        int numTerminals;
        int numRows;
        map<string, Node*> nodes;
        map<string, Node*> terminals;
        map<int, set<Row*>> rows; 

        pair<double, double> base;
        int rowHeight;
        int siteWidth;

        vector<Row*> garbage;

    public:
        Placement() {}
        ~Placement() {
            for (auto i : garbage) delete i;
            for (auto i : nodes) delete i.second;
        }

        void ReadInputFile(const char* filename);
            void ReadInputNodeFile(string filename);
            void ReadInputPlFile(string filename);
            void ReadInputSclFile(string filename);
        void GenOutputFile(const char* filename);

        void Abacus();
        void PrintRow();
        void SplitSubRow();

        double Cost();
        pair<bool, int> PlaceRow(Row* row, Node* node);

        void Test();
        void Check();
};

