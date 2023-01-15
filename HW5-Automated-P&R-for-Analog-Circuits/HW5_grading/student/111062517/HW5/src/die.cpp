#include "header.h"

extern const int CS_WIDTH;
extern const int CS_HEIGHT;
extern const int M3_WIDTH;    
extern const int M3_SPACING;
extern const int M4_WIDTH;
extern const int M4_SPACING;
extern const int CS_X1_TO_DRAIN;
extern const int CS_Y1_TO_DRAIN;
extern const string CS_LIB_NAME;
extern const string VIA34_LIB_NAME;

void Die::AddList() {
    for (int i = 0; i < numRow; ++i)
        for (int j = 0; j < numRow; ++j)
            component_list.push_back(cs_array[i][j]);

    for (int i = 0; i < numRow; ++i)
        for (int j = 0; j < numM3; ++j)
            specialnet_list.push_back(ME3_specialnet[i][j]);

    for (int i = 0; i < numRow; ++i)
        for (int j = 0; j < numRow; ++j)
            specialnet_list.push_back(ME4_specialnet_drain[i][j]);

    for (int i = 0; i < numRow; ++i)
        for (int j = 0; j < numM4Port; j++)
            specialnet_list.push_back(ME4_specialnet_port[i][j]);

    for (int i = 0; i < numRow; ++i)
        for (int j = 0; j < numRow; ++j)
            component_list.push_back(Via34_drain2ME3[i][j]);

    for (int i = 0; i < numRow; ++i)
        for (int j = 0; j < numM4Port; ++j)
            for (int k = 0; k < 2; ++k)
                component_list.push_back(Via34_port2ME3[i][j][k]);
}

void Die::WriteDef(const char* filename) {
    cout << filename << endl;
    ofstream f(filename);
    if (f.fail()) {
        cout << "[Error] Failed to open file: " << filename << endl;
        exit(-1);
    }

    f << "VERSION 5.6 ;" << endl;
    f << "DIVIDERCHAR \"/\" ;" << endl;
    f << "BUSBITCHARS \"[]\" ;" << endl;

    f << "DESIGN " << designName << " ;" << endl << endl;
    f << "UNITS DISTANCE MICRONS 1000 ;" << endl << endl;
    f << "PROPERTYDEFINITIONS" << endl;
    f << "  COMPONENTPIN text STRING ;" << endl;
    f << "END PROPERTYDEFINITIONS" << endl << endl;
    f << "DIEAREA ( " << _x1 << " " << _y1 << " ) ( " << _x2 << " " << _y2 << " ) ;" << endl << endl;

    if (component_list.size() == 0) {
       cout << "[Error] component_list size = 0" << endl;
       //exit(-1);
    }

    f << "COMPONENTS " << component_list.size() << " ;"  << endl;
    for (auto component : component_list) {
        if (component) {
            f << "- " << component->GetInstName() << " " << component->GetLibName() << endl;
            f << "  + PLACED ( " << component->GetX() << " " << component->GetY() << " ) N ;" << endl;
        }
    }
    f << "END COMPONENTS" << endl << endl;

    if (specialnet_list.size() == 0) {
        cout << "[Error] specialnet_list size = 0" << endl;
        //exit(-1);
    }

    f << "SPECIALNETS " << specialnet_list.size() << " ;" << endl;
    for (auto specialnet : specialnet_list) {
        if (specialnet)
            if (specialnet->GetLayer() == "ME3") {
                string name = specialnet->GetInstName();
                int x = (specialnet->GetX1() + specialnet->GetX2()) / 2;
                int y1 = specialnet->GetY1();
                int y2 = specialnet->GetY2();
                f << "- " << name << endl;
                f << "  + ROUTED ME3 440 ( " << x << " " << y1 << " ) ( * " << y2 << " ) ;" << endl;
            }
            else if (specialnet->GetLayer() == "ME4") {
                string name = specialnet->GetInstName();
                int x1 = specialnet->GetX1();
                int x2 = specialnet->GetX2();
                int y = (specialnet->GetY1() + specialnet->GetY2()) / 2;
                f << "- " << name << endl;
                f << "  + ROUTED ME4 1000 ( " << x1 << " " << y << " ) ( " << x2 << " * ) ;" << endl;
            }
            else {
                cout << "[Error] worong layer" << endl;
                exit(-1);
            }
    }

    f << "END SPECIALNETS" << endl << endl;
    f << "END DESIGN" << endl;
}

