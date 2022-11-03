#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <math.h>
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

        /*
         *if (tmpSizeA > Smax)
         *    this->Smax = tmpSizeA;
         *else if (tmpSizeB > Smax)
         *    this->Smax = tmpSizeB;
         */

        if (cells.insert({tmpName, cell}).second == false)
            cout << tmpName << " ERRRRRRRR during cells insertion" << endl;
    }

    // Read from input net file
    while (!netInput.eof()) {
        netInput >> tmpName;

        if (tmpName == "NET")  {
            netInput >> netName;
            NET* net = new NET(netName);
            if (nets.insert({netName, net}).second == false)
                cout << netName << " ERRRRRRRR during nets insertion" << endl;

        }
        else if (tmpName == "{")
            opening = true;
        else if (tmpName == "}")
            opening = false;
        else
            if (opening == true) {
                cells[tmpName]->IncrPin();
                cells[tmpName]->InsertNet(nets[netName]);
                nets[netName]->InsertCell(cells[tmpName]);
            }
        /*
         *if (netInput.eof())
         *    break;
         */
    }

    cellInput.close();
    netInput.close();
}

void FM::GenOutputFile(const char* outputName) {
    ofstream output(outputName);
    if (output.fail()) {
        cout << "Failed at opening output file" << outputName << endl;
        exit(-1);
    }
    
    output << "cut_size " << No_Cutset() << endl;
    
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

/*
 *NEED TO BE FIXED
 *moving cell from A to B
 */
bool FM::IsBalanced(int areaA, int areaB, int insert) {
    if ((areaA + insert - areaB) < ((areaA + insert + areaB) / 10.0)) 
        return true;
    else
        return false;
}

bool cmp(CELL* a, CELL* b) {
    //return a->GetSizeA() + a->GetSizeB() > b->GetSizeA() + b->GetSizeB();
    return a->No_Pins() > b->No_Pins();
}

void FM::Initial() {
    vector<CELL*> tmp;
    for (auto cell : cells)
        tmp.push_back(cell.second);

    //sort(tmp.begin(), tmp.end(), cmp);

    for (auto cell : tmp) {

        // For testing easy.cells 
        /*
         *if (cell->GetName() == "c1")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c2")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c3")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c4")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c5")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c6")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c7")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c8")
         *    A.InsertCell(cell, true);
         */

        /*
         *if (cell->GetName() == "c1")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c2")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c3")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c4")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c5")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c6")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c7")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c8")
         *    A.InsertCell(cell, true);
         */

        /*
         *if (cell->No_Pins() != cell->No_Nets()) {
         *    cout << cell->GetName() << endl;
         *    cout << cell->No_Pins() << endl;
         *    cout << cell->No_Nets() << endl;
         *}
         */

        if (IsBalanced(A.GetArea(), B.GetArea(), cell->GetSize()))
            A.InsertCell(cell, true);
        else
            B.InsertCell(cell, false);
    }
}

int FM::CalCutSize() {
    int cutSize = 0;
    for (auto net : nets) {
        if (net.second->GetCutState())
            ++cutSize;
    }
    return cutSize;
}

void FM::SetCut() {
    cutSet.clear();
    for (auto net : nets) {
        bool start = net.second->GetCells().front()->GetPosition();
        net.second->SetCutState(false);
        for (auto cell : net.second->GetCells()) {
            bool tmp = cell->GetPosition();
            net.second->IncrDistribution(tmp);
            if (tmp != start) {
                net.second->SetCutState(true);
                cutSet.insert(net.second);
            }
        }
    }
}

void FM::PrintAllGain() {
    cout << "Print All Gain of cells" << endl;
    for (auto cell : cells)
        cout << cell.first << " " << cell.second->GetGain() << endl;
}

void FM::CalAllGain() {
    for (auto cell : cells) 
        cell.second->CalGain();
}

void FM::VerbosePartition() {
    cout << "PartitionA: " << endl;
    for (auto cell : A.GetCells()) {
        cout << cell->GetName() << " pos: ";
        cout << cell->GetPosition() << " gain: ";
        cout << cell->GetGain() << " locked: ";
        cout << cell->IsLocked() << endl;
    }
    cout << "PartitionB: " << endl;
    for (auto cell : B.GetCells()) {
        cout << cell->GetName() << " pos: ";
        cout << cell->GetPosition() << " gain: ";
        cout << cell->GetGain() << " locked: ";
        cout << cell->IsLocked() << endl;
    }
}

void FM::InitBucket() {
    A.ResetBucket();
    B.ResetBucket();

    for (auto cell : A.GetCells())
        A.InsertBucket(cell);
    for (auto cell : B.GetCells())
        B.InsertBucket(cell);
}

void FM::PrintBucket() {
    cout << "PartitionA" << endl;
    A.PrintBucket();
    cout << "PartitionB" << endl;
    B.PrintBucket();
}

int FM::Pass() {
    int max = 0;
    int k = 0;
    int G = 0; // Maximum partial sum
    CELL* tmp;
    vector<CELL*> seq;
    bool done = false;

    cout << seq.size() << " " << cells.size() << endl;

    while (seq.size() != cells.size()) {

        CELL* tmp = NULL; // choose from both bucket
                          
        //cout  << IsBalanced(A.GetArea(), B.GetArea(), cell->GetSize());
        if (IsBalance() && !tmp->IsLocked()) {
            this->MoveCell(tmp);
            seq.push_back(tmp);
        }
    }

    /*
     *for (auto cell : A.GetCells()) {
     *}
     */
    
    /*
     *for (auto cell : cells) {
     *    cell.second->Unlock();
     *}
     */
    return G;
}

void FM::MoveCell(CELL* cell) {
    bool F = cell->GetPosition();
    bool T = !T;
    int oldGain = cell->GetGain();
    cout << "move " << cell->GetName() <<  " from " << F << " to " << T << endl;

    //cell->SetPosition(T);

    if (F) {
        A.DeleteCell(cell);
        B.InsertCell(cell, false);
    }
    else {
        B.DeleteCell(cell);
        A.InsertCell(cell, true);
    }

    cell->UpdateGain();

    if (F) {
        A.DeleteBucket(cell, oldGain);
        B.InsertBucket(cell);
    }
    else {
        B.DeleteBucket(cell, oldGain);
        A.InsertBucket(cell);
    }
}

void FM::PrintDistribution() {
    for (auto net : nets)
        net.second->PrintDistribution();
}

//-----------------------CELL-----------------------------------------------//

void CELL::CalGain() {
    bool F = GetPosition();
    bool T = !F;
    int tmp = 0;

    this->gain = 0;
    for (auto net : connectedNets) {
        if (net->GetDistribution(F) == 1)
            ++this->gain;
        if (net->GetDistribution(T) == 0)
            --this->gain;
    }
}

void CELL::UpdateGain() {

    /*
     *bool F = this->position;
     *bool T = !F;
     *this->position = T;
     */
    //position has already been updated in MoveCell()
    bool T = this->position;
    bool F = !T;

    this->locked = true;

    for (auto net : connectedNets) {
        if (net->GetDistribution(T) == 0) {
            for (auto cell : net->GetCells()) 
                if (!cell->IsLocked()) 
                    cell->IncrGain();
        }
        else if (net->GetDistribution(T) == 1) {
            for (auto cell : net->GetCells())
                if ((cell->GetPosition() == T) && !cell->IsLocked())
                    cell->DecrGain();
        }

        net->DecrDistribution(F);
        net->IncrDistribution(T);

        if (net->GetDistribution(F) == 0) {
            for (auto cell : net->GetCells())
                if (!cell->IsLocked())
                    cell->DecrGain();
        }
        else if (net->GetDistribution(F) == 1) {
            for (auto cell : net->GetCells()) 
                if ((cell->GetPosition() == F) && !cell->IsLocked()) 
                    cell->IncrGain();
        }
    }
}

void CELL::InsertNet(NET* net) {
    connectedNets.push_back(net);
}

//-----------------------NET-----------------------------------------------//

void NET::InsertCell(CELL* cell) {
    connectedCells.push_back(cell);
}

void NET::IncrDistribution(bool pos) {
    if (pos) 
        ++distribution.first;
    else
        ++distribution.second;
}

void NET::DecrDistribution(bool pos) {
    if (pos) 
        --distribution.first;
    else
        --distribution.second;
}

void NET::PrintDistribution() {
    cout << name << " " << distribution.first << " " << distribution.second << endl;
}

int NET::GetDistribution(bool pos) {
    return pos ? distribution.first : distribution.second;
}

bool NET::IsCritical() {
    if ((distribution.first == 0) || (distribution.second == 1) || 
            (distribution.second == 0) || (distribution.second == 1))
        return true;
    else
        return false;
}

//-----------------------PARTITION-----------------------------------------//

void PARTITION::InsertCell(CELL* cell, bool A) {
    cell->SetPosition(A);
    area += cell->GetSize(); 
    cells.push_back(cell);
}

void PARTITION::DeleteCell(CELL* cell) {
    //int tmpGain = cell->GetGain();
    //area : int
    area -= cell->GetSize();
    
    //cells : vector
    auto f = find(cells.begin(), cells.end(), cell);
    if (f != cells.end()) {
        cells.erase(f);
    }

    //bucket
}

void PARTITION::InsertBucket(CELL* cell) {
    bucket.insert({cell->GetGain(), list<CELL*>()});
    bucket[cell->GetGain()].push_back(cell);
}

void PARTITION::DeleteBucket(CELL* cell, int oldGain) {
    map<int, list<CELL*>, greater<int>>::iterator it = bucket.find(oldGain);
    if (it != bucket.end()) {
        cout << "FOUND" << endl;
        auto &l = it->second;
        auto c = find(l.begin(), l.end(), cell);
        if (c != l.end()) {
            cout << l.size() << endl;
            l.erase(c);
            cout << "REMOVE " << cell->GetName() << endl;
            cout << l.size() << endl;
            if (l.size() == 0)
                bucket.erase(it);
        }
        else
            cout << "404" << endl;
    }
    else
        cout << "NOT IN THE BUCKET" << endl;
}

void PARTITION::PrintBucket() {
    for (auto gain : bucket) {
        cout << gain.first << ": " << endl;
        for (auto cell : gain.second)
            cout << cell->GetName() << " " << cell->GetGain() << endl;
    }
}

void PARTITION::Popbucket() {
}

CELL* PARTITION::TopBucket() {
    return bucket.begin()->second.front();
}
