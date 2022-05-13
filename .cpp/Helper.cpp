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

string Strint::getStr() {
    return String;
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
    cout << dict.dump() << "\n";
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