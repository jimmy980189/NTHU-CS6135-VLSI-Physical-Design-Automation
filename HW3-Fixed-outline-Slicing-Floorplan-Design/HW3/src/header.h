#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cmath>
#include <climits>
#include <chrono>
#include "color.h"

#define LIMIT 550

using namespace std;
using namespace chrono;

enum type { hardblock, terminal };
enum nodeType { HARDBLOCK, VERTICALCUT, HORIZONTALCUT, X};

struct cmpPos {
    bool operator() (const pair<int, int>& a, const pair<int, int>& b) const {
        return a.first > b.first;
    }
};

class HardBlock {
    private:
        string name;
        int idx;
        vector<pair<int, int>> positions; // four corner
        pair<int, int> widthAndHeight;
        pair<int, int> lowerLeftCorner = { 0, 0 };
        //pair<int, int> lowerLeftCorner = { 200, 200 };
        int area;
        bool rotated = false;
        type t = hardblock;
        vector<string> contains;

    public:
        HardBlock(string name) : name(name) {
            const char* ptr = name.c_str();
            idx = atoi(ptr + 2);
        }
        HardBlock(string name, vector<pair<int, int>> positions) {
            this->name = name;
            this->positions = positions;
        }
        ~HardBlock() {}
        type GetType() { return t; }
        int GetIdx() { return idx; }

        string GetName() { return name; }
        vector<pair<int, int>> GetPositions() { return positions; }
        pair<int, int> GetPosition(int i) { return positions[i]; }
        void AddPosition(pair<int, int> p) { positions.push_back(p); }

        void RevRotated() { rotated = !rotated; }
        void SetRotated(bool r) { rotated = r; }
        bool GetRotated() { return rotated; }
        int GetArea() { return widthAndHeight.first * widthAndHeight.second; }

        void SetWidthAndHeight(pair<int, int> w) { widthAndHeight = w; }
        int GetWidth() { return !rotated ? widthAndHeight.first : widthAndHeight.second; }
        int GetHeight() { return !rotated ? widthAndHeight.second : widthAndHeight.first; }
        pair<int, int> GetWidthAndHeight() {
            if (!rotated) 
                return { widthAndHeight.first, widthAndHeight.second };
            else
                return { widthAndHeight.second, widthAndHeight.first };
        }
        pair<int, int> GetWidthAndHeight(bool r) {
            if (!r) 
                return { widthAndHeight.first, widthAndHeight.second };
            else
                return { widthAndHeight.second, widthAndHeight.first };
        }

        pair<int, int> GetCenter() {
            int x, y;
            if (!rotated) {
                x = lowerLeftCorner.first + widthAndHeight.first / 2;
                y = lowerLeftCorner.second + widthAndHeight.second / 2;
            }
            else {
                x = lowerLeftCorner.first + widthAndHeight.second / 2;
                y = lowerLeftCorner.second + widthAndHeight.first / 2;
            }
            return { x, y };
        }
        pair<int, int> GetLowerLeftCorner() { return lowerLeftCorner; }
        void SetLowerLeftCorner(pair<int, int> l) { lowerLeftCorner = l; }
        void AddLowerLeftCornerWidth(int w) { lowerLeftCorner.first += w; }
        void AddLowerLeftCornerHeight(int h) { lowerLeftCorner.second += h; }

        void AddContains(string s) { contains.push_back(s); }
        vector<string> GetAllContains() { return contains; }
};

class Terminal {
    private:
        string name;
        pair<int, int> position;
        type t = terminal;

    public:
        Terminal(string name) : name(name) {}
        ~Terminal() {}
        type GetType() { return t; }

        pair<int, int> GetPosition() { return position; }
        void SetPosition(pair<int, int> p) { position = p; }
};

class Net {
    private:
        int idx;
        int degree;
        vector<string> pins;

    public:
        Net(int idx, int degree) : idx(idx), degree(degree) {}
        ~Net() {}

