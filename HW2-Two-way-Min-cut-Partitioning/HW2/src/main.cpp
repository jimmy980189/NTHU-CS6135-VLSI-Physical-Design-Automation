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

    cout << "Number of cells: " << Fm.No_Cells() << endl;
    cout << "Number of nets: " << Fm.No_Nets() << endl;
    cout << "Number of total pins: " << Fm.No_Pins() << endl;

    Fm.Initial();

    Fm.GenOutputFile(argv[3]);
    return 0;
}
