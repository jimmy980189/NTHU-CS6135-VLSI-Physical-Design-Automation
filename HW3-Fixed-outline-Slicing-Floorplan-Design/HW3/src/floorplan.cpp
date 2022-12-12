#include "floorplan.h"

extern bool DEBUG;
extern steady_clock::time_point tStart;
extern steady_clock::time_point tInitialStart;
extern steady_clock::time_point tInitialEnd;
extern steady_clock::time_point tSaAreaStart;
extern steady_clock::time_point tSaAreaEnd;
extern steady_clock::time_point tSaStart;
extern steady_clock::time_point tSaEnd;

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
    string colon;
    regex r("(\\w+)|(\\d+)");
    smatch s;

    cout << "----- [Start Reading Input Hard Blocks] -----" << endl;

    f >> tmp >> colon >> numHardRectilinearBlocks;
    f >> tmp >> colon >> numTerminals;

    cout << GetNumHardRectilinearBlocks() << endl;
    cout << GetNumTerminals() << endl;

    HardBlock* h = NULL;
    Terminal* t = NULL;
    smatch sm;

    while (getline(f, line)) {
        if (line.size() == 0) 
            continue;

        sregex_iterator begin = sregex_iterator(line.begin(), line.end(), r);
        name = (*begin).str();
        ++begin;
        tmp = (*begin).str();
        ++begin;
        if (tmp == "hardrectilinear") {
            pair<int, int> position;
            h = new HardBlock(name);
            hardblocks.insert({ name, h });
            hbName.push_back(name);
            ++begin;
            int pCnt = 0;
            for (sregex_iterator i = begin; i != sregex_iterator(); ++i) {
                if (cnt++ % 2 == 0) {
                    position.first = stoi((*i).str());
                }
                else {
                    position.second = stoi((*i).str());
                    h->AddPosition(position);
                    if (pCnt++ == 2)
                        h->SetWidthAndHeight(position);
                }
            }
            position = h->GetPosition(2);
            totalBlockArea += position.first * position.second;
        }
        else if (tmp == "terminal") {
            //cout << "terminal " << name << endl;
            t = new Terminal(name);
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

bool cmpWidth(HardBlock* a, HardBlock* b) {
    return a->GetWidthAndHeight().first > b->GetWidthAndHeight().first;
}

bool cmpHeight(HardBlock* a, HardBlock* b) {
    return a->GetWidthAndHeight().second > b->GetWidthAndHeight().second;
}

bool cmpArea(HardBlock* a, HardBlock* b) {
    return a->GetArea() > b->GetArea();
}

void FloorPlan::InitialBStartTree() {
    pair<int, int> tmp;
    cout << "----- [Start Initial] -----" << endl;
    cout << partial.size() << endl;
    int x = 0;

    vector<HardBlock*> pool;
    for (auto i : hardblocks) {
        pair<int, int> w = i.second->GetWidthAndHeight();
        if (w.first < w.second)
            i.second->SetRotated(true);
        pool.push_back(i.second);
    }

    sort(pool.begin(), pool.end(), cmpHeight);
    //sort(pool.begin(), pool.end(), cmpWidth);
    //sort(pool.begin(), pool.end(), cmpArea);

    int cnt = 0;
    for (auto i : pool) {
        tmp = i->GetWidthAndHeight();
        cout << i->GetName() << " " << tmp.first << ", " << tmp.second << endl;
        int y = 0;

        if (x + tmp.first > width)
            x = 0;

        cout << "x: " << x << endl;
        //find max height
        for (int i = 0; i < tmp.first; ++i) {
            //cout << partial[x + i] << " ";
            if (partial[x + i] > y)
                y = partial[x + i];
        }
        //cout << endl;

        // y exceed top boundary
        while (y + tmp.second > width && x + tmp.first < width) {
            ++x;
            y = 0;
            for (int i = 0; i < tmp.first; ++i) {
                //cout << partial[x + i] << " ";
                if (partial[x + i] > y)
                    y = partial[x + i];
            }
        }
        if (x > width || y > width)
            cout << i->GetName() << " Exceed Boundary" << endl;

        cout << "y:" << y << endl;
        for (int i = 0; i < tmp.first; ++i) {
            partial[x + i] = y + tmp.second;
        }
        i->SetLowerLeftCorner({ x, y });

        x += tmp.first;
    }

    cout << " ---- " << endl;
    /*
     *for (auto i : partial)
     *    cout << i << " ";
     *cout << endl;
     */
}

void FloorPlan::InitialPolish() {

    vector<string> polishExpression;
    vector<HardBlock*> pool;

    for (auto i : hardblocks) {
        pair<int, int> w = i.second->GetWidthAndHeight();
        /*
         *if (w.first > w.second)
         *    i.second->SetRotated(true);
         */
        /*
         *if (w.first < w.second)
         *    i.second->SetRotated(true);
         */
        pool.push_back(i.second);
    }

    //sort(pool.begin(), pool.end(), cmpHeight);

    //sort(pool.begin(), pool.end(), cmpWidth);
    //sort(pool.begin(), pool.end(), cmpArea);

    int cnt = 0;
    int w = 0;
    int exceed = 0;
    for (auto i : pool) {
        if (w + i->GetWidth() <= width) {
            polishExpression.push_back(i->GetName());
            w += i->GetWidth();
            if (cnt != 0)
                polishExpression.push_back("V");
        }
        else {
            if (exceed > 0) {
                polishExpression.push_back("H");
                --exceed;
            }
            polishExpression.push_back(i->GetName());
            w = i->GetWidth();
            ++exceed;
        }
        ++cnt;
    }
    if (exceed > 0) 
        polishExpression.push_back("H");

    cout << "initial Polish2FloorPlan" << endl;

    pair<int, int> location = InitialSlicingTree(polishExpression);
    //cout << location.first << " " << location.second << endl;
    pair<int, int> boundary = Polish2FloorPlan(polishExpression);
    cout << "initial boundary: " << boundary.first << " " << boundary.second << endl;
    tInitialEnd = steady_clock::now();
    //M1(polishExpression, 0);
    //InitialSlicingTree(polishExpression);
    //Polish2FloorPlan(polishExpression);

/*
 *    string test = "sb0,sb1,V,sb2,sb3,V,H,sb4,sb5,V,sb6,H,V,sb7,sb8,V,sb9,H,sb10,sb11,V,sb12,H,V,H,sb13,sb14,V,sb15,H,sb16,sb17,V,sb18,H,V,sb19,sb20,V,sb21,H,sb22,sb23,V,sb24,H,V,H,V,sb25,sb26,V,sb27,sb28,V,H,sb29,sb30,V,sb31,H,V,sb32,sb33,V,sb34,H,sb35,sb36,V,sb37,H,V,H,sb38,sb39,V,sb40,H,sb41,sb42,V,sb43,H,V,sb44,sb45,V,sb46,H,sb47,sb48,V,sb49,H,V,H,V,H,sb50,sb51,V,sb52,sb53,V,H,sb54,sb55,V,sb56,H,V,sb57,sb58,V,sb59,H,sb60,sb61,V,sb62,H,V,H,sb63,sb64,V,sb65,H,sb66,sb67,V,sb68,H,V,sb69,sb70,V,sb71,H,sb72,sb73,V,sb74,H,V,H,V,sb75,sb76,V,sb77,sb78,V,H,sb79,sb80,V,sb81,H,V,sb82,sb83,V,sb84,H,sb85,sb86,V,sb87,H,V,H,sb88,sb89,V,sb90,H,sb91,sb92,V,sb93,H,V,sb94,sb95,V,sb96,H,sb97,sb98,V,sb99,H,V,H,V,H,V";
 *    vector<std::string> tokens;
 *    string token;
 *    stringstream ss(test);
 *    while (getline(ss, token, ',')){
 *        tokens.push_back(token);
 *    }
 *     
 *    for (auto &s: tokens) {
 *        cout << s << endl;
 *    }
 *
 *    polishExpression = tokens;
 */

    //swap(polishExpression[0], polishExpression[1]);

    tSaAreaStart = steady_clock::now();
    SAarea(polishExpression);
    tSaAreaEnd = steady_clock::now();

    /*
     *tSaStart = steady_clock::now();
     *SA(polishExpression);
     *tSaEnd = steady_clock::now();
     */
}

void FloorPlan::SA(vector<string>& polishExpression) {

    int cnt = 0;
    double r = 0.99;
    int epsilon = 100;
    int k = 2;

    int reject = 0;
    int N = k * numHardRectilinearBlocks;
    int M = 0;
    int MT = 0;
    int uphill = 0;
    //int T = 1000;
    //int T = 1000000;
    int T = INT_MAX;

    vector<string> best = polishExpression;
    pair<int, pair<int, int>> tmpCost = Cost(polishExpression);
    pair<int, int> tmpIdx;
    pair<int, int> bestWH;
    int bestCost = tmpCost.first;
    int oldCost = tmpCost.first;
    int idx;
    int idx2;
    int n = 0;

    do {
        MT = 0;
        uphill = 0;
        reject = 0;
        do {
            //M = rand() % 10;
            M = rand() % 4;
            switch (M) {
                case 0:
                    //M11(polishExpression);
                    idx = rand() % polishExpression.size();
                    M1(polishExpression, idx);
                    break;
                case 1:
                    idx = rand() % polishExpression.size();
                    n = rand() % polishExpression.size();
                    M2(polishExpression, idx, n);
                    break;
                case 2:
                    idx = rand() % polishExpression.size();
                    idx = M3(polishExpression, idx);
                    break;
                case 3:
                    tmpIdx = M11(polishExpression);
                    break;
                default:
                    idx = rand() % hbName.size();
                    hardblocks[hbName[idx]]->RevRotated();
                    break;
            }
            ++MT;
            tmpCost = Cost(polishExpression);
            int newCost = tmpCost.first;
            int deltaCost = newCost - oldCost;
            double a =  (double) rand() / RAND_MAX;
            double b = (double) exp((double) -deltaCost / T); 
            if ((deltaCost <= 0) || (false)) {
            cout << "area: " << tmpCost.second.first * tmpCost.second.second << " , cost: " << tmpCost.first;
            cout << " | w/h: " << tmpCost.second.first << " " << tmpCost.second.second << endl;
            //if ((deltaCost <= 0) || ((double) rand() / RAND_MAX < exp(-1 * deltaCost / T))) {
            //if ((tmpCost.second.first < width && tmpCost.second.second < width && deltaCost <= 0) || (false)) {
                oldCost = newCost;
                cout << "deltaCost: " << deltaCost << endl;
                cout << "T: " << T << endl;
                cout << "-deltaCost / T: " << (double)-deltaCost / T << endl; 
                if (newCost > oldCost) {
                    ++uphill;
                }
                //if (newCost < bestCost) {
                if (newCost < bestCost || tmpCost.second.first <= width && tmpCost.second.second <= width) {
                    cout << "newCost: " << newCost << " ";
                    cout << "bestCost: " << bestCost << " ";
                    cout << "w/h: " << tmpCost.second.first << " " << tmpCost.second.second << endl;
                    bestWH = tmpCost.second;
                    bestCost = newCost;
                    best = polishExpression;
                }
                if (tmpCost.second.first <= width && tmpCost.second.second <= width)
                    cout << FGRN("LEGALLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL") << endl;
                else
                    cout << FRED("NOTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT") << endl;
            }
            else {
                ++reject;
                switch (M) {
                    case 0:
                        M1(polishExpression, idx);
                        break;
                    case 1:
                        M2(polishExpression, idx, n);
                        break;
                    case 2:
                        if (idx != -1) {
                            swap(polishExpression[idx], polishExpression[idx + 1]);
                        }
                        break;
                    case 3:
                        if (tmpIdx.first != -1) 
                            swap(polishExpression[tmpIdx.first], polishExpression[tmpIdx.second]);
                        break;
                    default:
                        hardblocks[hbName[idx]]->RevRotated();
                        break;
                }
            }

        } while ((uphill < N) && (MT < 2 * N));

        T *= r;
        cout << "[while1] reject / MT: " << (double) reject/MT << " reject: " << reject << " MT: " << MT << " T: " << T << endl;

    } while (((double) (reject / MT) < 0.95) && (T > epsilon) && (steady_clock::now() < tStart + seconds(LIMIT)));

    cout << "--- After SA ---" << endl;
    InitialSlicingTree(polishExpression);
    //boundary = InitialSlicingTree(polishExpression);
    cout << "cost: " << Cost(polishExpression).first << endl;
    boundary = Polish2FloorPlan(polishExpression);
    cout << "W/H: " << boundary.first << " " << boundary.second << endl;
    cout << "Area: " << boundary.first * boundary.second << endl;
    cout << "WL: " << HPWL() << endl;

    cout << tmpCost.first << " " << bestCost << endl;
    if (bestCost < tmpCost.first) {
        cout << "BEST" << endl;
        InitialSlicingTree(polishExpression);
        //boundary = InitialSlicingTree(best);
        cout << "cost: " << Cost(best).first << endl;
        boundary = Polish2FloorPlan(best);
        cout << "W/H: " << boundary.first << " " << boundary.second << endl;
        cout << "Area: " << boundary.first * boundary.second << endl;
        cout << "WL: " << HPWL() << endl;
    }
}

void FloorPlan::SAarea(vector<string>& polishExpression) {

    //int T = INT_MAX;
    double T = 100000;
    double r = 0.99;
    int epsilon = 100;

    vector<string> best = polishExpression;
    pair<int, pair<int, int>> tmpCost = CostArea(polishExpression);
    int bestCost = tmpCost.first;
    int oldCost = tmpCost.first;

    // for random selecting operations
    int MT = 0;
    int k = 10;
    int N = k * numHardRectilinearBlocks;
    int uphill = 0;
    int reject = 0;
    int M = 0;

    pair<int, int> tmpIdx;
    int n = 0;
    int idx;
    int idx2;
    int cnt = 0;

    srand(time(NULL));

    do {
        MT = 0;
        uphill = 0;
        reject = 0;
        do {
            M = rand() % 6;
            switch (M) {
                case 0:
                    idx = rand() % polishExpression.size();
                    n = rand() % polishExpression.size();
                    M2(polishExpression, idx, n);
                    break;
                case 1:
                    idx = rand() % polishExpression.size();
                    idx = M3(polishExpression, idx);
                    break;
                default:
                    tmpIdx = M11(polishExpression);
                    break;
            }
            ++MT;
            tmpCost = CostArea(polishExpression);
            int newCost = tmpCost.first;
            double deltaCost = newCost - oldCost;
            if ((deltaCost <= 0) || (false)) {
            //if ((deltaCost <= 0) || ((double) rand() / RAND_MAX < exp(-1 * deltaCost / T))) {
                oldCost = newCost;
                steady_clock::time_point tNow = steady_clock::now();
                cout << duration_cast<duration<double>>(tNow - tStart).count() << " | ";
                cout << "iteration cnt: " << cnt << " ";
                cout << "area: " << tmpCost.second.first * tmpCost.second.second << " , cost: " << tmpCost.first;
                cout << " | w/h: " << tmpCost.second.first << " " << tmpCost.second.second << endl;
                if (deltaCost > 0) {
                    ++uphill;
                }
                if (newCost < bestCost) {
                    bestCost = newCost;
                    best = polishExpression;
                }
                if (tmpCost.second.first <= width && tmpCost.second.second <= width) {
                    return;
                }
            }
            else {
                ++reject;

                switch (M) {
                    case 0:
                        M2(polishExpression, idx, n);
                        break;
                    case 1:
                        if (idx != -1) {
                            swap(polishExpression[idx], polishExpression[idx + 1]);
                        }
                        break;
                    default:
                        if (tmpIdx.first != -1) {
                            swap(polishExpression[tmpIdx.first], polishExpression[tmpIdx.second]);
                        }
                }
            }
            ++cnt;
        } while ((uphill < N) && (MT < 2 * N));
        T *= r;
        cout << "[while1] reject / MT: " << (double) reject/MT << " reject: " << reject << " MT: " << MT << " T: " << T << endl;
    } while (((double) (reject / MT) < 0.95) && (T > epsilon) && (steady_clock::now() < tStart + seconds(200)));

    cout << tmpCost.first << " " << bestCost << endl;
}

pair<int, pair<int, int>> FloorPlan::Cost(vector<string>& polishExpression) {
    pair<int, int> boundary = InitialSlicingTree(polishExpression);
    //InitialSlicingTree(polishExpression);
    //pair<int, int> boundary = Polish2FloorPlan(polishExpression);
    int tmpArea = boundary.first * boundary.second;
    int penalty = 0;
    if (boundary.first - width > 0)
        penalty += (boundary.first - width);
    if (boundary.second - width > 0)
        penalty += (boundary.second - width);

    int wirelength = HPWL();
    cout << "wirelength:" << wirelength << endl;
    return  {
        //10 * tmpArea + 1 * HPWL() + 100000 * penalty,
        0.7 * tmpArea + 0.3 * wirelength + 100000 * penalty,
        //0.6 * tmpArea + 0.4 * wirelength + 100000 * penalty,
        //0.5 * tmpArea + 0.5 * HPWL(),
        //tmpArea + 1000000 * penalty,
        //HPWL(),
        //0.5 * HPWL() + 100000 * penalty,
        boundary
    };
}

pair<double, pair<int, int>> FloorPlan::CostArea(vector<string>& polishExpression) {
    pair<int, int> boundary = InitialSlicingTree(polishExpression);
    //pair<int, int> boundary = { 500, 500 };
    //InitialSlicingTree(polishExpression);
    //pair<int, int> boundary = Polish2FloorPlan(polishExpression);
    int tmpArea = boundary.first * boundary.second;
    int penalty = 0;
    if (boundary.first - width > 0)
        penalty += (boundary.first - width);
    if (boundary.second - width > 0)
        penalty += (boundary.second - width);

    /*
     *float alpha = 0.7;
     *float beta = 0;
     *float R_star = (double) boundary.first / boundary.second;
     *float R = 0.9;
     *float cost = alpha * tmpArea + beta * HPWL() + (1 - alpha - beta) * (R_star - R) * (R_star - R);
     *int wirelength = HPWL();
     */

    double testpenalty = 0;
    if (boundary.first - width * 0.99 > 0) {
        testpenalty += pow((double) boundary.first - width + 1, 3);
    }
    else
        testpenalty += 10000;

    if (boundary.second - width * 0.99 > 0) {
        testpenalty += pow((double) boundary.second - width + 1, 3);
    }
    else 
        testpenalty += 10000;


    return  {
        //tmpArea,
        //wirelength,
        //0.5 * tmpArea + 0.5 * wirelength,
        //tmpArea + 100000 * penalty,
        testpenalty,
        //penalty,
        //cost,
        boundary
    };
}

pair<int, int> FloorPlan::M11(vector<string>& expr) {
    /*
     *int r1 = rand() % (expr.size() / 2);
     *int r2 = rand() % (expr.size() / 2) + expr.size() / 2;
     */
    int r1 = rand() % expr.size();
    int r2 = rand() % expr.size();

    while (expr[r1] == "V" || expr[r1] == "H") {
        r1 = rand() % expr.size();
    }
    while (expr[r2] == "V" || expr[r2] == "H") {
        r2 = rand() % expr.size();
    }

    /*
     *cout << expr.size() << endl;
     *cout << r1 << " " << r2 << endl;
     *cout << "M11" << expr[r1] << " " << expr[r2] << " " << abs(r1 - r2) << endl;
     */

    if (r1 != r2) {
        swap(expr[r1], expr[r2]);
        return { r1, r2 };
    }

    return { -1, -1 };
}

void FloorPlan::M1(vector<string>& expr, int idx) {
    while (idx < expr.size()) {
        if (expr[idx] != "V" && expr[idx] != "H")
            break;
        ++idx;
    }

    int tmp = idx;
    for (int i = idx + 1; i < expr.size(); ++i)
        if (expr[i] != "V" && expr[i] != "H") {
            tmp = i;
            break;
        }

    if (idx == tmp) return;

    swap(expr[idx], expr[tmp]);
}

void FloorPlan::M2(vector<string>& expr, int idx, int n) {
    while (idx + n < expr.size() - 1) {
        if (expr[idx] == "V" && expr[idx] == "H")
            break;
        ++idx;
    }

    while (idx + n < expr.size() - 1) {
        if (expr[idx] != "V" && expr[idx] != "H")
            return;
        else
            expr[idx] = expr[idx] == "V" ? "H" : "V";
        ++idx;
    }
}

int FloorPlan::M3(vector<string>& expr, int idx) {
    int numOperand = 0;
    int numOperator = 0;
    int i;

    for (i = 0; i < idx; ++i) {
        if (expr[i] == "V" || expr[i] == "H")
            ++numOperator;
        else
            ++numOperand;
    }
    for (i = idx; i < expr.size() - 1; ++i) {
        if (expr[i] == "V" || expr[i] == "H")
            ++numOperator;
        else
            ++numOperand;

        if ((expr[i] == "V" || expr[i] == "H") && (expr[i + 1] != "V" && expr[i + 1] != "H")) {
            swap(expr[i], expr[i + 1]);
            return i;
        }
        else if ((expr[i] != "V" && expr[i] != "H") && (expr[i + 1] == "V" || expr[i + 1] == "H")) {
            ++numOperator;
            //if (2 * numOperator < i + 1) {
            if (2 * numOperator < i) {
                swap(expr[i], expr[i + 1]);
                return i;
            }
            break;
        }
    }

    return -1;
}

pair<int, int> FloorPlan::Polish2FloorPlan(vector<string> polishExpression) {
    int tmpCnt = 0;
    HardBlock* A = NULL;
    HardBlock* B = NULL;
    vector<HardBlock*> stack;
    vector<HardBlock*> garbage;
    pair<int, int> widthAndHeight = { 0, 0 };
    pair<int, int> tmpLowerLeft = { 0, 0 };
    pair<int, int> wA = { 0, 0 };
    pair<int, int> wB = { 0, 0 };

    HardBlock* tmp = new HardBlock("tmp" + to_string(tmpCnt++));
    garbage.push_back(tmp);

    for (auto i : hardblocks)
        i.second->SetLowerLeftCorner({ 0, 0 });

    for (int i = 0; i < polishExpression.size(); ++i) {
        if (polishExpression[i] == "V") {
            B = stack.back();
            stack.pop_back();
            A = stack.back();
            stack.pop_back();
            tmp = new HardBlock("tmp" + to_string(tmpCnt++));
            garbage.push_back(tmp);

            if (A->GetName().substr(0, 3) != "tmp") tmp->AddContains(A->GetName());
            if (B->GetName().substr(0, 3) != "tmp") tmp->AddContains(B->GetName());

            for (auto h : A->GetAllContains())
                tmp->AddContains(h);
            for (auto h : B->GetAllContains())
                tmp->AddContains(h);

            widthAndHeight.first = A->GetWidth() + B->GetWidth();
            widthAndHeight.second = max(A->GetHeight(), B->GetHeight());

            tmpLowerLeft = A->GetLowerLeftCorner();
            for (auto h : B->GetAllContains()) {
                hardblocks[h]->AddLowerLeftCornerWidth(A->GetWidth());
            }

            B->SetLowerLeftCorner({ tmpLowerLeft.first + A->GetWidth(), tmpLowerLeft.second });
            tmp->SetWidthAndHeight(widthAndHeight);

            stack.push_back(tmp);
        }
        else if (polishExpression[i] == "H") {
            B = stack.back();
            stack.pop_back();
            A = stack.back();
            stack.pop_back();
            tmp = new HardBlock("tmp" + to_string(tmpCnt++));
            garbage.push_back(tmp);

            if (A->GetName().substr(0, 3) != "tmp") tmp->AddContains(A->GetName());
            if (B->GetName().substr(0, 3) != "tmp") tmp->AddContains(B->GetName());

            for (auto h : A->GetAllContains())
                tmp->AddContains(h);
            for (auto h : B->GetAllContains())
                tmp->AddContains(h);
            // do
            widthAndHeight.second = A->GetWidthAndHeight().second + B->GetWidthAndHeight().second;
            widthAndHeight.first = max(A->GetWidthAndHeight().first, B->GetWidthAndHeight().first);

            tmpLowerLeft = A->GetLowerLeftCorner();

            for (auto h : B->GetAllContains()) {
                hardblocks[h]->AddLowerLeftCornerHeight(A->GetHeight());
            }
            B->SetLowerLeftCorner({ tmpLowerLeft.first, tmpLowerLeft.second + A->GetHeight() });

            tmp->SetWidthAndHeight(widthAndHeight);

            stack.push_back(tmp);
        }
        else {
            stack.push_back(hardblocks[polishExpression[i]]);
        }
    }

    for (auto i : garbage)
        delete i;

    return widthAndHeight;
}

bool cmpShape(vector<int> A, vector<int> B) {
    return A[1] > B[1];
}

pair<int, int> FloorPlan::InitialSlicingTree(vector<string>& expr) {
    vector<SlicingTreeNode*> s;
    SlicingTreeNode* node;
    SlicingTreeNode* lnode;
    SlicingTreeNode* rnode;
    pair<int, int> w;
    int cnt = 1;

    vector<SlicingTreeNode*> garbage;

    for (auto i : expr) {
        if (i != "V" && i != "H") {
            //operand
            node = new SlicingTreeNode(hardblocks[i], HARDBLOCK);
            garbage.push_back(node);

            w = hardblocks[i]->GetWidthAndHeight();
            node->SetWidth(w.first);
            node->SetHeight(w.second);
            if (w.first < w.second) {
                node->AddShape({ w.first, w.second, -1, -1 });
                node->AddShape({ w.second, w.first, -1, -1 });
            }
            else if (w.first > w.second) {
                node->GetNode()->RevRotated();
                node->AddShape({ w.second, w.first, -1, -1 });
                node->AddShape({ w.first, w.second, -1, -1 });
            }
            else if (w.first == w.second) {
                node->AddShape({ w.first, w.second, -1, -1 });
            }
            s.push_back(node);
        }
        else {
            //operator
            nodeType type = (i == "V") ? VERTICALCUT : HORIZONTALCUT;
            node = new SlicingTreeNode(i + to_string(cnt++), type);
            garbage.push_back(node);

            rnode = s.back();
            s.pop_back();
            node->SetRnode(rnode);
            rnode->SetParent(node);
            
            lnode = s.back();
            s.pop_back();
            node->SetLnode(lnode);
            lnode->SetParent(node);

            /*
             *vector<vector<int>>& lshape = lnode->GetShapes();
             *vector<vector<int>>& rshape = rnode->GetShapes();
             */
            pair<int, int> tmpW;
            pair<int, int> minW = { INT_MAX, INT_MAX };
            pair<int, int> child;
            int tmpArea;
            int minArea = INT_MAX;

            /*
             *sort(lshape.begin(), lshape.end(), cmpShape);
             *sort(rshape.begin(), rshape.end(), cmpShape);
             */

            /*
             *if (type == VERTICALCUT) {
             *    int i = 0, j = 0;
             *    while (i < lshape.size() && j < rshape.size()) {
             *        tmpW.first = lshape[i][0] + rshape[j][0];
             *        tmpW.second = max(lshape[i][1], rshape[j][1]);
             *        node->AddShape({ tmpW.first, tmpW.second, i , j });
             *        if (lshape[i][1] > rshape[j][1]) {
             *            ++i;
             *        }
             *        else if (lshape[i][1] < rshape[j][1]) {
             *            ++j;
             *        }
             *        else{
             *            ++i;
             *            ++j;
             *        }
             *        break;
             *    }
             *}
             *else if (type == HORIZONTALCUT){
             *    int i = lshape.size() - 1, j = rshape.size() - 1;
             *    while (i > -1 && j > -1) {
             *        tmpW.first = max(lshape[i][0], rshape[j][0]);
             *        tmpW.second = lshape[i][1] + rshape[j][1];
             *        node->AddShape({ tmpW.first, tmpW.second, i , j });
             *        if (lshape[i][0] > rshape[j][0]) {
             *            --i;
             *        }
             *        else if (lshape[i][0] < rshape[j][0]) {
             *            --j;
             *        }
             *        else{
             *            --i;
             *            --j;
             *        }
             *        break;
             *    }
             *}
             */

            s.push_back(node);
        }
    }

    node = s.back();
    s.pop_back();
    pair<int, int> r = { 0, 0 };

    /*
     *vector<vector<int>> tmpShape = node->GetShapes();
     *pair<int, int> option;
     *int minArea = INT_MAX;
     *int m = 0;
     */
    pair<int, int> b = { 500, 500 };
    /*
     *for (int i = 0 ; i < tmpShape.size(); ++i) {
     *    int area = tmpShape[i][0] * tmpShape[i][1];
     *    if (area < minArea) {
     *        minArea = area;
     *        b = { tmpShape[i][0], tmpShape[i][1] };
     *        m = i;
     *    }
     *}
     */
    /*
     *node->SetMin(m);
     *node->BackTrack();
     */

    //cout << "SetPosition" << endl;
    //node->SetBase({ 0, 0 });
    //r = node->InOrderSetPosition();

    for (auto i : garbage)
        delete i;

    return b;

    return r;
}

int FloorPlan::HPWL() {
    int wirelength = 0;
    pair<int, int> tmp;

    for (auto i : nets) {
        pair<int, int> lowerLeftCorner = { INT_MAX, INT_MAX };
        pair<int, int> upperRightCorner = { 0, 0 };

        for (auto j : i->GetPins()) {

            if (j.substr(0, 2) == "sb") {
                tmp = hardblocks[j]->GetCenter();
            }
            else {
                tmp = terminals[j]->GetPosition();
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
        wirelength += tmpWL;
    }

    this->wirelength = wirelength;
    return wirelength;
}
