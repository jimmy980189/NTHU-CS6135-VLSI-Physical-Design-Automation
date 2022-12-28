#include "header.h"

class Placement {
    private:
        int maxDisplacement;
        int numNodes;
        int numTerminals;
        int numRows;
        unordered_map<string, Node*> nodes;
        unordered_map<string, Node*> terminals;
        unordered_map<int, set<Row*>> rows; 

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
        void SplitSubRow();
        pair<bool, int> PlaceRow(Row* row, Node* node);

        void Check();
};
