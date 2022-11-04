#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <climits>
#include <chrono>
#include "fm.h"

#define LIMIT 297

using namespace chrono;

extern steady_clock::time_point tStart;

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
    
    //output << "cut_size " << No_Cutset() << endl;
    output << "cut_size " << GetCutSize() << endl;
    
    cout << "AreaA = " << A.GetArea() << endl;
    cout << "AreaB = " << B.GetArea() << endl;

/*
 *    output << "A " << A.No_Cells() << endl;
 *    for (auto i : A.GetCells()) 
 *        output << i->GetName() << endl;
 *
 *    output << "B " << B.No_Cells() << endl;
 *    for (auto i : B.GetCells()) 
 *        output << i->GetName() << endl;
 */
    A.OutputBucket(output);
    B.OutputBucket(output);

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

bool FM::IsBalanced(CELL* cell) {
    bool F = cell->GetPosition();
    bool T = !F;
    PARTITION* FP = F ? &A : &B;
    PARTITION* TP = T ? &A : &B;

    int areaF = FP->GetArea();
    int areaT = TP->GetArea();
    int cF = cell->GetSize(F);
    int cT = cell->GetSize(T);

    if (abs(areaF - areaT - cF - cT) < ((areaF + areaT - cF + cT) / 10))
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

        //A.InsertCell(cell, true);
        //B.InsertCell(cell, false);
       
        // For testing my.cells
        /*
         *vector<string> cA = { "a", "c", "d", "g" };
         *vector<string> cB = { "b", "e", "f", "h" };
         *if (find(cA.begin(), cA.end(), cell->GetName()) != cA.end())
         *    A.InsertCell(cell, true);
         *else
         *    B.InsertCell(cell, false);
         */


        // For testing easy.cells 
        /*
         *if (cell->GetName() == "c1")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c2")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c3")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c4")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c5")
         *    B.InsertCell(cell, false);
         *else if (cell->GetName() == "c6")
         *    A.InsertCell(cell, true);
         *else if (cell->GetName() == "c7")
         *    A.InsertCell(cell, true);
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
    this->cutSize = cutSize;
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
    /*
     *A.ResetBucket();
     *B.ResetBucket();
     */
    for (auto cell : cells)
        if (cell.second->GetPosition())
            A.InsertBucket(cell.second);
        else
            B.InsertBucket(cell.second);

    /*
     *for (auto cell : A.GetCells())
     *    A.InsertBucket(cell);
     *for (auto cell : B.GetCells())
     *    B.InsertBucket(cell);
     */
}

void FM::PrintBucket() {
    cout << "PartitionA" << endl;
    A.PrintBucket();
    cout << "PartitionB" << endl;
    B.PrintBucket();
}

int FM::Test() {
    vector<CELL*> seq;
    int cnt = 0;
    int max = INT_MIN;
    int idxMax = 0;
    int gain = 0;

    for (auto c : cells) {
        if (IsBalanced(c.second)) {
            gain += c.second->GetGain();
            if (gain > max) {
                max = gain;
                idxMax = cnt++;
            }
            MoveCell(c.second);
            seq.push_back(c.second);
            break;
        }
    }

    for (auto i : seq)
        cout << i->GetName() << "->";
    cout << endl;

    return 0;
}

