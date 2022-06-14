//
// Created by qza2468 on 22-6-10.
//
#include <tuple>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <vector>

using namespace std;

class Path_QZA {
public:
    std::vector<std::string> paths;
    explicit Path_QZA(const std::string &s);
};

Path_QZA::Path_QZA(const string &s) {
    size_t pos_last = 0;
    vector<string> path_need_process;
    while (true) {
        auto pos = s.find('/', pos_last);
        if (pos == std::string::npos) {
            pos = s.size();
        }

        if (pos_last != pos) {
            path_need_process.emplace_back(s, pos_last, pos - pos_last);
        }

        if (pos == s.size()) {
            break;
        }

        pos_last = pos + 1;
    }

    for (size_t i = 0; i < path_need_process.size(); i++) {
        if (path_need_process[i].empty() or path_need_process[i] == ".") {
            continue;
        } else if (path_need_process[i] == "..") {
            if (not paths.empty())
                paths.pop_back();
        } else {
            paths.emplace_back(path_need_process[i]);
        }
    }
}
int main () {
    using namespace std;

    vector<Path_QZA> paths;
    paths.emplace_back("/lfsaj/fajsl//fasdlf//../jlad/");
    paths.emplace_back("/lfsaj/fajsl//fasdlf//../jlad");
    paths.emplace_back("fdjkl/flakjsd//ldsaj/./fjasl/../jfsao");
    paths.emplace_back("/");
    paths.emplace_back("..");
    paths.emplace_back("./");
    paths.emplace_back(".");
    paths.emplace_back("/aljfd/.././afsjdl");
    paths.emplace_back("//////");
    paths.emplace_back("/../..");

    for (int i = 0; i < paths.size(); ++i) {
        for (int j = 0; j < paths[i].paths.size(); ++j) {
            cout << paths[i].paths[j] << "\t";
        }
        cout << endl;
    }
}