#include <filesystem>
#include <iostream>

int main () {
    using namespace std;
    bool a;
    try {
        a = std::filesystem::create_directory("/home/qza2468");
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    cout << a << endl;

    try {
        a = std::filesystem::create_directory("/home/qza2468/SourceCode/bbb/bbb");
    } catch (exception &e) {
        std::cout << e.what() << std::endl;
    }

    cout << a << endl;


    try {
        a = std::filesystem::create_directory("/home/qza2468/SourceCode/app_server/test");
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    cout << a << endl;
}