int FM::Pass() {
    int k = 0;
    int G = 0; // Maximum partial sum
    CELL* tmp;
    bool done = false;
    
    vector<CELL*> seq;
    vector<int> seqGain;

    int cnt = 0;
    int max = 0;
    int idxMax = 0;
    int gain = 0;

    A.ResetBucket();
    B.ResetBucket();
    CalAllGain();
    InitBucket();
    //cout << seq.size() << " " << cells.size() << endl;
    int cutA = CalCutSize();
    cout << "current cutsize " << cutA << endl;

    UnlockAll();

    cout << " ------[FIND MAX GAIN POS START]------ " << endl;

    int whileCnt = 0;
    CELL* waitedA = NULL;
    CELL* waitedB = NULL;

    int forCnt = 0;
    do {
        waitedA = NULL;
        waitedB = NULL;

        for (auto i : A.GetBucket()) {
            ++forCnt;
            if (!i.second->IsLocked() && IsBalanced(i.second)) {
                waitedA = i.second;
                break;
            }
        }

        for (auto i : B.GetBucket()) {
            ++forCnt;
            if (!i.second->IsLocked() && IsBalanced(i.second)) { 
                waitedB = i.second; 
                break;
            }
        }

        CELL* tmp = NULL;
        if (waitedA && waitedB)
            tmp = (waitedA->GetGain() > waitedB->GetGain()) ? waitedA : waitedB;
        else if (waitedA)
            tmp = waitedA;
        else if (waitedB)
            tmp = waitedB;

        if (tmp) {
            if (tmp->GetGain() < 0) break;

            gain += tmp->GetGain();
            if (gain > max) {
                idxMax = cnt;
                max = gain;
            }
            this->MoveCell(tmp);
            seq.push_back(tmp);
            seqGain.push_back(gain);
            ++cnt;
        }

        ++whileCnt;
    } while ((waitedA || waitedB) && (steady_clock::now() < tStart + seconds(LIMIT)));
    /*
     *cout << "forCnt: " << forCnt << endl;
     *cout << "aveforCnt: " << forCnt / whileCnt << endl;
     */


/*
 *    waitedA = A.TopBucket();
 *    waitedB = B.TopBucket();
 *    while ((waitedA || waitedB) && (steady_clock::now() < tStart + seconds(LIMIT))) {
 *
 *        CELL* tmp = NULL;
 *        if (waitedA && waitedB)
 *            tmp = (waitedA->GetGain() > waitedB->GetGain()) ? waitedA : waitedB;
 *        else if (waitedA)
 *            tmp = waitedA;
 *        else if (waitedB)
 *            tmp = waitedB;
 *
 *        if (IsBalanced(tmp)) {
 *            if (tmp->GetGain() < 0) break;
 *
 *            gain += tmp->GetGain();
 *            if (gain > max) {
 *                idxMax = cnt;
 *                max = gain;
 *            }
 *            this->MoveCell(tmp);
 *            seq.push_back(tmp);
 *            seqGain.push_back(gain);
 *            ++cnt;
 *        }
 *
 *        tmp->Lock();
 *
 *        waitedA = A.TopBucket();
 *        waitedB = B.TopBucket();
 *
 *        ++whileCnt;
 *    }
 */

    cout << "CNT: " << cnt << endl;
    cout << "WHILE COUNT: " << whileCnt << endl;

    cout << " ------[FIND MAX GAIN POS END]------ " << endl;

    UnlockAll();

    cout << " ------[RESTORE START]------ " << endl;
    for (int i = seq.size() - 1; i > idxMax; --i) {
        //cout << seq[i]->GetName() << "( " << seqGain[i] << "  )" << "->";
        //cout << KRED << seq[i]->GetName() << "->" << RST;
        this->MoveCell(seq[i]);
    }
    //cout << endl;
    cout << " ------[RESTORE END]------ " << endl;

    /*
     *cout << "max: " << max << endl;
     *cout << "idxMax: " << idxMax << endl;
     *cout << "seq.size(): " << seq.size() << endl;
     *for (int i = 0; i < seq.size(); ++i)
     *    if (i != idxMax)
     *        if (i < idxMax)
     *            cout << seq[i]->GetName() << "( " << KGRN << seqGain[i] << RST << " )" << "->";
     *        else
     *            cout << KRED << seq[i]->GetName() << "( " << seqGain[i] << " )" << "->" << RST;
     *    else
     *        cout << KCYN << seq[i]->GetName() << "( " << seqGain[i] << " )" << RST << "->";
     *cout << endl;
     */

    int cutB = CalCutSize();
    cout << "current cutsize " << cutB << endl;

    cout << "minus " << cutA - cutB << endl;
    /*
     *A.ResetBucket();
     *B.ResetBucket();
     */

    return max;
}

