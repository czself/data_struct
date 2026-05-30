#include <iostream>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
namespace fs = std::filesystem;
using namespace std;
using var = std::variant<string, int, bool>;
vector<unordered_map<string, var>> dirList;
int id = 0;

void bfs(std::string path, int parent_id) {
    if (fs::is_regular_file(path)) {
        return;
    }
    
    try {
        for (auto& entry: std::filesystem::directory_iterator{path}) {
            dirList.push_back({
                {"id", id++},
                {"parent_id", parent_id},
                {"path", entry.path().string()},
                {"name", entry.path().filename().string()},
                {"is_dir", entry.is_directory()}
            });
        }
    } catch (const fs::filesystem_error& ex) {
        cerr << "无法访问目录: " << path << ", 错误: " << ex.what() << endl;
    }
}

int main() {
    std::string path = "D:/CLion 2026.1.1";
    dirList.push_back({
        {"id", id++},
        {"parent_id", -1},
        {"path", path},
        {"name", "data structure"},
        {"is_dir", true}
    });


    for (int i = 0; i < dirList.size(); i++) {
        unordered_map<string, var> dir = dirList[i];
        if (get<bool>(dir["is_dir"])) {
            bfs(get<string>(dir["path"]), get<int>(dir["id"]));
        }
    }
    
    cout << "总共找到 " << dirList.size() << " 个项目" << endl;
    
    return 0;
}
