#include "placement.h"

extern bool DEBUG;

void Placement::ReadInputFile(const char* filename) {
    cout << filename << endl;
    int end = string(filename).find_last_of("/");
    string path = string(filename).substr(0, end + 1);
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
        for (int i = 0; i < 3; ++i) {
            f >> line;
            switch (i) {
                case 0:
                    nodeFile = path + line;
                    break;
                case 1:
                    plFile = path + line;
                    break;
                case 2:
                    sclFile = path + line;
                    break;
            }
        }
        f >> line;
        f >> line;
        f >> maxDisplacement;
        cout << "MaxDisplacement: " << maxDisplacement << endl;

        cout << "Read .aux file successfully" << endl;
        cout << "Trying to readfile: " << endl;
        cout << "  " << nodeFile << endl;
        cout << "  " << plFile << endl;
        cout << "  " << sclFile << endl;

        ReadInputSclFile(sclFile);
        ReadInputNodeFile(nodeFile);
        ReadInputPlFile(plFile);

        cout << endl << "ReadInputFile(): " << endl;
        cout << "  NumRows: " << numRows << " | " << rows.size() << endl;
        cout << "  NumNodes: " << numNodes << " | " << nodes.size() << endl;
        cout << "    NumNodes (not terminal): " << nodes.size() - terminals.size() << endl;
        cout << "    NumTerminals: " << numTerminals << " | " << terminals.size() << endl;
    }
    f.close();
}

void Placement::ReadInputNodeFile(string filename) {
    ifstream f(filename);
    string line;
    string name;
    char buffer[1024];
    char colon;
    char* token = NULL;
    const char s[2] = " ";
    int width;
    int height;
    int cnt = 0;
    Node* node = NULL;

    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        cout << "Read .node file successfully" << endl;
    
        f >> line >> colon >> numNodes;
        f >> line >> colon >> numTerminals;

        while (f.getline(buffer, 1024)) {
            cnt = 0;
            token = strtok(buffer, s);
            while (token != NULL) {
                switch (cnt) {
                    case 0:
                        name = string(token);
                        break;
                    case 1:
                        width = atoi(token);
                        break;
                    case 2:
                        height = atoi(token);
                        node = new Node(name, width, height);
                        nodes.insert({ name, node });
                        break;
                    case 3:
                        node->SetMoveType(TERMINAL);
                        terminals.insert({ name, node });
                        break;
                }
                ++cnt; 
                token = strtok(NULL, s);
            }
        }
        if (DEBUG) {
            for (auto i : nodes) {
                cout << i.first << " | " << i.second->GetName() << " " << i.second->GetWidth() << " " << i.second->GetHeight() << " " << i.second->GetType() << endl;
            }
            cout << nodes.size() << endl;
        }
        cout << "Parse .node file successfully" << endl;
    }
    f.close();
}

