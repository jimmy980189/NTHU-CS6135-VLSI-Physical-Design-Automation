#include "floorplan.h"

bool DEBUG = false;
steady_clock::time_point tStart;
steady_clock::time_point tInitialStart;
steady_clock::time_point tInitialEnd;
steady_clock::time_point tSaAreaStart;
steady_clock::time_point tSaAreaEnd;
steady_clock::time_point tSaStart;
steady_clock::time_point tSaEnd;

int main(int argc, char* argv[]) {
    if (argc != 6) {
        cout << "Usage: " << endl;
        cout << "    ./hw3 [*.hardblocks] [*.nets] [*.pl] [*.floorplan] [dead_space_ratio]" << endl;
        exit(-1);
    }
    
    tStart = steady_clock::now();
    FloorPlan Fp;

    steady_clock::time_point tInputStart = steady_clock::now();
    Fp.ReadInputFile(argv[1], argv[2], argv[3], atof(argv[5]));
    steady_clock::time_point tInputEnd = steady_clock::now();

    cout << "----- [Finish Reading Input] -----" << endl;

    cout << Fp.GetWhiteSpaceRatio() << endl;
    cout << Fp.GetTotalBlockArea() << endl;
    cout << Fp.GetWidth() << endl;

    //Fp.InitialBStartTree();
    tInitialStart = steady_clock::now();
    Fp.InitialPolish();

    steady_clock::time_point tOutputStart = steady_clock::now();
    Fp.GenOutputFloorPlan(argv[4]);
    steady_clock::time_point tOutputEnd = steady_clock::now();

    cout << endl << " -----[Time]-----" << endl;
    double inputTime = (double) duration_cast<microseconds>(tInputEnd - tInputStart).count() / 1000000;
    double outputTime = (double) duration_cast<microseconds>(tOutputEnd - tOutputStart).count() / 1000000;
    double saAreaTime = (double) duration_cast<microseconds>(tSaAreaEnd - tSaAreaStart).count() / 1000000;
    double saTime = (double) duration_cast<microseconds>(tSaEnd - tSaStart).count() / 1000000;
    double totalTime = (double) duration_cast<microseconds>(tOutputEnd - tStart).count() / 1000000;
    cout << "Total Time of Input: " << inputTime << endl;
    cout << "Total Time of Output: " << outputTime << endl;
    cout << "Total Time of SA area: " << saAreaTime << endl; 
    cout << "Total Time of SA: " << saTime << endl; 
    cout << "Total Time: " << totalTime << endl;

    return 0;
}
