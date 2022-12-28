#include "placement.h"

extern bool DEBUG;
extern steady_clock::time_point tStart;
int numPlaceRow = 0;
int numPlaceRowSuccess = 0;
int numStore = 0;
int numRestore = 0;

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
                     
    Row* row = NULL;

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
                    this->rowHeight = height; 
                    this->siteWidth = siteWidth;
                }

                row = new Row({ subrowOrigin, coordinate }, height, siteWidth, numSites);
                garbage.push_back(row);
                rowIdx = (coordinate - base.second) / rowHeight;
                rows[rowIdx].insert(row);
            }
            ++cnt;
        }
        cout << "Parse .scl file successfully" << endl;
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

bool cmpRow(Row*& a, Row*& b) {
    return a->GetX() < b->GetX();
}

void Placement::SplitSubRow() {
    int terminalCnt = 0;

    cout << "SplitSubRow(): # of terminals = " << terminals.size() << endl;
    cout << "SplitSubRow(): # of rows before split = " << rows.size() << endl;

    for (auto &terminal : terminals) {
        pair<double, double> terminalCoor = terminal.second->GetOriginalCoor();
        int startIdx = (terminalCoor.second - base.second) / rowHeight;
        int n = (terminal.second->GetHeight() / rowHeight);

        if (DEBUG) {
            cout << KCYN << "[" << terminalCnt++ << "] " << RST;
            cout << terminal.first << " | ";
            cout << "h: " << terminal.second->GetHeight() << " | ";
            cout << terminalCoor.second << " - " << base.second << " = ";
            cout << (terminalCoor.second - base.second) << " | ";

            cout << KGRN << "startIdx: " << startIdx << ", ";
            cout << "endIdx: " << startIdx + n - 1 << RST << " | ";
            cout << "n: " << n << endl;
        }

        for (int i = startIdx; i < startIdx + n; ++i) {
            for (set<Row*>::iterator iter = rows[i].begin(); iter != rows[i].end();)  {
                // check if terminal overlap with subrow
                //if (terminalCoor.first < (*iter)->GetX() + (*iter)->GetNumSites() * (*iter)->GetSiteWidth() && terminalCoor.first >= (*iter)->GetX()) {
                if (terminalCoor.first < (*iter)->GetRightX() && terminalCoor.first >= (*iter)->GetX()) {

                    int numSitesTerminal = terminal.second->GetWidth() / (*iter)->GetSiteWidth();
                    int numSitesRemain = (*iter)->GetNumSites() - numSitesTerminal;

                    int n1 = (terminalCoor.first - (*iter)->GetX()) / (*iter)->GetSiteWidth();
                    int n2 = (*iter)->GetNumSites() - n1 - numSitesTerminal;
                    pair<int, int> p1 = (*iter)->GetCoordinate();
                    pair<int, int> p2 = { terminalCoor.first + terminal.second->GetWidth(), (*iter)->GetY() };

                    if (n1 > 0) {
                        Row* subrow1 = new Row(p1, (*iter)->GetHeight(), (*iter)->GetSiteWidth(), n1);
                        rows[i].insert(subrow1);
                        garbage.push_back(subrow1);
                    }

                    if (n2 > 0) {
                        Row* subrow2 = new Row(p2, (*iter)->GetHeight(), (*iter)->GetSiteWidth(), n2);
                        rows[i].insert(subrow2);
                        garbage.push_back(subrow2);
                    }

                    iter = rows[i].erase(iter);
                    //break;
                }
                else {
                    ++iter;
                }
            }
        }
    }

    int cnt = 0;
    int max = 0;
    for (auto i : rows) {
        if (max < i.second.size())
            max = i.second.size();

        cnt += i.second.size();
    }
    cout << "SplitSubRow(): # of rows after split = " << cnt << endl;
    cout << "SplitSubRow(): max # of subrow = " << max << endl;

    /*
     *cout << "Check Subrow" << endl;
     *for (auto row : rows) {
     *    vector<Row*> pool;
     *    for (auto subrow : row.second) {
     *        pool.push_back(subrow);
     *        sort(pool.begin(), pool.end(), cmpRow);
     *    }
     *    cout << pool[0]->GetX() << ", " << pool[0]->GetY() << " width = " << pool[0]->GetWidth() << endl;
     *    for (size_t i = 1; i < pool.size(); ++i) {
     *        cout << pool[i]->GetX() << ", " << pool[i]->GetY() << "width = " << pool[i]->GetWidth() << endl;
     *        if (pool[i]->GetX() < pool[i - 1]->GetRightX()) {
     *            cout << "ERROR subrow overlap" << endl;
     *        }
     *    }
     *    cout << " ---- " << endl;
     *}
     */
}

