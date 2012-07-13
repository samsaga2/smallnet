#include "labels.h"
#include <sstream>
#include <boost/algorithm/string/replace.hpp>

using namespace std;

const string Labels::global_start = "__start";

string Labels::static_class_initializer_label(string ns, AST::Class *c) {
    stringstream ss;
    ss << "_" << boost::replace_all_copy(ns, ".", "_") << "__" << c->id << "___sinitializer";
    return ss.str();
}

string Labels::class_initializer_label(string ns, AST::Class *c) {
    stringstream ss;
    ss << "_" << boost::replace_all_copy(ns, ".", "_") << "__" << c->id << "___initializer";
    return ss.str();
}

string Labels::static_field_label(string ns, AST::Class *c, AST::FieldFeature *f) {
    stringstream ss;
    ss << "_" << boost::replace_all_copy(ns, ".", "_") << "__" << c->id << "__" << f->id;
    return ss.str();
}

