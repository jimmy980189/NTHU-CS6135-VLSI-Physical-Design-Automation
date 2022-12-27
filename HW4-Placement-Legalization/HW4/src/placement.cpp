#include "placement.h"

extern bool DEBUG;
int numPlaceRow = 0;

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

                Row* row = new Row({ subrowOrigin, coordinate }, height, siteWidth, numSites);
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
                if (terminalCoor.first < (*iter)->GetX() + (*iter)->GetNumSites() * (*iter)->GetSiteWidth() && terminalCoor.first >= (*iter)->GetX()) {

                    int numSitesTerminal = terminal.second->GetWidth() / (*iter)->GetSiteWidth();
                    int numSitesRemain = (*iter)->GetNumSites() - numSitesTerminal;

                    int n1 = (terminalCoor.first - (*iter)->GetX()) / (*iter)->GetSiteWidth();
                    int n2 = (*iter)->GetNumSites() - n1 - numSitesTerminal;
                    pair<int, int> p1 = (*iter)->GetCoordinate();
                    pair<int, int> p2 = { terminalCoor.first + terminal.second->GetWidth(), terminalCoor.second };

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
    cout << "SplitSubRow(): max # of subrow = " << max << endl;
    cout << "SplitSubRow(): # of rows after split = " << cnt << endl;
}

bool cmpX(Node*& a, Node*& b) {

    pair<double, double> pA = a->GetCoordinate();
    pair<double, double> pB = b->GetCoordinate();

    /*
     *return (pA.first != pB.first) ? pA.first > pB.first :
     *    pA.second > pB.second;
     */

    return a->GetCoordinate().first > b->GetCoordinate().first;
}