bool cmpX(Node*& a, Node*& b) {
    return a->GetCoordinate().first > b->GetCoordinate().first;
}

void Placement::Abacus() {

    int cnt = 0;
    int candidate = 0;
    int up = 0;
    int down = 0;
    double minDisplacement = 0;
    double minDeltaCost = DBL_MAX;
    double oldCost = 0;
    double newCost = 0;
    Row* minRow = NULL;
    Node* last = NULL;
    Node* current = NULL;
    vector<Node*> pool;

    for (auto i : nodes) 
        if (i.second->GetType() == MOVABLE) {
            i.second->Store();
            pool.push_back(i.second);
        }

    sort(pool.begin(), pool.end(), cmpX);

    if (DEBUG) {
        cout << "Abacus(): # of nodes (not terminal) = " << pool.size() << endl;
        cout << "Abacus(): check nodes in pool" << endl;
        for (auto i : pool) {
            pair<double, double> tmp = i->GetCoordinate();
            cout << i->GetName() << " | ("  << tmp.first << ", " << tmp.second << ") | [adajusted]";
            cout << " (" << tmp.first - base.first << ", " << tmp.second - base.second << ") | ";
            cout << "rowHeight: " << rowHeight << " | ";
            cout << fmod(tmp.second - base.second, rowHeight) << endl;
        }
    }

    current = pool.back();
    
    if (numTerminals > 0) {
        cout << "Abacus(): call SplitSubRow()" << endl;
        steady_clock::time_point tSplitRowStart = steady_clock::now();
        SplitSubRow();
        steady_clock::time_point tSplitRowEnd = steady_clock::now();
        double splitRowTime = (double) duration_cast<microseconds>(tSplitRowEnd - tSplitRowStart).count() / 1000000;
        cout << "Total Time of SplitRow: " << splitRowTime << endl;
    }

    cout << "---------------------------[start]-------------------------" << endl;

    while (!pool.empty()) {
        //start trying from the closest row
        candidate = round((current->GetY() - base.second) / rowHeight);

        up = candidate;
        down = candidate - 1;
        minDisplacement = INT_MAX;
        minDeltaCost = DBL_MAX;
        minRow = NULL;

        while (up < (int) rows.size()) {
            for (auto currentRow : rows[up]) {
                if (abs(currentRow->GetY() - current->GetOriginalY()) > minDeltaCost)
                    break;

                /*
                 *currentRow->Store();
                 *current->Store();
                 */

                pair<bool, double> result = PlaceRow(currentRow, current);
                if (result.first) {
                    ++numPlaceRowSuccess;
                    //cout << current->GetName() << " Success! Displacement: " << current->GetDisplacement() << endl;
                    newCost = currentRow->Cost(false) + current->GetDisplacement(false);
                    if (newCost - currentRow->GetCost() < minDeltaCost) {
                        minDeltaCost = newCost - currentRow->GetCost();
                        minRow = currentRow;
                    }

                    /*
                     *if (current->GetX() % siteWidth != 0) {
                     *    cout << KRED << "ERROR does not align to site: ";
                     *    cout << current->GetName() << " " << current->GetX() << " " << current->GetY() << endl;
                     *    exit(-1);
                     *}
                     */
                }

                currentRow->Restore();
                current->Restore();
            }
            ++up;
        }

        while (down > -1) {
            for (auto currentRow : rows[down]) {
                if (abs(currentRow->GetY() - current->GetOriginalY()) > minDeltaCost)
                    break;

                /*
                 *currentRow->Store();
                 *current->Store();
                 */

                pair<bool, double> result = PlaceRow(currentRow, current);
                if (result.first) {
                    ++numPlaceRowSuccess;
                    //cout << current->GetName() << " Success! Displacement: " << current->GetDisplacement() << endl;
                    newCost = currentRow->Cost(false) + current->GetDisplacement(false);
                    if (newCost - currentRow->GetCost() < minDeltaCost) {
                        minDeltaCost = newCost - currentRow->GetCost();
                        minRow = currentRow;
                    }
                    /*
                     *if (current->GetX() % siteWidth != 0) {
                     *    cout << KRED << "ERROR does not align to site: ";
                     *    cout << current->GetName() << " " << current->GetX() << " " << current->GetY() << endl;
                     *    exit(-1);
                     *}
                     */
                }

                currentRow->Restore();
                current->Restore();
            }
            --down;
        }

        if (minRow) {
            //cout << "[Place to minRow] " << current->GetName() << endl;
            PlaceRow(minRow, current);
            minRow->AddCell(current);
            minRow->Cost(true);
            minRow->Store();
            
            if (minRow->GetFront()->GetX() + minRow->GetFront()->GetClusterWidth() > minRow->GetRightX()) {
                cout << "ERROR" << endl;
                cout << current->GetName() << " " << current->GetX() << " | " << current->GetRightX() << endl;
                cout << "cluster x: " << minRow->GetFront()->GetX() << endl;
                cout << "cluster width: " << minRow->GetFront()->GetClusterWidth() << endl;
                cout << "cluster right x: " << minRow->GetFront()->GetX() + minRow->GetFront()->GetClusterWidth() << endl;
                cout << "last node right x: " << minRow->GetLast()->GetRightX() << endl;
                minRow->Print();

                exit(-1);
            }

            //cout << "Finish Place to minRow: " << current->GetName() << endl;
            //minRow->Print();
        }
        else {
            cout << "Error: There is no minRow for " << current->GetName() << " " << current->GetWidth() << endl;
        }

        last = current;
        pool.pop_back();
        if (!pool.empty())
            current = pool.back();
    }
}

