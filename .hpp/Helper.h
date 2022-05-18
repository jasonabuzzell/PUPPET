#ifndef HELPER_H
#define HELPER_H

#pragma once
#include "json.hpp"
#include <string>
#include <vector>

class Strint
{
    public:
        Strint(std::string a, int b);
        int getInt();
        void setInt(int b);
        std::string getStr();
        void setStr(std::string a);
    private:
        int Int;
        std::string String;
};

std::string cap(std::string s);
std::string decap(std::string s);
int enumConvert(std::string s, std::vector<std::string> vec);
int convertStrInt(std::string s);
Strint printJson(nlohmann::json dict);
std::string printJsonRepeat(nlohmann::json dict, std::string *print, int itr);
Strint printJsonAll(nlohmann::json dict);
void update(std::string link, nlohmann::json file);
std::vector<float> vecIntToFloat(std::vector<int> vecInt); 

#endif