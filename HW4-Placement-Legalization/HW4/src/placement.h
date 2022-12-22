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

        vector<Row*> garbage;

    public:
        Placement() {}
        ~Placement() {
            for (auto i : garbage) delete i;
            for (auto i : nodes) delete i.second;
            /*
             *for (auto i : rows) {
             *    for (auto j : i.second) {
             *        delete j;
             *    }
             *}
             */
        }

        void ReadInputFile(const char* filename);
            void ReadInputNodeFile(string filename);
            void ReadInputPlFile(string filename);
            void ReadInputSclFile(string filename);
        void GenOutputFile(const char* filename);

        void SplitSubRow();

        double Cost();
        void Abacus();
        pair<bool, int> PlaceRow(int idx, Node* node);
        int ChooseCandidate(Node* node);
        void PrintRow();
};