pair<bool, int> Placement::PlaceRow(Row* row, Node* node) {
    ++numPlaceRow;
    /*
     *cout << "--- " << endl << "PlaceRow(): " << node->GetName() << endl;
     *cout << "PlaceRow(): x boundary = " << row->GetX() << endl;
     *cout << "PlaceRow(): row width = " << row->GetWidth() << endl;
     *cout << "PlaceRow(): original position (" << node->GetX() << ", " << node->GetY() << ")" << endl;
     */
    vector<Node*>& cells = row->GetCells();
    Node* last = NULL;
    Node* current = NULL;
    Node* backup = node;

    //find closestSite x of the node in this row
    int closestSite = round((node->GetX() - row->GetX()) / siteWidth);
    int x = row->GetX() + closestSite * siteWidth;

    if (node->GetWidth() > row->GetWidth()) {
        return { false, 0 };
    }

    //need to consider 3 types of situation
    // 1. [ cell ]
    // 2. cell [ ]
    // 3. [ ] cell
    if (x < row->GetX())
        x = row->GetX();
    else if (x + node->GetWidth() > row->GetRightX())
        x = row->GetRightX() - node->GetWidth();

    /*
     *cout << "PlaceRow(): closest x position = " << x << endl;
     *cout << "PlaceRow(): # of cells in the row = " << cells.size() << endl;
     */

    if (cells.size() == 0) {
        //cout << "directly set x = " << x << endl;
        node->SetCoordinate({ x, row->GetY() });
        return { true, 0 };
    }

    for (int i = cells.size() - 1; i > -1; --i) {
        int move = 0;
        last = cells[i];
        /*
         *cout << "Check: " << last->GetName()  << " with " << node->GetName() << endl;
         *cout << last->GetName() << " last: " << "x/w " << last->GetX() << " " << last->GetWidth() << " " << last->GetX() + last->GetWidth() << endl;
         *cout << node->GetName() << " node: " << "x/w " << node->GetX() << " " << node->GetWidth() << " " << node->GetX() + node->GetWidth() << endl;
         */
        closestSite = round((node->GetX() - row->GetX()) / siteWidth);
        x = row->GetX() + closestSite * siteWidth;

        if (x < row->GetX())
            x = row->GetX();
        else if (x + node->GetWidth() > row->GetRightX())
            x = row->GetRightX() - node->GetWidth();
        
        //node does not overlap with the last node 
        if (last->GetX() + last->GetWidth() <= x) {
            /*
             *cout << "node does not overlap with the last cell of the row => directly set coordinate" << endl;
             *cout << "set x = " << x << endl;
             */
            //cout << x + node->GetWidth() << endl;
            node->SetCoordinate({ x, row->GetY() });
            break;
        }
        //node overlaps with the last node
        else {
            //cluster with previous cell
            last->SetRight(node);
            node->SetLeft(last);
            //cout << node->GetName() << " overlaps with " << last->GetName() << endl;
            //cout << node->GetName() << " " << node->GetX() << " " << node->GetY() << endl;

            //if previous cell is in a cluster => move current to cluster head
            //if previous cell is not in a cluster => it will stay at last
            while (last->GetLeft() != NULL) {
                ++move;
                last = last->GetLeft();
            }
            //cout << row->GetY() << " clusterHead = " << last->GetName() << endl;

            if (!last->IsClusterHead()) 
                last->SetIsClusterHead();

            int x1 = last->GetX();
            int x2 = node->GetX();
            int e1 = last->GetClusterWeight();
            int e2 = node->GetClusterWeight();
            int w1 = last->GetClusterWidth();
            int w2 = node->GetClusterWidth();
            int newX = round((double) (e1 * x1 + e2 * (x2 - w1)) / (double) (e1 + e2));

            if (newX + w1 + w2 > row->GetRightX())
                newX = row->GetRightX() - w1 - w2;

            last->SetX(newX);
            last->SetClusterWeight(e1 + e2);
            last->SetClusterWidth(w1 + w2);
        }
        i -= move;
        node = last;
    }
    
    //align the first cell of the row
    closestSite = round((row->GetFront()->GetX() - row->GetX()) / siteWidth);
    x = row->GetX() + closestSite * row->GetSiteWidth();
    if (x < row->GetX()) {
        x = row->GetX();
    }
    row->GetFront()->SetX(x);
    row->AlignCluster();

    node = backup;

    if (row->GetFront()->GetX() + row->GetFront()->GetClusterWidth() > row->GetRightX()) {
        //cout << "PlaceRow(): Exceed right boundary " << node->GetName() << endl;
        return { false, 0 };
    }

    return { true, 0 };
}

