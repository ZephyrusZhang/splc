#include <iostream>
#include <string>

#include "lib/CLI11.hpp"

using std::cout;
using std::endl;
using std::string;

int main(int argc, char **argv)
{
    CLI::App app{"Parser"};

    bool stdout{false};
    app.add_flag("--stdout", stdout, "Whether to redirect output to stdout");

    string output_path{""};
    app.add_option("-o, --output", output_path, "Path of output file");

    CLI11_PARSE(app, argc, argv);

    cout << stdout << endl;
    cout << output_path << endl;

    
    cout << std::stol(string("0xdeadbeef"), 0, 16) << endl;
}