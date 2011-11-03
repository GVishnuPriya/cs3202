
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