void Die::CreateCSarray() {
    cout << "[CreateCSarray()]" << endl;
    cout << numRow << " * " << numRow << endl;

    cs_array = new Component** [numRow];
    for (int i = 0; i < numRow; ++i) {
        cs_array[i] = new Component* [numRow];
        for (int j = 0; j < numRow; ++j) {
            string cs_lib_name = CS_LIB_NAME;
            string cs_instance_name = "Transistor" + to_string(i * numRow + j);
            int off_y = (M4_SPACING + M4_WIDTH) * numM4Port;
            int D_x = CS_WIDTH + M3_SPACING * (numM3 + 1) + M3_WIDTH * numM3;
            int D_y = CS_HEIGHT + M4_SPACING * (numM4Port + 1) + M4_WIDTH * numM4Port;
            int x = i * D_x;
            int y = j * D_y + off_y;
            cs_array[i][j] = new Component(cs_lib_name, cs_instance_name, x, y);
        }
    }
}

void Die::CreateVerticalME3() {
    cout << "[CreateVerticalME3()]" << endl;
    ME3_specialnet = new SpecialNet** [numRow];
    for (int i = 0; i < numRow; ++i) {
        ME3_specialnet[i] = new SpecialNet* [numM3];
        for (int j = 0; j < numM3; ++j) {
            string inst_name = "Metal3_" + to_string(i * numM3 + j);
            string layer = "ME3";
            int D_x = CS_WIDTH + M3_SPACING;
            int P_x = M3_WIDTH + M3_SPACING;
            int x1 = cs_array[i][0]->GetX() + D_x + j * P_x;
            int x2 = x1 + M3_WIDTH;
            ME3_specialnet[i][j] = new SpecialNet(inst_name, layer, x1, _y1, x2, _y2);
        }
    }
    for (int i = 0; i < numRow; ++i)
        for (int j = 0; j < numM3; ++j)
            cout << ME3_specialnet[i][j]->GetInstName() << " " << ME3_specialnet[i][j]->GetLayer() << endl;
}

void Die::CreateME4Drain() {
    cout << "[CreateME4Drain()]" << endl;
    string inst_name;
    string layer;
    int x1;
    int x2;
    int y1;
    int y2;

    ME4_specialnet_drain = new SpecialNet** [numRow];
    for (int i = 0; i < numRow; ++i)
        ME4_specialnet_drain[i] = new SpecialNet* [numRow];

    for (int i = 0; i < numM3; ++i) {
        for (int j = 0; j < numM3; ++j) {
            layer = "ME4";
            // left bottom corner units
            inst_name = "Metal4_drain_" + to_string(i * numM3 + j + 0 * numCurrentSources);
            x1 = cs_array[i][j]->GetX() + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[i][j]->GetX2();
            y1 = cs_array[i][j]->GetY() + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            ME4_specialnet_drain[i][j] = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            // right bottom corner units
            inst_name = "Metal4_drain_" + to_string(i * numM3 + j + 1 * numCurrentSources);
            x1 = cs_array[numRow - 1 - i][j]->GetX() + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[numRow - 1 - i][j]->GetX2();
            y1 = cs_array[numRow - 1 - i][j]->GetY() + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            ME4_specialnet_drain[numRow - 1 - i][j] = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            // left top corner units
            inst_name = "Metal4_drain_" + to_string(i * numM3 + j + 2 * numCurrentSources);
            x1 = cs_array[i][numRow - 1 - j]->GetX() + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[i][j]->GetX2();
            y1 = cs_array[i][numRow - 1 - j]->GetY() + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            ME4_specialnet_drain[i][numRow - 1 - j] = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            // right top corner units
            inst_name = "Metal4_drain_" + to_string(i * numM3 + j + 3 * numCurrentSources);
            x1 = cs_array[numRow - 1 - i][numRow - 1 - j]->GetX() + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[numRow - 1 - i][j]->GetX2();
            y1 = cs_array[numRow - 1 - i][numRow - 1 - j]->GetY() + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            ME4_specialnet_drain[numRow - 1 - i][numRow - 1 - j] = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
        }
    }
}

