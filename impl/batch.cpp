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

#include <set>
#include <vector>
#include <string>
#include <sstream>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <istream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include "simple/spa.h"
#include "impl/parse_error.h"

using namespace std;
using simple::parser::IncompleteParseError;

bool file_exists(const string& filename)
{
  ifstream ifile(filename);
  return ifile != NULL;
}

void normalize_string(string& str)
{
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

    string::size_type pos = str.find_last_not_of(' ');

    if(pos != string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if(pos != string::npos) str.erase(0, pos);
    } else {
        str.erase(str.begin(), str.end());
    }
}

set<string> split_string(string str) {
    set<string> result;
    istringstream ss(str);

    while(!ss.eof()) {
        string in;
        getline(ss, in, ',');

        normalize_string(in);
        result.insert(in);
    }

    return result;
}

string get_line(istream& in) {
    string line;

    getline(in, line);

    normalize_string(line);
    return line;
}

string get_two_lines(istream& in) {
    string line1 = get_line(in);
    string line2 = get_line(in);

    return line1 + " " + line2;
}

void print_set(const set<string>& source) {
    auto end = source.end();
    for(auto it=source.begin(); it != end;) {
        cout << *it;
        if(++it != end) {
            cout << ", ";
        }
    }
    cout << endl;
}

void batch_process(SimpleProgramAnalyzer *spa, istream& in) {
    string count = get_line(in);
    int total_queries = stoi(count);
    int passed = 0;

    string batch_name = get_line(in);

    cout << "Running " << total_queries << " queries for batch " 
         << batch_name << endl << endl;

    for(int i=0; i < total_queries; ++i) {
        string query_name = get_line(in);
        string query = get_two_lines(in);
        string expected = get_line(in);
        string timeout = get_line(in);

        vector<string> result;

        try {
            result = spa->evaluate(query);
        } catch(runtime_error& e) {
            cout << "Error evaluating query #" << i << endl;
            cout << "Query: " << query << endl;
            cout << "Error: " << e.what() << endl;
            cout << endl;

            continue;
        }

        if(result.size() == 0 && expected == "none") {
            continue;
        }

        set<string> result_set;
        copy(result.begin(), result.end(), inserter(result_set, result_set.begin()));
        
        set<string> expected_set = split_string(expected);

        if(result_set != expected_set) {
            cout << "Result differ in query #" << i << endl;
            cout << "Query: " << query << endl;

            cout << "Result: ";
            print_set(result_set);

            cout << "Expected: ";
            print_set(expected_set);

            cout << endl;
        } else {
            ++passed;
        }
    }

    cout << "Finished evaluating all queries." << endl;
    cout << passed << "/" << total_queries << " passed" << endl;
}

int main(int argc, const char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cerr.tie(nullptr);

    if(argc < 3) {
        cout << "Usage: batch [source_file] [pql_file]." << endl;
        return 0;
    }

    string source_file(argv[1]);
    string pql_file(argv[2]);

    if(!file_exists(source_file)) {
        cout << "Unable to open simple source file " << source_file << endl;
        return 0;
    }

    SimpleProgramAnalyzer *spa = create_simple_program_analyzer();

    try {
        spa->parse(source_file);
    } catch(runtime_error& e) {
        cout << "Error parsing file. " << e.what() << endl;
        return 0;
    }

    ifstream pql_source(pql_file);
    if(pql_source == NULL) {
        cout << "Unable to open pql file " << pql_file << endl;
        return 0;
    }

    try {
        batch_process(spa, pql_source);
    } catch(runtime_error& e) {
        cout << "Error evaluating PQL. " << e.what() << endl;
        return 0;
    }

    return 0;
}
