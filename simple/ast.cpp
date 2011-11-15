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

#include "simple/ast.h"

namespace simple {

SimpleRoot::SimpleRoot(ProcAst *proc) : 
    _procs(new ProcListType()) 
{
    insert_proc(proc);
}

SimpleRoot::SimpleRoot(ProcListType *procs) :
    _procs(procs)
{ }

SimpleRoot::SimpleRoot(const SimpleRoot& other) :
    _procs(other._procs)
{ }

SimpleRoot::SimpleRoot(SimpleRoot&& other) :
    _procs(std::move(other._procs))
{ }

ProcAst* SimpleRoot::get_proc(const std::string& name) {
    if(_procs->count(name) == 0) {
        return NULL;
    } else {
        return (*_procs.get())[name].get();
    }
}

SimpleRoot::iterator SimpleRoot::begin() {
    return SimpleRoot::iterator(_procs->begin());
}

SimpleRoot::iterator SimpleRoot::end() {
    return SimpleRoot::iterator(_procs->end());
}

SimpleRoot& SimpleRoot::operator =(const SimpleRoot& other) {
    _procs = other._procs;
    return *this;
}

void SimpleRoot::insert_proc(ProcAst *proc) {
    (*_procs.get())[proc->get_name()].reset(proc);
}

SimpleRoot::~SimpleRoot() { }

SimpleRoot::iterator::iterator(_iterator it) : _it(it) { }

SimpleRoot::iterator& SimpleRoot::iterator::operator ++() {
    ++_it;
    return *this;
}

ProcAst* SimpleRoot::iterator::operator *() {
    return _it->second.get();
}

bool SimpleRoot::iterator::operator ==(const iterator& other) const {
    return _it == other._it;
}

bool SimpleRoot::iterator::operator !=(const iterator& other) const {
    return _it != other._it;
}

} // namespace simple