void Die::CreateME4Port() {
    cout << "[CreateME4Port()]" << endl;
    ME4_specialnet_port = new SpecialNet** [numRow];
    string inst_name;
    string layer;
    int y1;
    int y2;

    for (int i = 0; i < numRow; ++i) {
        ME4_specialnet_port[i] = new SpecialNet* [numM4Port];
        for (int j = 0; j < numM4Port; ++j) {
            inst_name = "Metal4_port_" + to_string(i * numM4Port + j);
            layer = "ME4";
            int D_y = CS_HEIGHT + M4_SPACING * (numM4Port + 1) + M4_WIDTH * numM4Port;
            y1 = i * D_y + j * (M4_WIDTH + M4_SPACING);
            y2 = y1 + M4_WIDTH;
            ME4_specialnet_port[i][j] = new SpecialNet(inst_name, layer, _x1, y1, _x2, y2);
        }
    }
}

void Die::CreateVia43FromME4Drain() {
    cout << "[CreateVia43FromME4Drain()]" << endl;
    int x;
    int y;
    string lib_name;
    string inst_name;
    Via34_drain2ME3 = new Component** [numRow];
    for (int i = 0; i < numRow; ++i)
        Via34_drain2ME3[i] = new Component* [numRow];

    for (int i = 0; i < numM3; ++i) {
        for (int j = 0; j < numM3; ++j) {
            lib_name = VIA34_LIB_NAME;
            // left bottom corner units
            inst_name = "Via34_drain2ME3_" + to_string(i * numM3 + j + 0 * numCurrentSources);
            x = ME3_specialnet[i][j]->GetX1();
            y = cs_array[i][j]->GetY() + CS_Y1_TO_DRAIN;
            Via34_drain2ME3[i][j] = new Component(lib_name, inst_name, x, y);
            // right bottom corner units
            inst_name = "Via34_drain2ME3_" + to_string(i * numM3 + j + 1 * numCurrentSources);
            x = ME3_specialnet[numRow - 1 - i][j]->GetX1();
            y = cs_array[numRow - 1 - i][j]->GetY() + CS_Y1_TO_DRAIN;
            Via34_drain2ME3[numRow - 1 - i][j] = new Component(lib_name, inst_name, x, y);
            // left top corner units
            inst_name = "Via34_drain2ME3_" + to_string(i * numM3 + j + 2 * numCurrentSources);
            x = ME3_specialnet[i][j]->GetX1();
            y = cs_array[i][numRow - 1 - j]->GetY() + CS_Y1_TO_DRAIN;
            Via34_drain2ME3[i][numRow - 1 - j] = new Component(lib_name, inst_name, x, y);
            // right top corner units
            inst_name = "Via34_drain2ME3_" + to_string(i * numM3 + j + 3 * numCurrentSources); 
            x = ME3_specialnet[numRow - 1 - i][j]->GetX1();
            y = cs_array[numRow - 1 - i][numRow - 1 - j]->GetY() + CS_Y1_TO_DRAIN;
            Via34_drain2ME3[numRow - 1 - i][numRow - 1 - j] = new Component(lib_name, inst_name, x, y);
        }
    }
}

void Die::CreateVia34ToME4Port() {
    cout << "[CreateVia34ToME4Port()]" << endl;

    string lib_name = VIA34_LIB_NAME;
    string inst_name;
    int x = 0;
    int y = 0;
    int cnt = 0;
    Via34_port2ME3 = new Component*** [numRow];
    for (int i = 0; i < numRow; ++i) {
        Via34_port2ME3[i] = new Component** [numM4Port];
        for (int j = 0; j < numM4Port; ++j) {
            Via34_port2ME3[i][j] = new Component* [2];
        }
    }

    for (int i = 0; i < numRow; ++i) {
        for (int j = 0; j < numM4Port; ++j) {
            // left
            inst_name = "Via34_port2ME3_" + to_string(cnt++);
            x = ME3_specialnet[i][j]->GetX1();
            y = ME4_specialnet_port[i][j]->GetY1();
            Via34_port2ME3[i][j][0] = new Component(lib_name, inst_name, x, y);
            // right
            inst_name = "Via34_port2ME3_" + to_string(cnt++);
            x = ME3_specialnet[numRow - 1 - i][j]->GetX1();
            y = ME4_specialnet_port[i][j]->GetY1();
            Via34_port2ME3[i][j][1] = new Component(lib_name, inst_name, x, y);
        }
    }
}
