#include <iostream>
#include <vector>
#include <filesystem>
#include <variant>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace std;
using var = variant<int, string,bool>;
vector <unordered_map<string, var>> vect;
int id=0;
void dfs(string dir,int parent_id){
    if (dir.empty() || fs::is_regular_file(dir)) return;
    for (auto &item : fs::directory_iterator(dir)){
        unordered_map<string, var> m;
        m["id"]=id++;
        m["name"]=item.path().filename().string();
        m["is_file"]=fs::is_regular_file(item.path());
        m["parent_id"]=parent_id;
        m["path"]=item.path().string();
        vect.push_back(m);
        if (fs::is_directory(item.path())){
            continue;
        }
        dfs(item.path().string(),m["id"].index());
    }
}
int main ()
{
    unordered_map<string, var> m;
    string path="D:/CLion 2026.1.1";
    m["id"]=id;
    m["name"]=fs::path(path).filename().string();
    m["is_dir"]=true;
    m["path"]=path;
    m["parent_id"]=-1;
    vect.push_back(m);
    dfs(path,0);
    return 0;
}
