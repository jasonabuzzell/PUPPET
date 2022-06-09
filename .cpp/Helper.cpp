#include "../.hpp/XYZ.h"
#include "../.hpp/Character.h"
#include "../.hpp/Helper.h"
#include "../.hpp/json.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace nlohmann;

// STRINT CLASS
// -------------------------------------------------------
Strint::Strint(string a, int b) 
: String(a), Int(b)
{}

int Strint::getInt() {
    return Int;
}

void Strint::setInt(int b) {
    Int = b;
}

string Strint::getStr() {
    return String;
}

void Strint::setStr(string a) {
    String = a;
}

// HELPER FUNCTIONS
// -------------------------------------------------------

string cap(string s) {
    s[0] = toupper(s[0]);
    for (int i = 1; i < s.length(); i++) {
        if (s[i-1] == ' ') {
            s[i] = toupper(s[i]);
        }
    }
    return s;
}

string decap(string s) {
    for (int i = 0; i < s.length(); i++) {
        s[i] = tolower(s[i]);
    }
    return s;
}

int enumConvert(string s, vector<string> vec) {
    for (int i = 0; i < vec.size(); i++) {
        if (s.length() == vec[i].length()) {
            for (int j = 0; j < s.length(); j++) {
                if (toupper(s[j]) != vec[i][j] && tolower(s[j]) != vec[i][j]) break;
                else if (j == s.length() - 1) return i;
            }
        }
    }
    return -2;
}

int convertStrInt(string s) {
    try {
        for (auto c : s) {
            if (!isdigit(c))
                throw 1;
        }
        return stoi(s);
    } catch (int e) {
        return -2;
    }
}

string floatString(float f, int precision) {
    f = round(f * 100) / 100;
    string iStr, print;
    string fStr = to_string(f);
    int count = 0;
    for (auto c: fStr) {
        if (c == '.') break;
        iStr += fStr[count++];         
    }
    string floats = fStr.substr(++count, precision);
    for (int i = floats.size()-1; i >= 0; i--) {
        if (floats[i] == '0') floats.pop_back();
    }
    if (floats.empty()) print = iStr;
    else print = iStr + "." + floats;
    return print;
}

Strint printJson(json dict) {
    string print;
    int count = 0;
    print += "\n";
    for (auto i: dict.items()) {
        print += to_string(count) + ". " + i.key();
        if (i.value().is_number_float()) print += ": " + floatString(i.value(), 2);
        else if (!i.value().is_object()) print += ": " + to_string(i.value());
        print += "\n";
        count++;
    }
    print += to_string(count) + ". Cancel\nEnter: ";
    Strint full(print, count);
    return full;
}

string printJsonRepeat(json dict, string print, int itr) {
    for (auto i: dict.items()) {
        print += "\n";
        for (int i = 0; i < itr; i++) {
            print += "  ";
        }
        print += i.key() + ": ";
        if (i.value().is_number()) {
            print += to_string(i.value());
        } else {
            print = printJsonRepeat(dict[i.key()], print, ++itr);
        }
    }
    return print;
}

Strint printJsonAll(json dict) {
    Strint strint("", -1);
    string print;
    int count = 0;

    if (dict == json({})) return strint;
    for (auto i: dict.items()) {
        print += "\n" + to_string(count++) + ". " + i.key() + ":";
        if (i.value().is_number()) {
            print += i.value();
        } else {
            print = printJsonRepeat(dict[i.key()], print, 1);
        }
    }
    print += "\n" + to_string(count) + ". Cancel\nEnter: ";
    strint.setStr(print);
    strint.setInt(count);
    return strint;
}

void update(string link, json file) {
    ofstream ofs(link);
    ofs << file;
    ofs.close();
}

vector<float> vecIntToFloat(vector<int> vecInt) {
    vector<float> vecFloat;
    for (auto i: vecInt) {
        vecFloat.push_back(float(i));
    }
    return vecFloat;
}

string listPrint(json dict) {
    string print;

    if (dict == json({})) return print;
    int count = 0;
    int size = dict.size();
    for (auto i: dict.items()) {
        print += i.key();
        if (++count < size) print += ", ";
    }
    print += "\n";

    return print;
}

string jsonListPrint(json dict) {
    string print;

    if (dict == json({})) return print;
    for (auto i: dict.items()) {
        print += "    " + i.key() + ": " + listPrint(dict[i.key()]);
    }
    return print;
}

string roundStr(float f) {
    string fStr = to_string(f);
    for (int i = 0; i < fStr.length(); i++) {
        if (fStr[i] == '.') {
            string print = fStr.substr(0, i);
            if (fStr[i+1]) {
                print + fStr[i+1];
                if (fStr[i+2]) print + fStr[i+2];
            }
            return print;
        }
    }
    return fStr;
}

string upper(string s) {
    string up;
    for (auto c: s) {
        up += toupper(c);
    }
    return up;
}

json jsonLoad(string name) {
    ifstream ifs(".json/" + name + ".json");
    json dict = json::parse(ifs);
    ifs.close();
    return dict;
}

void jsonSave(json dict, string name) {
    ofstream ofs(".json/" + name + ".json");
    ofs << dict;
    ofs.close();   
    return;
}

vector<string> splitString(string s, string del) {
    vector<string> v;
    s += del;
    int start = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s.substr(i, del.length()) == del) {
            v.push_back(s.substr(start, i-start));
            i += del.length();
            start = i;
        }
    }
    return v;
}

// For space seperated values
vector<float> strVecFloat(vector<string> vStr) {
    vector<float> vFloat;
    for (auto s: vStr) {
        vFloat.push_back(stof(s));
    }
    return vFloat;
}

string lowercase(string s) {
    for (int i = 0; i < s.length(); i++) {
        s[i] = tolower(s[i]);
    }
    return s;
}

string onlyKey(json dict) {
    for (auto i: dict.items()) {
        return i.key();
    }

    return "";
}