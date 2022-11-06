#include "fm.h"

bool DEBUG = false;
steady_clock::time_point tStart;

int main(int argc, char* argv[]) {

    if (argc < 4) {
        cout << "Usage: ./hw2 [cell file] [net file] [output file]" << endl;
        exit(-1);
    }

    tStart = steady_clock::now();
    FM Fm;

    steady_clock::time_point tIutputStart = steady_clock::now();
    Fm.ReadInputFile(argv[1], argv[2]);
    steady_clock::time_point tIutputEnd = steady_clock::now();

    // Initial
    cout << " -----[Initial Start]----- " << endl;
    Fm.Initial();
    cout << " -----[Initial Finished]----- " << endl;

    Fm.SetCut();

    pair<int, int> area = Fm.GetArea();
    cout << "AreaA: " << area.first << " | " << "AreaB: " << area.second << endl;

    Fm.CalAllGain();
    //Fm.InitBucket();
    Fm.CalCutSize();

    cout << " -----[start pass]------ " << endl;

    int iteration = 0;
    //while ((Fm.Pass() > 0) || (steady_clock::now() < tStart + seconds(100))) {
    while (Fm.Pass() > 0) {
        cout << "[ ITERATION " << iteration++ << " ]" << endl;
    }


    //----------output---------//
    cout << endl << " -----[end   pass]------ " << endl << endl;

    cout << endl << " -----[Info]-----" << endl;
    pair<int, int> partitionCnt = Fm.GetCnt();

    cout << "Number of cells: " << Fm.No_Cells() << endl;
    cout << "Number of nets: " << Fm.No_Nets() << endl;
    cout << "Number of total pins: " << Fm.No_Pins() << endl;
    cout << "Number of nets are cut: " << Fm.GetCutSize() << endl << endl;
    cout << "Number of cells in Partition A: " << partitionCnt.first << endl;
    cout << "Number of cells in Partition B: " << partitionCnt.second << endl;

    // ALL FINISHED START TO GENERATE OUTPUT
    steady_clock::time_point tOutputStart = steady_clock::now();
    Fm.GenOutputFile(argv[3]);
    steady_clock::time_point tOutputEnd = steady_clock::now();

    cout << endl << " -----[Time]-----" << endl;
    double inputTime = (double) duration_cast<microseconds>(tIutputEnd - tIutputStart).count() / 1000000;
    double outputTime = (double) duration_cast<microseconds>(tOutputEnd - tOutputStart).count() / 1000000;
    double totalTime = (double) duration_cast<microseconds>(tOutputEnd - tStart).count() / 1000000;
    cout << "Total Time of Iutput: " << inputTime << "s" << endl;
    cout << "Total Time of Output: " << outputTime << "s" << endl;
    cout << "Total Time of I/O: " << inputTime + outputTime << "s" << endl;
    cout << "Total Time of computation: " << totalTime - (inputTime + outputTime) << "s" << endl;
    cout << "Total Time: " << totalTime << "s" << endl;

    return 0;
}
