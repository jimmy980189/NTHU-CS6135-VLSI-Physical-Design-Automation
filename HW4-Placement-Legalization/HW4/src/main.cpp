#include "placement.h"

bool DEBUG = false;
steady_clock::time_point tStart;
steady_clock::time_point tInitialStart;
steady_clock::time_point tInitialEnd;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << endl;
        cout << "    ./hw4 [aux file] [output file]" << endl;
        exit(-1);
    }
    
    tStart = steady_clock::now();
    Placement P;

    steady_clock::time_point tInputStart = steady_clock::now();
    P.ReadInputFile(argv[1]);
    steady_clock::time_point tInputEnd = steady_clock::now();

    cout << "============================================================" << endl;

    steady_clock::time_point tOutputStart = steady_clock::now();
    P.GenOutputFile(argv[2]);
    steady_clock::time_point tOutputEnd = steady_clock::now();

    cout << endl << " -----[Time]-----" << endl;
    double inputTime = (double) duration_cast<microseconds>(tInputEnd - tInputStart).count() / 1000000;
    double outputTime = (double) duration_cast<microseconds>(tOutputEnd - tOutputStart).count() / 1000000;
    double totalTime = (double) duration_cast<microseconds>(tOutputEnd - tStart).count() / 1000000;
    cout << "Total Time of Input: " << inputTime << endl;
    cout << "Total Time of Output: " << outputTime << endl;
    cout << "Total Time: " << totalTime << endl;

    return 0;
}
