```cpp

#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;
using namespace std::filesystem;
// namespace fs = std::filesystem;
using VAR = variant<int,string,bool>;

/**
 * dfs
 */

vector< unordered_map<string, VAR> > vect;
int id = 0;
void dirTree(string dir,int parent_id) {
    if (dir.empty() || std::filesystem::is_regular_file(dir)) {
        return;
    }

    for (auto item : std::filesystem::directory_iterator(dir)) {
        unordered_map<string, VAR> dirMap;
        dirMap["id"] = ++id;
        dirMap["parent_id"] = parent_id;
        dirMap["name"] = item.path().filename().string();
        dirMap["is_dir"] = std::filesystem::is_directory(dir);
        dirMap["path"] = item.path().string();
        vect.push_back(dirMap); 
        if (std::filesystem::is_regular_file(item.path())) {
            continue;
        }
        dirTree(item.path().string(),id);
    }
}
int main() {
    unordered_map<string, VAR> dirMap;
    string dir = "D:\\mingw64";
    dirMap["id"] = id;
    dirMap["parent_id"] = -1;
    dirMap["name"] = "mingw64";
    dirMap["is_dir"] = std::filesystem::is_directory(dir);
    dirMap["path"] = dir;

    vect.push_back(dirMap);
    dirTree(dir,id);
}


```

在本次课程中，需要理解值传递和地址（指针）传递

```cpp

/**
 * bfs
 */
int main() {
    vector< unordered_map<string, VAR> > vect;
    int id = 0;
    unordered_map<string, VAR> dirMap;
    string dir = "D:\\mingw64";
    dirMap["id"] = id;
    dirMap["parent_id"] = -1;
    dirMap["name"] = "mingw64";
    dirMap["is_dir"] = std::filesystem::is_directory(dir);
    dirMap["path"] = dir;

    vect.push_back(dirMap);

    for (int i=0; i<vect.size(); i++) {
        unordered_map<string, VAR> m = vect[i];
        //m["id"] // 由于是一个不确定的类型，它有可能是int、string、bool
        int _id = get<int>( m["id"]  ); 
        int _parent_id = get<int>( m["parent_id"] );
        string _path = get<string>( m["path"] );
        string _name = get<string>( m["name"] );
        bool _is_dir = get<bool>( m["is_dir"] );

        for (auto item : std::filesystem::directory_iterator(_path)) {
            unordered_map<string, VAR> dirMap1;
            dirMap1["id"] = ++id;
            dirMap1["parent_id"] = _parent_id;
            dirMap1["name"] = item.path().filename().string();
            dirMap1["is_dir"] = std::filesystem::is_directory(dir);
            dirMap1["path"] = item.path().string();
            vect.push_back(dirMap);
        }
    }

}
```