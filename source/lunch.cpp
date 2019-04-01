#include <iostream>
#include <string>
#include <vector>
#include "curl/curl.h"

using namespace std;

static void show_usage(string name) {
    cerr << "Usage: " << name << " <option(s)>"
        << "Options:\n"
        << "\t-h,--help\t\tShow this help message\n"
        << endl;
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        show_usage(argv[0]);
        return 1;
    }
    cout << "Hello World!\n";


    return 0;
}