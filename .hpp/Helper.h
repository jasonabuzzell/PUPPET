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
std::string floatString(float f);
Strint printJson(nlohmann::json dict);
std::string printJsonRepeat(nlohmann::json dict, std::string *print, int itr);
Strint printJsonAll(nlohmann::json dict);
void update(std::string link, nlohmann::json file);
std::vector<float> vecIntToFloat(std::vector<int> vecInt); 
std::string jsonListPrint(nlohmann::json dict);
std::string listPrint(nlohmann::json dict);
std::string roundStr(float f);
std::string upper(std::string s);
nlohmann::json jsonLoad(std::string name);
void jsonSave(nlohmann::json dict, std::string name);
std::vector<std::string> splitString(std::string s, std::string del);
std::vector<float> strVecFloat(std::vector<std::string> s);

#endif