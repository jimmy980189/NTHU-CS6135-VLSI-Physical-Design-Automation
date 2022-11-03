#include <iostream>
#include <fstream>
#include "fm.h"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 4) {
        cout << "Usage: ./hw2 [cell file] [net file] [output file]" << endl;
        exit(-1);
    }

    FM Fm;
    Fm.ReadInputFile(argv[1], argv[2]);

    // Initial
    Fm.Initial();
    Fm.SetCut();
    //cout << "cutsize:" << Fm.CalCutSize() << endl;

    /*
     *Fm.CalAllGain();
     *Fm.InitBucket();
     */

    //Fm.PrintDistribution();
    //Fm.PrintAllGain();

/*
 *    Fm.PrintBucket();
 *    cout << " -----[Move e]----- " << endl;
 *    Fm.MoveCell(Fm.GetCell("e"));
 *    Fm.PrintBucket();
 *
 *    cout << " -----[Move d]----- " << endl;
 *    Fm.MoveCell(Fm.GetCell("d"));
 *    Fm.PrintBucket();
 */

    cout << " -----[start pass]------ " << endl;

    int iteration = 0;
    while (Fm.Pass() > 0) {
        cout << "[ ITERATION " << iteration++ << " ]" << endl;
    }
/*
 *    vector<int> result;
 *    for (int i = 0; i < 100; ++i)
 *        result.push_back(Fm.Pass());
 *
 *    int cnt = 0;
 *    for (auto i : result)
 *        cout << cnt++ << " " << i << endl;
 */

    //Fm.Test();
    //Fm.MoveCell(Fm.GetCell("c7"));
    //Fm.PrintAllGain();
    //Fm.PrintBucket();

    //----------output---------//
    cout << endl;
    cout << " -----[end   pass]------ " << endl;
    cout << endl;

    cout << endl;
    cout << " -----[Verbose]-----" << endl;
    //Fm.VerbosePartition();
    Fm.PrintBucket();
    //Fm.SetCut();
    //Fm.CalCutSize();

    cout << "Number of cells: " << Fm.No_Cells() << endl;
    cout << "Number of nets: " << Fm.No_Nets() << endl;
    cout << "Number of total pins: " << Fm.No_Pins() << endl;
    //cout << "Number of nets are cut: " << Fm.No_Cutset() << endl;
    cout << "Number of nets are cut: " << Fm.GetCutSize() << endl;

    Fm.GenOutputFile(argv[3]);
    return 0;
}
