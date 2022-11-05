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
    //Fm.PrintBucket();

    //Fm.PrintDistribution();
    //Fm.PrintAllGain();

    /*
     *cout << " -----[Move c1]----- " << endl;
     *Fm.MoveCell(Fm.GetCell("c1"));
     *Fm.CalCutSize();
     *Fm.PrintBucket();
     */

    /*
     *cout << " -----[Move e]----- " << endl;
     *Fm.PrintBucket();
     *Fm.MoveCell(Fm.GetCell("e"));
     *Fm.PrintBucket();
     */


    cout << " -----[start pass]------ " << endl;

    int iteration = 0;
    //while ((Fm.Pass() > 0) || (steady_clock::now() < tStart + seconds(100))) {
    while (Fm.Pass() > 0) {
        cout << "[ ITERATION " << iteration++ << " ]" << endl;
        //break;
    }

    //Fm.Test();
    //Fm.PrintAllGain();
    //Fm.PrintBucket();

    //----------output---------//
    cout << endl;
    cout << " -----[end   pass]------ " << endl;
    cout << endl;

    cout << endl;
    cout << " -----[Verbose]-----" << endl;
    //Fm.VerbosePartition();
    //Fm.PrintBucket();
    //Fm.SetCut();
    //Fm.CalCutSize();
    pair<int, int> cnt = Fm.GetCnt();
    cout << cnt.first << " | " << cnt.second << endl;

    cout << "Number of cells: " << Fm.No_Cells() << endl;
    cout << "Number of nets: " << Fm.No_Nets() << endl;
    cout << "Number of total pins: " << Fm.No_Pins() << endl;
    cout << "Number of nets are cut: " << Fm.GetCutSize() << endl;

    // ALL FINISHED START TO GENERATE OUTPUT
    steady_clock::time_point tOutputStart = steady_clock::now();
    Fm.GenOutputFile(argv[3]);
    steady_clock::time_point tOutputEnd = steady_clock::now();

    cout << endl << " -----[Time]-----" << endl;
    cout << "Total Time of Iutput: " << (double) duration_cast<microseconds>(tIutputEnd - tIutputStart).count() / 1000000 << "s" << endl;
    cout << "Total Time of Output: " << (double) duration_cast<microseconds>(tOutputEnd - tOutputStart).count() / 1000000 << "s" << endl;
    cout << "Total Time: " << (double) duration_cast<microseconds>(tOutputEnd - tStart).count() / 1000000 << "s" << endl;

    return 0;
}
