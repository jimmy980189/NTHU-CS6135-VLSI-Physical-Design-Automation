#include "placement.h"

void Placement::ReadInputFile(const char* filename) {
    ifstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        string nodeFile;
        string sclFile;
        string plFile;
        string line;
        f >> line;
        f >> line;
        while (f >> line) {
            cout << line << endl;
        }

        cout << "Read .aux file successfully" << endl;
        cout << "Trying to readfile: " << endl;
        cout << "  " << nodeFile << endl;
        cout << "  " << plFile << endl;
        cout << "  " << sclFile << endl;

        /*
         *ReadInputSclFile(sclFile);
         *ReadInputNodeFile(nodeFile);
         *ReadInputPlFile(plFile);
         */
    }
}

void Placement::ReadInputNodeFile(string filename) {
    ifstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        cout << "Read .node file successfully" << endl;
    }
}

void Placement::ReadInputPlFile(string filename) {
    ifstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        cout << "Read .pl file successfully" << endl;
    }
}

void Placement::ReadInputSclFile(string filename) {
    ifstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        cout << "Read .scl file successfully" << endl;
    }
}

void Placement::GenOutputFile(const char* filename) {
    ofstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
}