void Placement::ReadInputPlFile(string filename) {
    ifstream f(filename);
    string name;
    char buffer[1024];
    char* token = NULL;
    const char s[2] = " ";
    int cnt = 0;
    pair<double, double> coordinates;

    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        cout << "Read .pl file successfully" << endl;
        while (f.getline(buffer, 1024)) {
            cnt = 0;
            token = strtok(buffer, s);
            while (token != NULL) { 
                //cout << token << " ";
                switch (cnt) {
                    case 0:
                        name = string(token);
                        break;
                    case 1:
                        coordinates.first = atof(token);
                        break;
                    case 2:
                        coordinates.second = atof(token);
                        nodes[name]->SetOriginalCoor(coordinates);
                        break;
                }
                ++cnt;
                token = strtok(NULL, s);
            } 
            //cout << endl;
        }
        if (DEBUG) {
            for (auto i : nodes) {
                cout << i.first << " " << i.second->GetCoordinate().first << " " << i.second->GetCoordinate().second << endl;
            }
        }
        cout << "Parse .pl file successfully" << endl;
    }
    f.close();
}
void Placement::ReadInputSclFile(string filename) {
    ifstream f(filename);
    string line;
    char colon;
    int cnt = 0;
    int rowIdx = 0;

    int coordinate; //y-coordinate of row
    int height;     //row height
    int siteWidth;  //width of a site
    int numSites;   //number of sites
    int subrowOrigin; // x-coordinate of row

    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        cout << "Read .scl file successfully" << endl;
        f >> line >> colon >> numRows;
        while (f >> line) {
            if (line.size() > 0 && line == "CoreRow") {
                f >> line;
                f >> line >> colon >> coordinate;
                f >> line >> colon >> height;
                f >> line >> colon >> siteWidth;
                f >> line >> colon >> numSites;
                f >> line >> colon >> subrowOrigin;
                f >> line;

                if (DEBUG) {
                    cout << "Coordinate : " << coordinate << endl;
                    cout << "Height : " << height << endl;
                    cout << "Sitewidth : " << siteWidth << endl;
                    cout << "NumSites : " << numSites << endl;
                    cout << "SubrowOrigin : " << subrowOrigin << endl;
                    cout << endl;
                }

                if (cnt == 0) {
                    base = { subrowOrigin, coordinate };
                    rowHeight = height; 
                }

                Row* row = new Row({ subrowOrigin, coordinate }, height, siteWidth, numSites);
                garbage.push_back(row);
                rowIdx = (coordinate - base.second) / rowHeight;
                rows[rowIdx].insert(row);
            }
            ++cnt;
        }
        cout << "Parse .scl file successfully" << endl;

        if (DEBUG) {
            /*
             *for (auto i : rows) {
             *    pair<int, int> tmp = i->GetCoordinate();
             *    cout << "Coordinate : " << tmp.second << endl;
             *    cout << "Height : " << i->GetHeight() << endl;
             *    cout << "Sitewidth : " << i->GetSiteWidth() << endl;
             *    cout << "NumSites : " << i->GetNumSites() << endl;
             *    cout << "SubrowOrigin : " << tmp.first << endl;
             *    cout << endl;
             *}
             */
        }
    }
    f.close();
}

void Placement::GenOutputFile(const char* filename) {
    ofstream f(filename);
    if (f.fail()) {
        cout << "Failed to open file: " << filename << endl;
        exit(-1);
    }
    else {
        for (auto i : nodes) {
            pair<double, double> coordinates = i.second->GetCoordinate();
            f << i.first << " " << coordinates.first << " " << coordinates.second << endl;
        }
    }
    f.close();
}

double Placement::Cost() {
    double totalDisplacement = 0;

    for (auto node : nodes)
        totalDisplacement += node.second->GetDisplacement();

    return totalDisplacement;
}

