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
    auto itr = find(vec.begin(), vec.end(), s);
    if (itr == vec.end()) {
        return -2;
    } else {
        return distance(vec.begin(), itr);
    }
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

Strint printJson(json dict) {
    string print;
    int count = 0;
    print += "\n";
    for (auto i: dict.items()) {
        print += to_string(count) + ". " + i.key();
        if (i.value().is_number() || i.value().is_string()) 
            print += ": " + to_string(i.value());
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
    string print;
    int count = 0;
    for (auto i: dict.items()) {
        print += "\n" + to_string(count++) + ". " + i.key() + ":";
        if (i.value().is_number()) {
            print += i.value();
        } else {
            print = printJsonRepeat(dict[i.key()], print, 1);
        }
    }
    print += "\n" + to_string(count) + ". Cancel\nEnter: ";
    Strint strint(print, count);
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

    for (int i = 0; i < dict.size(); i++) {
        print += dict[i];
        if (i != dict.size() - 1) print += ", ";
    }
    print += "\n";

    return print;
}

string jsonListPrint(json dict) {
    string print = "";
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