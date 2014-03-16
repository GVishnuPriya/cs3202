/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include "simple/spa.h"
#include "impl/parse_error.h"

using std::cout;
using std::cin;
using std::endl;
using simple::parser::IncompleteParseError;

bool file_exists(const std::string& filename)
{
  std::ifstream ifile(filename);
  return ifile != NULL;
}

int main(int argc, const char* argv[]) {
    if(argc < 2) {
        cout << "Please supply filename as first argument." << endl;
    }

    std::string filename(argv[1]);

    if(!file_exists(filename)) {
        cout << "Unable to open file " << filename << endl;
        return 0;
    }

    SimpleProgramAnalyzer *spa = create_simple_program_analyzer();

    try {
        spa->parse(filename);
    } catch(std::runtime_error& e) {
        cout << "Error parsing file. " << e.what() << endl;
        return 0;
    }

    std::string line;
    cout << "simple> ";

    while(true) {
        std::string input;
        getline(cin, input);
        line += input;

        try {
            std::vector<std::string> result = spa->evaluate(line);

            auto it = result.begin();
            auto end = result.end();
            
            while(it != end) {
                cout << *it;
                
                if(++it == end) break;
                cout << ", ";
            }

            cout << endl << "simple> ";
            line = "";
        } catch(IncompleteParseError& e) {
            cout << "      > ";
        } catch(std::runtime_error& e) {
            cout << "Error evaluating query. " << e.what() << endl;
            cout << endl << "simple> ";

            line = "";
        }
    }

    return 0;
}
