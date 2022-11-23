#include "floorplan.h"

extern bool DEBUG;

void FloorPlan::ReadInputFile(const char* hardBlocks, const char* nets, const char* pl, const double ratio) {
    ReadInputHardBlocks(hardBlocks);
    ReadInputNets(nets);
    ReadInputPl(pl);
    ReadInputWhiteSpaceRatio(ratio);
}

void FloorPlan::ReadInputHardBlocks(const char* filename) {
    ifstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }

    int cnt = 0;
    string line;
    string name;
    string tmp;
    char colon;
    regex r("(\\w+)|(\\d+)");
    smatch s;

    cout << "----- [Start Reading Input Hard Blocks] -----" << endl;

    f >> tmp >> colon >> numHardRectilinearBlocks;
    f >> tmp >> colon >> numTerminals;

    cout << GetNumHardRectilinearBlocks() << endl;
    cout << GetNumTerminals() << endl;

    while (getline(f, line)) {
        auto begin = sregex_iterator(line.begin(), line.end(), r);
        name = (*begin).str();
        ++begin;
        tmp = (*begin).str();
        ++begin;
        if (tmp == "hardrectilinear") {
            pair<int, int> position;
            HardBlock* h = new HardBlock(name);
            hardblocks.insert({ name, h });
            ++begin;
            for (auto i = begin; i != sregex_iterator(); ++i) {
                if (cnt++ % 2 == 0) {
                    position.first = stoi((*i).str());
                }
                else {
                    position.second = stoi((*i).str());
                    h->AddPosition(position);
                }
            }
            position = h->GetPosition(2);
            totalBlockArea += position.first * position.second;
        }
        else if (tmp == "terminal") {
            //cout << "terminal " << name << endl;
            Terminal* t = new Terminal(name);
            terminals.insert({ name, t });
        }
    }

    if (DEBUG) {
        cout << "----- [Checking Input HardBlocks] -----" << endl;
        for (auto i : hardblocks) {
            cout << i.first << endl;
            for (auto j : i.second->GetPositions())
                cout << "(" << j.first << ", " << j.second << ")" << " "; 
            cout << endl;
        }
    }

    f.close();
}

void FloorPlan::ReadInputNets(const char* filename) {
    ifstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    //cout << filename << endl;
    string tmp;
    char colon;
    int degree = 0;

    cout << "----- [Start Reading Input Nets] -----" << endl;
    f >> tmp >> colon >> numNets;
    f >> tmp >> colon >> numPins;

    cout << numNets << endl;
    cout << numPins << endl;

    for (int i = 0; i < numNets; ++i) {
        f >> tmp >> colon >> degree;
        Net* n = new Net(i, degree);
        nets.push_back(n);
        for (int j = 0; j < degree; ++j) {
            f >> tmp;
            n->AddPin(tmp);
        }
    }

    if (DEBUG) {
        cout << "----- [Checking Reading Input Nets] -----" << endl;
        for (auto i : nets) {
            cout << i->GetIdx();
            for (auto j : i->GetPins())
                cout << " " << j;
            cout << endl;
        }
    }

    f.close();
}

void FloorPlan::ReadInputPl(const char* filename) {
    ifstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    //cout << filename << endl;
    string name;
    pair<int, int> position;

    cout << "----- [Start Reading Input Pl] -----" << endl;

    while (f >> name >> position.first >> position.second) {
        terminals[name]->SetPosition(position);
    }

    if (DEBUG) {
        cout << "----- [Checking pin coordinates] -----" << endl;
        for (auto i : terminals)  {
            position = i.second->GetPosition();
            cout << i.first << " " << position.first << ", " << position.second << endl;
        }
    }

    f.close();
}

void FloorPlan::GenOutputFloorPlan(const char* filename) {
    ofstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    //cout << filename << endl;
    int wirelength = HPWL();
    pair<int, int> tmp;

    f << "Wirelength " << wirelength << endl;
    f << "Blocks" << endl;

    for (auto i : hardblocks) {
        tmp = i.second->GetLowerLeftCorner();
        f << i.first << " " << tmp.first << " " << tmp.second << " " << i.second->GetRotated() << endl;
    }

    f.close();
}

