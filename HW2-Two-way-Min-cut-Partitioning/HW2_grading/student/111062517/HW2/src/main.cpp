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
    Fm.CalCutSize();

    Fm.CalAllGain();
    Fm.InitBucket();

    //Fm.PrintDistribution();
    //Fm.PrintAllGain();
    Fm.PrintBucket();

    cout << " ---------- " << endl;

    Fm.Pass();
    //Fm.MoveCell(Fm.GetCell("c8"));
    //Fm.PrintAllGain();
    Fm.PrintBucket();

    //----------output---------//
    cout << " ---------- " << endl;
    Fm.VerbosePartition();
    Fm.SetCut();
    Fm.CalCutSize();

    cout << "Number of cells: " << Fm.No_Cells() << endl;
    cout << "Number of nets: " << Fm.No_Nets() << endl;
    cout << "Number of total pins: " << Fm.No_Pins() << endl;
    cout << "Number of nets are cut: " << Fm.No_Cutset() << endl;

    Fm.GenOutputFile(argv[3]);
    return 0;
}
