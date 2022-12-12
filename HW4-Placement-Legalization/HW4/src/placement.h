#include "header.h"

class Placement {
    private:
    public:
        void ReadInputFile(const char* filename);
            void ReadInputNodeFile(string filename);
            void ReadInputPlFile(string filename);
            void ReadInputSclFile(string filename);
        void GenOutputFile(const char* filename);
};