bool cmpHeight(HardBlock* a, HardBlock* b) {
    return a->GetWidthAndHeight().first > b->GetWidthAndHeight().first;
}

bool cmpWidth(HardBlock* a, HardBlock* b) {
    return a->GetWidthAndHeight().second > b->GetWidthAndHeight().second;
}

bool cmpArea(HardBlock* a, HardBlock* b) {
    return a->GetArea() > b->GetArea();
}

void FloorPlan::Initial() {
    pair<int, int> tmp;
    cout << "----- [Start Initial] -----" << endl;
    cout << partial.size() << endl;
    int x = 0;

    /*
     *vector<int> test = {3, 1, -4, 5};
     *cout << *max_element(test.begin(), test.begin() + 4) << endl;
     */
    vector<HardBlock*> pool;
    for (auto i : hardblocks) {
        pair<int, int> w = i.second->GetWidthAndHeight();
        if (w.first < w.second)
            i.second->SetRotated(true);
        pool.push_back(i.second);
    }

    //sort(pool.begin(), pool.end(), cmpHeight);
    sort(pool.begin(), pool.end(), cmpWidth);
    //sort(pool.begin(), pool.end(), cmpArea);

    int cnt = 0;
    for (auto i : pool) {
        tmp = i->GetWidthAndHeight();
        cout << i->GetName() << " " << tmp.first << ", " << tmp.second << endl;
        int y = 0;

        if (x + tmp.first > width)
            x = 0;

        cout << "x: " << x << endl;
        for (int i = 0; i < tmp.first; ++i) {
            cout << partial[x + i] << " ";
            if (partial[x + i] > y)
                y = partial[x + i];
        }
        cout << endl;
        cout << "max:" << y << endl;
        for (int i = 0; i < tmp.first; ++i) {
            partial[x + i] = y + tmp.second;
        }
        i->SetLowerLeftCorner({ x, y });

        x += tmp.first;

        //if (cnt++ == 25) break;

        int check = partial[0];
        int keep = 0;
        int c = 0;
        for (int i = 0; i < partial.size(); ++i) {
            if (check == partial[i])
                ++c;
            else {
                cout << "(" << keep << ", " << i << ") " << i - keep << ": " << check << " | ";
                check = partial[i];
                keep = i;
                c = 0;
            }
        }
        cout << endl;

        //if (cnt++ == 2) break;
    }

    cout << " ---- " << endl;
    for (auto i : partial)
        cout << i << " ";
    cout << endl;
}

bool FloorPlan::IsOverlapping(string i, string j) {
    
    return false;
}

int FloorPlan::HPWL() {
    int wirelength = 0;
    pair<int, int> tmp;

    for (auto i : nets) {
        pair<int, int> lowerLeftCorner = { INT_MAX, INT_MAX };
        pair<int, int> upperRightCorner = { 0, 0 };
        //cout << i->GetIdx() << endl;

        for (auto j : i->GetPins()) {

            if (j.substr(0, 2) == "sb") {
                tmp = hardblocks[j]->GetCenter();
                //cout << "sb " << j << " " << tmp.first << ", " << tmp.second << endl;
            }
            else {
                tmp = terminals[j]->GetPosition();
                //cout << "p " << j << " " << tmp.first << ", " << tmp.second << endl;
            }

            if (tmp.first < lowerLeftCorner.first)
                lowerLeftCorner.first = tmp.first;
            if (tmp.second < lowerLeftCorner.second)
                lowerLeftCorner.second = tmp.second;
            if (tmp.first > upperRightCorner.first)
                upperRightCorner.first = tmp.first;
            if (tmp.second > upperRightCorner.second)
                upperRightCorner.second = tmp.second;
        }

        int tmpWL = (upperRightCorner.first - lowerLeftCorner.first + upperRightCorner.second - lowerLeftCorner.second);
        //cout << upperRightCorner.first << " " << upperRightCorner.second << endl;
        //cout << lowerLeftCorner.first << " " << lowerLeftCorner.second << endl;
        //cout << "wl: " << tmpWL << endl;
        wirelength += tmpWL;
    }

    this->wirelength = wirelength;
    return wirelength;
}