void Placement::SplitSubRow() {
    int terminalCnt = 0;

    cout << "SplitSubRow(): # of terminals = " << terminals.size() << endl;
    cout << "SplitSubRow(): # of rows before split = " << rows.size() << endl;

    for (auto &terminal : terminals) {
        pair<double, double> terminalCoor = terminal.second->GetOriginalCoor();
        int startIdx = (terminalCoor.second - base.second) / rowHeight;
        int n = (terminal.second->GetHeight() / rowHeight);
        int numOverlap = 0;

        cout << KCYN << "[" << terminalCnt++ << "] " << RST;
        cout << terminal.first << " | ";
        cout << "h: " << terminal.second->GetHeight() << " | ";
        cout << terminalCoor.second << " - " << base.second << " = ";
        cout << (terminalCoor.second - base.second) << " | ";

        cout << KGRN << "startIdx: " << startIdx << ", ";
        cout << "endIdx: " << startIdx + n - 1 << RST << " | ";
        cout << "n: " << n << endl;

        for (int i = startIdx; i < startIdx + n; ++i) {

            cout << "rows size: " << rows[i].size() << endl;
            int subrowCnt = 0;
            for (auto iter : rows[i]) {

                //cout << (iter->GetY() - base.second) / rowHeight << endl;
                // check if terminal overlap with subrow
                cout << subrowCnt++ << " | ";
                cout << terminalCoor.first << " " << iter->GetX() + iter->GetNumSites() * iter->GetSiteWidth() << " | ";
                if (terminalCoor.first <= iter->GetX() + iter->GetNumSites() * iter->GetSiteWidth() && terminalCoor.first >= iter->GetX()) {

                    int numSitesTerminal = terminal.second->GetWidth() / iter->GetSiteWidth();
                    int numSitesRemain = iter->GetNumSites() - numSitesTerminal;

                    int n1 = (terminalCoor.first - iter->GetX()) / iter->GetSiteWidth();
                    int n2 = iter->GetNumSites() - n1 - numSitesTerminal;
                    pair<int, int> p1 = iter->GetCoordinate();
                    pair<int, int> p2 = { terminalCoor.first + terminal.second->GetWidth(), terminalCoor.second };

                    cout << (iter->GetY() - base.second) / rowHeight << " ";
                    cout << iter->GetNumSites() << " - " << numSitesTerminal << " = " << numSitesRemain << " | " << KGRN << "[check] ";
                    cout << "n1 = " << n1 << " | " << numSitesTerminal << " | " << "n2 = " << n2 << " | n1 + n2 = " << n1 + n2;
                    cout << RST << endl;

                    if (n1 > 0) {
                        Row* subrow1 = new Row(p1, iter->GetHeight(), iter->GetSiteWidth(), n1);
                        rows[i].insert(subrow1);
                        garbage.push_back(subrow1);
                    }

                    if (n2 > 0) {
                        Row* subrow2 = new Row(p2, iter->GetHeight(), iter->GetSiteWidth(), n2);
                        rows[i].insert(subrow2);
                        garbage.push_back(subrow2);
                    }

                    //iter = rows[i].erase(iter);
                    rows[i].erase(iter);

                    //++numOverlap;
                    break;
                }
                else
                    cout << (iter->GetY() - base.second) / rowHeight << " NO" << endl;
            }
/*
 *            for (set<Row*>::iterator iter = rows[i].begin(); iter != rows[i].end();)  {
 *
 *                // check if terminal overlap with subrow
 *                if (terminalCoor.first <= (*iter)->GetX() + (*iter)->GetNumSites() * (*iter)->GetSiteWidth() && terminalCoor.first > (*iter)->GetX()) {
 *
 *                    int numSitesTerminal = terminal.second->GetWidth() / (*iter)->GetSiteWidth();
 *                    int numSitesRemain = (*iter)->GetNumSites() - numSitesTerminal;
 *
 *                    int n1 = (terminalCoor.first - (*iter)->GetX()) / (*iter)->GetSiteWidth();
 *                    int n2 = (*iter)->GetNumSites() - n1 - numSitesTerminal;
 *                    pair<int, int> p1 = (*iter)->GetCoordinate();
 *                    pair<int, int> p2 = { terminalCoor.first + terminal.second->GetWidth(), terminalCoor.second };
 *
 *                    cout << ((*iter)->GetY() - base.second) / rowHeight << " ";
 *                    cout << (*iter)->GetNumSites() << " - " << numSitesTerminal << " = " << numSitesRemain << " | " << KGRN << "[check] ";
 *                    cout << "n1 = " << n1 << " | " << numSitesTerminal << " | " << "n2 = " << n2 << " | n1 + n2 = " << n1 + n2;
 *                    cout << RST << endl;
 *
 *                    if (n1 > 0) {
 *                        Row* subrow1 = new Row(p1, (*iter)->GetHeight(), (*iter)->GetSiteWidth(), n1);
 *                        rows[i].insert(subrow1);
 *                        garbage.push_back(subrow1);
 *                    }
 *
 *                    if (n2 > 0) {
 *                        Row* subrow2 = new Row(p2, (*iter)->GetHeight(), (*iter)->GetSiteWidth(), n2);
 *                        rows[i].insert(subrow2);
 *                        garbage.push_back(subrow2);
 *                    }
 *
 *                    iter = rows[i].erase(iter);
 *
 *                    break;
 *                }
 *                else {
 *                    ++iter;
 *                }
 *            }
 */
        }
        
        //if (terminalCnt == 2)
            //break;
    }

    int cnt = 0;
    int max = -1;
    cout << "SUBROW SIZE" << endl;
    for (auto &i : rows) {
        cnt += i.second.size();
    }
    cout << "max # of subrow = " << max << endl;
    cout << "SplitSubRow(): # of rows after split = " << cnt << endl;
}

bool cmpX(Node*& a, Node*& b) {

    pair<double, double> pA = a->GetCoordinate();
    pair<double, double> pB = b->GetCoordinate();

    return (pA.first != pB.first) ? pA.first > pB.first :
        pA.second > pB.second;

    //return (pA.second != pB.second) ? pA.second > pB.second :
        //pA.first > pB.first;

    //return a->GetCoordinate().first > b->GetCoordinate().first;
}

