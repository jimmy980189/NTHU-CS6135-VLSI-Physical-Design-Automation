#include "floorplan.h"

bool DEBUG = true;

int main(int argc, char* argv[]) {
    if (argc != 6) {
        cout << "Usage: " << endl;
        cout << "    ./hw3 [*.hardblocks] [*.nets] [*.pl] [*.floorplan] [dead_space_ratio]" << endl;
        exit(-1);
    }
    
    FloorPlan Fp;
    Fp.ReadInputFile(argv[1], argv[2], argv[3], atof(argv[5]));

    cout << "----- [Finish Reading Input] -----" << endl;

    cout << Fp.GetWhiteSpaceRatio() << endl;
    cout << Fp.GetTotalBlockArea() << endl;
    cout << Fp.GetWidth() << endl;
    Fp.Initial();

    Fp.GenOutputFloorPlan(argv[4]);
    return 0;
}