        int GetIdx() { return idx; }
        vector<string> GetPins() { return pins; }
        string GetPin(int i) { return pins[i]; }
        void AddPin(string p) { pins.push_back(p); }
        int HPWL();
};

class BstarTree {
};

class SlicingTreeNode {
    private:
        HardBlock* node = NULL;
        string name;
        nodeType type;
        SlicingTreeNode* parent = NULL;
        SlicingTreeNode* lnode = NULL;
        SlicingTreeNode* rnode = NULL;

        pair<int, int> option = { 0, 0 };
        pair<int, int> position = { 0, 0 };
        pair<int, int> base = { -1, -1 };
        int min = 0;
        int width = 0;
        int height = 0;

        vector<vector<int>> shapes;
        // {width, height, lnode, rnode}

    public:
        SlicingTreeNode(HardBlock* h, nodeType type) : node(h), type(type) {
            name = h->GetName();
        }
        SlicingTreeNode(string name, nodeType type) : name(name), type(type) {

        }
        ~SlicingTreeNode() {}
        SlicingTreeNode* insert(SlicingTreeNode* node);

        string GetName() { return name; }
        nodeType GetType() { return type; }

        void SetWidth(int w) { width = w; }
        void SetHeight(int h) { height = h; }
        int GetWidth() { return width; }
        int GetHeight() { return height; }

        HardBlock* GetNode() { return node; }
        SlicingTreeNode* GetLnode() { return lnode; }
        SlicingTreeNode* GetRnode() { return rnode; }
        SlicingTreeNode* GetParent() { return parent; }
        void SetLnode(SlicingTreeNode* node) { lnode = node; }
        void SetRnode(SlicingTreeNode* node) { rnode = node; }
        void SetParent(SlicingTreeNode* node) { parent = node; }

        pair<int, int> GetBase() { return base; }

        void AddShape(vector<int> shape) { shapes.push_back(shape); }
        vector<vector<int>>& GetShapes() { return shapes; }
        vector<int> GetShape(int i) { return shapes[i]; }

        void SetMin(int m) { min = m; }
        int GetMin() { return min; }
        void SetOption(pair<int, int> o) { option = o; }
        pair<int, int> GetOption() { return option; }

        void SetBase(pair<int, int> b) { base = b; }
        void SetPosition(pair<int, int> p) { 
            position = p; 
            /*
             *if (type == HARDBLOCK)
             *    node->SetLowerLeftCorner(p);
             */
        }
        pair<int, int> GetPosition() { return position; }

        void BackTrack() {
            if (type != HARDBLOCK) {
                lnode->SetMin(shapes[min][2]);
                rnode->SetMin(shapes[min][3]);
                /*
                 *cout << "[" << name << "]" << endl;
                 *cout << lnode->GetName() << " ";
                 *cout << "l: " << lnode->GetShape(shapes[min][2])[0] << " ";
                 *cout << lnode->GetShape(shapes[min][2])[1] << endl;
                 *cout << rnode->GetName() << " ";
                 *cout << "r: " << rnode->GetShape(shapes[min][3])[0] << " ";
                 *cout << rnode->GetShape(shapes[min][3])[1] << endl;
                 *cout << endl;
                 */
            }
            else {
                /*
                 *cout << name << " min: " << min << endl;
                 *cout << node->GetRotated() << " " << node->GetWidth() << " " << node->GetHeight() << endl;
                 *for (auto i : shapes)
                 *    cout << i[0] << " " << i[1] << endl;
                 */
                if (min)
                    node->RevRotated();
                /*
                 *if (shapes[min][2]) 
                 *    lnode->GetNode()->RevRotated();
                 *if (shapes[min][3])
                 *    rnode->GetNode()->RevRotated();
                 */
            }

            if (lnode)
                lnode->BackTrack();
            if (rnode)
                rnode->BackTrack();
        }