void Placement::Abacus() {

    // check terminals
    /*
     *cout << "[Abacus]" << endl;
     *cout << "Abacus(): Add nodes to pool" << endl;
     */

    int cnt = 0;
    int candidate = 0;
    int up = 0;
    int down = 0;
    double minDisplacement = 0;
    double oldCost = 0;
    double newCost = 0;
    Row* minRow = NULL;
    Row* currentRow = NULL;
    Node* last = NULL;
    Node* current = NULL;
    vector<Node*> pool;

    for (auto i : nodes) 
        if (i.second->GetType() == MOVABLE)
            pool.push_back(i.second);

    sort(pool.begin(), pool.end(), cmpX);

    //cout << "Abacus(): # of nodes (not terminal) = " << pool.size() << endl;

    if (DEBUG) {
        cout << "Abacus(): check nodes in pool" << endl;
        for (auto i : pool) {
            pair<double, double> tmp = i->GetCoordinate();
            cout << i->GetName() << " | ("  << tmp.first << ", " << tmp.second << ") | [adajusted]";
            cout << " (" << tmp.first - base.first << ", " << tmp.second - base.second << ") | ";
            cout << "rowHeight: " << rowHeight << " | ";
            cout << fmod(tmp.second - base.second, rowHeight) << endl;
        }
        cout << "Abacus(): check completed" << endl;
    }

    current = pool.back();
    
    if (numTerminals > 0) {
        cout << "Abacus(): call SplitSubRow()" << endl;
        SplitSubRow();
    }

    //cout << endl << "[START ABACUS]" << endl;

    while (!pool.empty()) {
        //start trying from the closest row
        //cout << "Start to find a row for " << current->GetName() << endl;
        candidate = round((current->GetY() - base.second) / rowHeight);

        up = candidate;
        down = candidate - 1;
        minDisplacement = INT_MAX;
        minRow = NULL;

        /*
         *cout << "candidate: " << candidate << endl;
         *cout << "rows[candidate].size() " << rows[candidate].size() << endl;
         *cout << "start up " << up << endl;
         */
        while (up < (int) rows.size()) {
            /*
             *cout << "up: " << up << " | ";
             *cout << "# of subrows in row = " << rows[up].size() << endl;
             */
            for (auto currentRow : rows[up]) {
                currentRow->Store();
                current->Store();

                if (abs(currentRow->GetY() - current->GetOriginalY()) > minDisplacement)
                    break;

                pair<bool, double> result = PlaceRow(currentRow, current);
                if (result.first) {
                    //cout << current->GetName() << " Success! Displacement: " << current->GetDisplacement() << endl;
                    if (current->GetDisplacement() < minDisplacement) {
                        //cout << "YES MIN" << current->GetName() << endl;
                        minDisplacement = current->GetDisplacement();
                        minRow = currentRow;
                    }    
                    if (current->GetX() % siteWidth != 0) {
                        cout << KRED << "ERROR does not align to site: ";
                        cout << current->GetName() << " " << current->GetX() << " " << current->GetY() << endl;
                        exit(-1);
                    }
                }
                else 
                    cout << current->GetName() << " Failed" << endl;

                currentRow->Restore();
                current->Restore();
            }
            ++up;
        }

        //cout << "start down: " << down << endl;
        while (down > -1) {
            /*
             *cout << "down" << down << " | ";
             *cout << "# of subrows in row = " << rows[down].size() << endl;
             */
            for (auto currentRow : rows[down]) {
                currentRow->Store();
                current->Store();

                if (abs(currentRow->GetY() - current->GetOriginalY()) > minDisplacement)
                    break;

                pair<bool, double> result = PlaceRow(currentRow, current);
                if (result.first) {
                    //cout << current->GetName() << " Success! Displacement: " << current->GetDisplacement() << endl;
                    if (current->GetDisplacement() < minDisplacement) {
                        //cout << "YES MIN " << current->GetName() << endl;
                        minDisplacement = current->GetDisplacement();
                        minRow = currentRow;
                    }    
                    if (current->GetX() % siteWidth != 0) {
                        cout << KRED << "ERROR does not align to site: ";
                        cout << current->GetName() << " " << current->GetX() << " " << current->GetY() << endl;
                        exit(-1);
                    }
                }
                else 
                    cout << current->GetName() << " Failed" << endl;

                currentRow->Restore();
                current->Restore();
            }
            --down;
        }

        //cout << "PlaceRow(): restore position (" << current->GetX() << ", " << current->GetY() << ")" << endl;

        if (minRow) {
            //cout << "[Place to minRow] " << current->GetName() << endl;
            PlaceRow(minRow, current);
            minRow->AddCell(current);
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

        //PrintRow();
        //cout << " --------------------------------------------------------------- " << endl;
    }
    //PrintRow();
}

pair<bool, int> Placement::PlaceRow(Row* row, Node* node) {
    ++numPlaceRow;
    /*
     *cout << "PlaceRow(): " << node->GetName() << endl;
     *cout << "PlaceRow(): original position (" << node->GetX() << ", " << node->GetY() << ")" << endl;
     */
    vector<Node*>& cells = row->GetCells();
    Node* last = NULL;
    Node* lastClusterHead = NULL;
    Node* previousClusterHead = NULL;
    Node* current = NULL;
    Node* backup = node;

    //find closestSite x of the node in this row
    int closestSite = round((node->GetX() - row->GetX()) / siteWidth);
    int x = row->GetX() + closestSite * siteWidth;
     
    //need to consider 3 types of situation
    // 1. [ cell ]
    // 2. cell [ ]
    // 3. [ ] cell
    if (x < row->GetX())
        x = row->GetX();
    else if (x > row->GetRightX())
        x = row->GetRightX() - node->GetX();
    
    /*
     *cout << "PlaceRow(): closest x position = " << x << endl;
     *cout << "PlaceRow(): # of cells in the row = " << cells.size() << endl;
     */

    if (cells.size() == 0) {
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
        
        //node does not overlap with the last node 
        if (last->GetX() + last->GetWidth() <= x) {
            //cout << "node does not overlap with the last cell of the row => directly set coordinate" << endl;
            node->SetCoordinate({ x, row->GetY() });
           
            // ??????
            /*
             *last->SetRight(node);
             *node->SetLeft(last);
             */

            break;
        }
        //node overlaps with the last node
        else {
            //cluster with previous cell
            last->SetRight(node);
            node->SetLeft(last);
            /*
             *cout << node->GetName() << " overlaps with " << last->GetName() << endl;
             *cout << node->GetName() << " " << node->GetX() << " " << node->GetY() << endl;
             */

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
            //cout << e1 * x1 + e2 * (x2 - w1) << endl;
            int newX = round((double) (e1 * x1 + e2 * (x2 - w1)) / (double) (e1 + e2));

            /*
             *cout << "x1 = " << x1 << endl;
             *cout << "x2 = " << x2 << endl;
             *cout << "e1 = " << e1 << endl;
             *cout << "e2 = " << e2 << endl;
             *cout << "w1 = " << w1 << endl;
             *cout << "w2 = " << w2 << endl;
             */

            //cout << "newX for " << last->GetName() << " is " << newX << endl;

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
    //cout << "align the first of the row " << row->GetFront()->GetName() << endl;
    closestSite = round((row->GetFront()->GetX() - row->GetX()) / siteWidth);
    x = row->GetX() + closestSite * row->GetSiteWidth();
    if (x < row->GetX()) {
        x = row->GetX();
    }
    //cout << "x = " << x << endl;
    row->GetFront()->SetX(x);
    
    row->AlignCluster();

    node = backup;
    /*
     *cout << "check last node: " << row->GetLast()->GetName() << " " << row->GetLast()->GetX() << endl;
     *cout << "check node: " << node->GetName() << " " << node->GetX() << endl;
     *cout << "check right: " << node->GetX() + node->GetWidth() << endl;
     */

    /*
     *if (node->GetX() + node->GetWidth() > row->GetX() + row->GetSiteWidth() * row->GetNumSites()) {
     *    cout << "PlaceRow(): Exceed right boundary " << node->GetName() << endl;
     *    return { false, 0 };
     *}
     */
    if (row->GetFront()->GetX() + row->GetFront()->GetClusterWidth() > row->GetRightX()) {
        cout << "PlaceRow(): Exceed right boundary " << node->GetName() << endl;
        return { false, 0 };
    }

    return { true, 0 };
}

void Placement::PrintRow() {
    cout << endl << KGRN << "[PrintRow]" << RST << endl;
    for (auto row : rows) {
        for (auto subrow : row.second) {
            cout << KCYN << "[" << subrow->GetX() << ", " << subrow->GetY() << "] " << RST;
            for (auto cell : subrow->GetCells()) {
                cout << cell->GetName() << ", (" << cell->GetX() << ", " << cell->GetY() << ") ";
            }
            cout << "| " << endl;
        }
    }
}

void Placement::Test() {
    
    vector<Node*> pool;
    for (auto i : nodes) 
        if (i.second->GetType() == MOVABLE)
            pool.push_back(i.second);

    sort(pool.begin(), pool.end(), cmpX);

    Row* row = (*rows[0].begin());

    Node* node = pool.back();
    while(!pool.empty()) {
        PlaceRow(row, node);
        row->AddCell(node);
        PrintRow();

        pool.pop_back();
        node = pool.back();
    }

}

void Placement::Check() {
    int numCells = 0;
    int cnt = 0;
    Node* last = NULL;
    for (auto row : rows) {
        for (auto subrow : row.second) {
            vector<Node*> cells = subrow->GetCells();
            numCells += cells.size();
            //cout << cnt++ << " whiteSpace = " << subrow->GetWhiteSpace() << endl;
            for (int i = 0; i < cells.size(); ++i) {
                if (cells[i]->GetX() < subrow->GetX()) {
                    cout << "ERROR: " << cells[i]->GetName() << " exceeds left boundary (";
                    cout << cells[i]->GetX() << ", " << cells[i]->GetY() << ")" << endl;
                    exit(-1);
                }
                if (cells[i]->GetX() + cells[i]->GetWidth() > subrow->GetX() + subrow->GetNumSites() * subrow->GetSiteWidth()) {
                    cout << "ERROR: " << cells[i]->GetName() << " exceeds right boundary (";
                    cout << cells[i]->GetX() << ", " << cells[i]->GetY() << ")" << endl;
                    cout << "right = " << cells[i]->GetX() + cells[i]->GetWidth() << endl;
                    cout << "right boundary = " << subrow->GetX() + subrow->GetNumSites() * subrow->GetSiteWidth() << endl;
                    exit(-1);
                }
                if ((cells[i]->GetX() - subrow->GetX()) % subrow->GetSiteWidth() != 0) {
                    cout << "ERROR: " << cells[i]->GetName() << " does not align with any site";
                    cout << cells[i]->GetX() << ", " << cells[i]->GetY() << endl;
                    exit(-1);

                }
            }
        }
    }
    cout << "Total # of cells for Check(): " << numCells << endl;
    cout << "Total # of cells from IO: " << numNodes << endl;
    cout << "Total # of PlaceRow: " << numPlaceRow << endl;
}