void FM::MoveCell(CELL* cell) {
    bool F = cell->GetPosition();
    bool T = !F;
    int oldGain = cell->GetGain();

    /*
     *cout << FCYN("move ") << cell->GetName() <<  FCYN(" from ") << boolalpha << F << FCYN(" to ") << boolalpha << T << " ";
     *cout << FCYN("its gain is ") << cell->GetGain() << ". size(F/T): " << cell->GetSize(F) << " / " << cell->GetSize(T) << endl;;
     */

    //cell->SetPosition(T);

    if (F) {
        A.DeleteCell(cell);
        B.InsertCell(cell, false);
    }
    else {
        B.DeleteCell(cell);
        A.InsertCell(cell, true);
    }

    //cout << " -------[UPDATE GAIN START]------- " << endl;
    set<CELL*> updated = cell->UpdateGain();
    for (auto i : updated) {
        /*
         *cout << i->GetName();
         *cout << " from ";
         *cout << i->GetIter()->first; 
         *cout << " " ;
         *cout << i->GetGain();
         *cout << endl;
         */

        bool F = i->GetPosition();
        PARTITION* FP = F ? &A : &B;
        if (i->GetIter()->second->GetName() != i->GetName())
            cout << "ERROR" << endl;
        FP->Remove(i->GetIter());
        FP->InsertBucket(i);

        /*
         *if (!FP->CheckBucket())
         *    cout << "UPDATE BUCKET " << i->GetName() << " ";
         */

    }
    //cout << " -------[UPDATE GAIN END]------- " << endl;

    //cout << " -------[UPDATE BUCKET START]------- " << endl;
    /*
     *A.UpdateBucket();
     *B.UpdateBucket();
     */
    //cout << " -------[UPDATE BUCKET END]------- " << endl;

    /*
     *if (F) {
     *    A.DeleteBucket(cell, oldGain);
     *    B.InsertBucket(cell);
     *}
     *else {
     *    B.DeleteBucket(cell, oldGain);
     *    A.InsertBucket(cell);
     *}
     */
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

set<CELL*> CELL::UpdateGain() {

    /*
     *bool F = this->position;
     *bool T = !F;
     *this->position = T;
     */
    //position has already been updated in MoveCell()
    bool T = this->position;
    bool F = !T;

    set<CELL*> updated;

    this->locked = true;

    for (auto net : connectedNets) {
        if (net->GetDistribution(T) == 0) {
            net->SetCutState(true); //
            
            for (auto cell : net->GetCells()) 
                if (!cell->IsLocked()) { 
                    cell->IncrGain(); 
                    updated.insert(cell);
                }
        }
        else if (net->GetDistribution(T) == 1) {
            for (auto cell : net->GetCells())
                if ((cell->GetPosition() == T) && !cell->IsLocked()) { 
                    cell->DecrGain(); 
                    updated.insert(cell);
                }
        }

        net->DecrDistribution(F);
        net->IncrDistribution(T);

        if (net->GetDistribution(F) == 0) {
            net->SetCutState(false); //

            for (auto cell : net->GetCells())
                if (!cell->IsLocked()) { 
                    cell->DecrGain(); 
                    updated.insert(cell);
                }
        }
        else if (net->GetDistribution(F) == 1) {
            for (auto cell : net->GetCells()) 
                if ((cell->GetPosition() == F) && !cell->IsLocked()) { 
                    cell->IncrGain(); 
                    updated.insert(cell);
                }
        }
    }
    return updated;
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

    //cells.push_back(cell);

    //cout << KGRN << "INSERT " << cell->GetName() << " to bucket " << name << RST << endl;
    //InsertBucket(cell);
}

void PARTITION::DeleteCell(CELL* cell) {
    //int tmpGain = cell->GetGain();
    //area : int
    area -= cell->GetSize();
    
    //cells : vector
    /*
     *auto f = find(cells.begin(), cells.end(), cell);
     *if (f != cells.end()) {
     *    cells.erase(f);
     *}
     */

    //bucket
    //DeleteBucket(cell);
}

void PARTITION::InsertBucket(CELL* cell) {
    auto it = bucket.insert({cell->GetGain(), cell});
    //if (it == bucket.end()) { cout << "FAILED INSERT BUCKET"; }
    cell->SetIter(it);
    /*
     *if (cell->GetIter()->second->GetName() != cell->GetName())
     *    cout << "NAME FAIL" << endl;
     */
}

void PARTITION::DeleteBucket(CELL* cell) {
    int oldGain = cell->GetGain();
    multimap<int, CELL*>::iterator i;
    auto range = bucket.equal_range(oldGain);
    for (i = range.first; i != range.second; )
        if (i->second->GetName() == cell->GetName()) {
            //cout << KRED << "DELETE " << i->second->GetName() << " from bucket " << name << RST << endl;
            i = bucket.erase(i);
        }
        else
            ++i;
    cell->SetIter(bucket.end());
}

void PARTITION::PrintBucket() {
    cout << "[" << name << " " << bucket.size() << "]" << endl;
    for (auto gain : bucket) {
        cout << gain.first << ": " << gain.second->GetName() << " " << gain.second->GetGain() << " ";
        cout << (gain.second->IsLocked() ? FRED("LOCKED") : FCYN("UNLOCKED")) << endl;
    }
    cout << endl;
}

void PARTITION::UpdateBucket() {
    multimap<int, CELL*>::iterator i = bucket.begin();

    for (; i != bucket.end(); )
        if (i->first != i->second->GetGain()) {
            //cout << i->second->GetName() << " key and gain are different " << i->first << " " << i->second->GetGain() << endl;
            bucket.insert({i->second->GetGain(), i->second});
            i = bucket.erase(i);
        }
        else
            ++i;
}

void PARTITION::Popbucket() {
}

CELL* PARTITION::TopBucket() {
    for (auto i : bucket) 
        if (!i.second->IsLocked())
            return i.second;

    return NULL;
}