        void PostOrder() {
            if (lnode)
                lnode->PostOrder();

            if (rnode)
                rnode->PostOrder();

            /*
             *if (type == HORIZONTALCUT)
             *    cout << KGRN << "node: " << name << RST << endl;
             *else if (type == VERTICALCUT)
             *    cout << KBLU << "node: " << name << RST << endl;
             *else
             *    cout << "node: " << name << endl;
             *if(lnode) cout << lnode->GetName() << endl;
             *if(rnode) cout << rnode->GetName() << endl;
             *for (auto i : shapes)
             *    cout << i[0] << " " << i[1] << " " << i[0] * i[1] << " " << i[2] << " " << i[3] << endl;
             *cout << endl;
             */

            if (type != HARDBLOCK) {

                /*
                 *pair<int, int> posLnode = lnode->GetPosition();
                 *vector<int> wLnode = lnode->GetShape(shapes[0][2]);
                 *vector<int> wRnode = rnode->GetShape(shapes[0][3]);
                 */

                if (shapes[0][2]) 
                    lnode->GetNode()->RevRotated();
                if (shapes[0][3])
                    rnode->GetNode()->RevRotated();

                /*
                 *if (type == VERTICALCUT) {
                 *    rnode->SetPosition({ posLnode.first + wLnode[0], posLnode.second });
                 *}
                 *else {
                 *    rnode->SetPosition({ posLnode.first, posLnode.second + wLnode[1] });
                 *}
                 */
            }
            /*
             *else
             *    cout << name << endl;
             */
        }

        pair<int, int> InOrderSetPosition() {
            /*
             *if (parent && base.first == -1)
             *    base = parent->GetBase();
             */

            if (lnode) {
                lnode->SetBase(base);
                lnode->InOrderSetPosition();
            }

            //cout << name << endl;
            if (type == HARDBLOCK) {
                node->SetLowerLeftCorner(base); 
                //cout << KGRN << "set ll " << name << " " << base.first << " " << base.second << " | " << node->GetWidth() << " " << node->GetHeight() << RST << endl;
                /*
                 *width = node->GetWidth();
                 *height = node->GetHeight();
                 */
            }
            if (type != HARDBLOCK) {
                pair<int, int> tmp = base;
                /*
                 *if (shapes[0][2]) 
                 *    lnode->GetNode()->RevRotated();
                 *if (shapes[0][3])
                 *    rnode->GetNode()->RevRotated();
                 */

                if (type == VERTICALCUT) {
                    tmp.first += lnode->GetWidth();

                    //width = lnode->GetWidth() + rnode->GetWidth();
                    //height = max(lnode->GetHeight(), rnode->GetHeight());
                }
                else {
                    tmp.second += lnode->GetHeight();

                    //width = max(lnode->GetWidth(), rnode->GetWidth());
                    //height = lnode->GetHeight() + rnode->GetHeight();
                }

                width = lnode->GetWidth();
                height = lnode->GetHeight();
                lnode->SetBase(base);
                rnode->SetBase(tmp);


                if (false) {
                    cout << name << " " << base.first << " " << base.second << " | " << width << " " << height << endl;
                    if (parent)
                        cout << "parent: " << parent->GetName() << " " << parent->GetBase().first << " " << parent->GetBase().second << endl;
                    cout << KBLU << "set lnode base: " << lnode->GetName() << " " << base.first << " " << base.second << RST << endl;
                    cout << KBLU << "set rnode base: " << rnode->GetName() << " " << tmp.first << " " << tmp.second << RST << endl;
                }
                
            }

            if (rnode) {
                rnode->InOrderSetPosition();
            }

            if (type != HARDBLOCK) {
                if (type == VERTICALCUT) {
                    width = lnode->GetWidth() + rnode->GetWidth();
                    height = max(lnode->GetHeight(), rnode->GetHeight());
                }
                else {
                    width = max(lnode->GetWidth(), rnode->GetWidth());
                    height = lnode->GetHeight() + rnode->GetHeight();
                }
            }
            return { base.first + width, base.second + height };
        }
};
