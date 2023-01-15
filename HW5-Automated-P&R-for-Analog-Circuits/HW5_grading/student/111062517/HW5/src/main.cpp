#include "header.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << endl;
        cout << "    ./hw5 [number of current sources] [def file path]" << endl;
        exit(-1);
    }
    // Step 1: create die boundary 
    string designName = string("CS_APR");
    int numCurrentSources = atoi(argv[1]);
    Die die = Die(designName, numCurrentSources);
    
    // Step 2: create CS array
    die.CreateCSarray();

    // Step 3: create vertical ME3
    die.CreateVerticalME3();

    // Step 4: create ME4 drain 
    die.CreateME4Drain();

    // Step 5: create ME4 port 
    die.CreateME4Port();

    // Step 6: create Via34 from ME4 drain 
    die.CreateVia43FromME4Drain();

    // Step 7: create Via34 to ME4 port 
    die.CreateVia34ToME4Port();

    // write info to def file
    die.AddList();
    die.WriteDef(argv[2]);

    return 0;
}
