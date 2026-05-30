//
// Created by 34217 on 2026/5/14.
//
#include<iostream>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <string>
using namespace std;
namespace fs = std::filesystem;
int main()
{
    string path="D:/CLion 2026.1.1";
    unordered_map<string ,variant<bool,string ,int,double>>map;
    map["id"]=0;
    map["name"]="D:/CLion 2026.1.1";
    map["is_dir"]=true;
    map["path"]=path;
    static int id=0;
    for (auto &it :fs::directory_iterator(path))
    {
        map["id"]=++id;
        map["name"]=it.path().filename().string();
        map["path"]=it.path().string();
        map["is_dir"]=fs::is_directory(it.path());
    }
}