void Placement::Abacus() {

    // check terminals
    cout << "[Abacus]" << endl;

    cout << "Abacus(): Add nodes to pool" << endl;

    int cnt = 0;
    int candidate = 0;
    int minRow = 0;
    bool up = true;
    double minDisplacement = 0;
    double oldCost = 0;
    double newCost = 0;
    Node* current = NULL;
    vector<Node*> pool;

    for (auto i : nodes) 
        if (i.second->GetType() == MOVABLE)
            pool.push_back(i.second);

    cout << "Abacus(): # of nodes (not terminal) = " << pool.size() << endl;
    sort(pool.begin(), pool.end(), cmpX);

    cout << "Abacus(): check nodes in pool" << endl;
    for (auto i : pool) {
        pair<double, double> tmp = i->GetCoordinate();
        cout << i->GetName() << " | ("  << tmp.first << ", " << tmp.second << ") | [adajusted]";
        cout << " (" << tmp.first - base.first << ", " << tmp.second - base.second << ") | ";
        cout << "rowHeight: " << rowHeight << " | ";
        cout << fmod(tmp.second - base.second, rowHeight) << endl;
    }
    cout << "Abacus(): check completed" << endl;

    current = pool.back();
    
    if (numTerminals > 0) {
        cout << "Abacus(): call SplitSubRow()" << endl;
        SplitSubRow();
        cout << "Abacus(): Split subrow completed" << endl;
    }

    cout << endl << "[START ABACUS]" << endl;

    oldCost = Cost();
    while (!pool.empty()) {
        //start trying from the closest row
        candidate = ChooseCandidate(current);
        minDisplacement = INT_MAX;
/*
 *        while (true) {
 *
 *            pair<bool, double> result = PlaceRow(candidate, current);
 *            if (result.first) {
 *                if (current->GetDisplacement() < minDisplacement) {
 *                    minDisplacement = current->GetDisplacement();
 *                    minRow = candidate;
 *                }    
 *            }
 *
 *            if (up)
 *                ++candidate;
 *            else
 *                --candidate;
 *        }
 */

        pool.pop_back();
        current = pool.back();

        if (cnt++ == 0)
            break;
    }
    //PrintRow();
}

// return the closest row idx of the node
int Placement::ChooseCandidate(Node* node) {
    pair<double, double> tmp = node->GetCoordinate();
    int r;

    r = round((tmp.second - base.second) / rowHeight);

    cout << node->GetName() << " | ";
    cout << "(" << tmp.first - base.first << ", ";
    cout << tmp.second - base.second << ") | ";
    cout << (tmp.second - base.second) / rowHeight << " | ";
    cout << r << endl;

    return r;
}

pair<bool, int> Placement::PlaceRow(int idx, Node* node) {

/*
 *    if (rows[idx]->size() > 0) {
 *        Node* last = rows[idx]->GetLast();
 *        cout << "last: " << last->GetName() << " | ";
 *
 *        //if no overlap
 *        if (last->GetX() + last->GetWidth() <= node->GetX()) {
 *            cout << "no overlap" << endl;
 *
 *        }
 *        // if overlap
 *        else {
 *            cout << "overlap" << endl;
 *        }
 *    }
 *    else if (rows[idx]->size() == 0){
 *        node->SetCoordinate({ node->GetCoordinate().first , base.second + rowHeight * idx });
 *        rows[idx]->AddCell(node);
 *    }
 *    else {
 *        cout << "[ERROR] Placement::PlaceRow()" << endl;
 *    }
 */
    
    return { true, 0 };
}

void Placement::PrintRow() {
    cout << endl << "[PrintRow]" << endl;
    /*
     *for (int i = 0; i < rows.size(); ++i)
     *    if (rows[i]->size() > 0) {
     *        cout << "siteWidth: " << rows[i]->GetSiteWidth() << " | ";
     *        vector<Node*>& cells = rows[i]->GetCells();
     *        cout << i << " | ";
     *        for (auto cell : cells) {
     *            cout << "[";
     *            cout << cell->GetName() << ", ";
     *            cout << cell->GetX() << ", ";
     *            cout << cell->GetX() - base.first << "] ";
     *        }
     *        cout << endl;
     *    }
     */
}
