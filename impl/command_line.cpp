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
#include "impl/frontend.h"
#include "impl/parser/parser.h"

using namespace simple;
using namespace simple::impl;

using std::cout;
using std::cin;
using std::endl;

int main(int argc, const char* argv[]) {
    if(argc < 2) {
        cout << "Please supply filename as first argument." << endl;
    }

    std::string filename(argv[1]);
    std::ifstream source(filename);

    if(source.is_open()) {
        try {
            std::istreambuf_iterator<char> source_begin(source);
            std::istreambuf_iterator<char> source_end;

            SimplePqlFrontEnd frontend(source_begin, source_end);

            std::string line;
            cout << "simple> ";
            while(getline(cin, line)) {
                frontend.process_query(line.begin(), line.end(),
                        std::ostreambuf_iterator<char>(cout));

                cout << endl << "simple> ";
            }

        } catch(std::exception& e) {
            cout << "Error parsing simple source file: " << e.what() << endl;
            return 0;
        }
    } else {
        cout << "file " << filename << " not found." << endl;
    }

    return 0;
}