void Placement::Check() {
    int numCells = 0;
    int numError = 0;
    int cnt = 0;

    for (auto row : rows) {
        for (auto subrow : row.second) {
            vector<Node*> cells = subrow->GetCells();
            numCells += cells.size();
            ++cnt;
            //cout << cnt++ << " whiteSpace = " << subrow->GetWhiteSpace() << endl;
            for (size_t i = 0; i < cells.size(); ++i) {
                if (cells[i]->GetX() < subrow->GetX()) {
                    cout << "ERROR: " << cells[i]->GetName() << " exceeds left boundary (";
                    cout << cells[i]->GetX() << ", " << cells[i]->GetY() << ")" << endl;
                    ++numError;
                }
                if (cells[i]->GetRightX() > subrow->GetRightX()) {
                    cout << "ERROR: " << cells[i]->GetName() << " exceeds right boundary (";
                    cout << cells[i]->GetX() << ", " << cells[i]->GetY() << ")" << endl;
                    cout << "right = " << cells[i]->GetX() + cells[i]->GetWidth() << endl;
                    cout << "right boundary = " << subrow->GetRightX() << endl;
                    ++numError;
                }
                if ((cells[i]->GetX() - subrow->GetX()) % subrow->GetSiteWidth() != 0) {
                    cout << "ERROR: " << cells[i]->GetName() << " does not align with any site";
                    cout << cells[i]->GetX() << ", " << cells[i]->GetY() << endl;
                    ++numError;
                }
                if (i > 0) {
                    if (cells[i]->GetX() < cells[i - 1]->GetRightX()) {
                        cout << "ERROR: " << cells[i]->GetName() << " overlaps with ";
                        cout << cells[i - 1]->GetName() << endl;
                        ++numError;
                    }
                }
            }
            //subrow->Print();
        }
    }

    cout << "Total # of cells for Check(): " << numCells << endl;
    cout << "Total # of cells from IO: " << numNodes - numTerminals << endl;
    cout << "Total # of PlaceRow: " << numPlaceRow << endl;
    cout << "Total # of PlaceRow SUCCESS: " << numPlaceRowSuccess << endl;
    cout << "Total # of PlaceRow FAILED: " << numPlaceRow - numPlaceRowSuccess << endl;
    cout << "Total # of Store(): " << numStore << endl;
    cout << "Total # of Restore(): " << numRestore << endl;
    cout << "Total # of ERROR: " << numError << endl;
    cout << (double) duration_cast<microseconds>(steady_clock::now() - tStart).count() / 1000000 << endl;
}
