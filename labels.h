#pragma once

#include <string>
#include "ast.h"

class Labels {
    public:
        std::string class_initializer_label(std::string ns, AST::Class *c);
        std::string static_field_label(std::string ns, AST::Class *c, AST::FieldFeature *f);
};

