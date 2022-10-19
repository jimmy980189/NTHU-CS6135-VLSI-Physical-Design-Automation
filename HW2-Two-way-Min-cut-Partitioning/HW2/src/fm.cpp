#include <fstream>
#include <map>
#include <unordered_map>
#include "fm.h"

void FM::ReadInputFile(const char* cellInputName, const char* netInputName) {

    ifstream cellInput(cellInputName);
    ifstream netInput(netInputName);
    string tmpName;
    string netName;
    int tmpSizeA;
    int tmpSizeB;
    bool opening = false;

    if (cellInput.fail()) {
        cout << "Failed at opening cell file: " << cellInputName << endl;
        exit(-1);
    }
    else if (netInput.fail()) {
        cout << "Failed at opening net file: " << netInputName << endl;
        exit(-1);
    }

    // Read from input cell file
    while (!cellInput.eof()) {

        cellInput >> tmpName >> tmpSizeA >> tmpSizeB;
        if(cellInput.eof()) 
            break;

        CELL* cell = new CELL(tmpName, tmpSizeA, tmpSizeB);
        if (cells.insert({tmpName, cell}).second == false)
            cout << tmpName << " ERRRRRRRR during cells insertion" << endl;
        cellMap.insert({tmpName, {tmpSizeA, tmpSizeB}});
    }

    // Read from input net file
    while (!netInput.eof()) {
        netInput >> tmpName;

        if (tmpName == "NET")  {
            netInput >> netName;
            //cout << netName << endl;
            NET* net = new NET(netName);
            if (nets.insert({netName, net}).second == false)
                cout << netName << " ERRRRRRRR during nets insertion" << endl;

            if (netMap.insert({netName, vector<string>()}).second == false)
                cout << netName << " ERRRRRRRR during netMap insertion" << endl;

        }
        else if (tmpName == "{")
            opening = true;
        else if (tmpName == "}")
            opening = false;
        else
            if (opening == true) {
                cells[tmpName]->IncrPin();
                nets[netName]->InsertCell(cells[tmpName]);
                netMap[netName].push_back(tmpName);
            }
        /*
         *if (netInput.eof())
         *    break;
         */
    }

    // Check
/*
 *    cout << this->cellMap.size() << endl;
 *    for (auto i : this->cellMap) 
 *        cout << i.first << " " << i.second.first << " " << i.second.second << endl;
 *
 *    cout << this->netMap.size() << endl;
 *    for (auto i : this->netMap) {
 *        cout << "NET " << i.first << " {";
 *        for (auto j : i.second) 
 *            cout << " " << j;
 *        cout << " }" << endl;
 *    }
 */
    /*
     *for (auto i : nets) {
     *    cout << "NET " << i.first << " {";
     *    for (auto j : i.second->GetCells())
     *        cout << " " << j->GetName();
     *    cout << "}" << endl;
     *}
     */
    //cout << nets.size() << endl;
    //cout << netMap.size() << endl;

    cellInput.close();
    netInput.close();
}

void FM::GenOutputFile(const char* outputName) {
    ofstream output(outputName);
    if (output.fail()) {
        cout << "Failed at opening output file" << outputName << endl;
        exit(-1);
    }
    
    // tmp cut_size
    output << "cut_size " << CalCutSize() << endl;
    
/*
 *    output << "A " << partitionA.size() << endl;
 *    for (auto i : partitionA) 
 *        output << i << endl;
 *
 *    output << "B " << partitionB.size() << endl;
 *    for (auto i : partitionB) 
 *        output << i << endl;
 */

    cout << "AreaA = " << A.GetArea() << endl;
    cout << "AreaB = " << B.GetArea() << endl;

    output << "A " << A.No_Cells() << endl;
    for (auto i : A.GetCells()) 
        output << i->GetName() << endl;

    output << "B " << B.No_Cells() << endl;
    for (auto i : B.GetCells()) 
        output << i->GetName() << endl;

    output.close();
}

bool FM::IsValid(int areaA, int areaB) {
    if ((areaA - areaB) < ((areaA + areaB) / 10))
        return true;
    else
        return false;
}

void FM::Initial() {
    for (auto cell : cells) {
        //A.InsertCell(cell.second, true);
        if (IsValid(A.GetArea(), B.GetArea()))
            A.InsertCell(cell.second, true);
        else
            B.InsertCell(cell.second, false);
    }
}

int FM::CalCutSize() {
    int cutSize = 0;
    for (auto net : nets) {
        char start = net.second->GetCells().front()->GetPosition();
        for (auto cell : net.second->GetCells()) {
            if (cell->GetPosition() != start) {
                ++cutSize;
                break;
            }
        }
    }
    return cutSize;
}

void PARTITION::InsertCell(CELL* cell, bool left) {
    if (left) {
        cell->SetPosition('A');
        area += cell->GetSizeA(); 
    }
    else {
        cell->SetPosition('B');
        area += cell->GetSizeB();
    }
    cells.push_back(cell);
}

void NET::InsertCell(CELL* cell) {
    connectedCells.push_back(cell);
}
