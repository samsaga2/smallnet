#pragma once

#include <string>
#include "ast.h"

class Labels {
    public:
        static const std::string global_start;

        std::string static_class_initializer_label(std::string ns, AST::Class *c);
        std::string class_initializer_label(std::string ns, AST::Class *c);
        std::string static_field_label(std::string ns, AST::Class *c, AST::FieldFeature *f);
};

