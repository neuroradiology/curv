// Copyright Doug Moen 2016.
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENCE.md or http://www.boost.org/LICENSE_1_0.txt

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <string>
#include <curv/eval.h>
#include <curv/exception.h>
#include <boost/math/constants/constants.hpp>

using namespace curv;
using namespace std;
using namespace boost::math::double_constants;

const Namespace
curv::builtin_namespace = {
    {"pi", pi},
    {"tau", two_pi},
    {"inf", INFINITY},
    {"null", curv::Value()},
    {"false", curv::Value(false)},
    {"true", curv::Value(true)},
};

Value
curv::eval(Phrase& expr, const Namespace& names)
{
    // Hmm. Obviously, this could be done using a virtual eval function.
    // But, this code is temporary scaffolding, so I'll wait.

    auto num = dynamic_cast<Numeral*>(&expr);
    if (num != nullptr) {
        string str(num->location().range());
        char* endptr;
        double n = strtod(str.c_str(), &endptr);
        assert(endptr == str.c_str() + str.size());
        return n;
    }

    auto ident = dynamic_cast<Identifier*>(&expr);
    if (ident != nullptr) {
        std::string id(ident->location().range());
        auto p = names.find(id);
        if (p != names.end())
            return p->second;
        else
            throw Phrase_Error(*ident, "not defined");
    }

    auto unary = dynamic_cast<Unary_Phrase*>(&expr);
    if (unary != nullptr) {
        switch (unary->op_.kind) {
        case Token::k_minus:
            {
                Value a = eval(*unary->arg_, names);
                Value r = Value(-a.get_num_or_nan());
                if (!r.is_num())
                    throw Phrase_Error(*unary, "domain error");
                return r;
            }
        case Token::k_plus:
            {
                Value a = eval(*unary->arg_, names);
                Value r = Value(+a.get_num_or_nan());
                if (!r.is_num())
                    throw Phrase_Error(*unary, "domain error");
                return r;
            }
        default:
            assert(0);
        }
    }

    auto binary = dynamic_cast<Binary_Phrase*>(&expr);
    if (binary != nullptr) {
        switch (binary->op_.kind) {
        case Token::k_plus:
            {
                Value a = eval(*binary->left_, names);
                Value b = eval(*binary->right_, names);
                Value r = Value(a.get_num_or_nan() + b.get_num_or_nan());
                if (!r.is_num())
                    throw Phrase_Error(*binary, "domain error");
                return r;
            }
        case Token::k_minus:
            {
                Value a = eval(*binary->left_, names);
                Value b = eval(*binary->right_, names);
                Value r = Value(a.get_num_or_nan() - b.get_num_or_nan());
                if (!r.is_num())
                    throw Phrase_Error(*binary, "domain error");
                return r;
            }
        case Token::k_times:
            {
                Value a = eval(*binary->left_, names);
                Value b = eval(*binary->right_, names);
                Value r = Value(a.get_num_or_nan() * b.get_num_or_nan());
                if (!r.is_num())
                    throw Phrase_Error(*binary, "domain error");
                return r;
            }
        case Token::k_over:
            {
                Value a = eval(*binary->left_, names);
                Value b = eval(*binary->right_, names);
                Value r = Value(a.get_num_or_nan() / b.get_num_or_nan());
                if (!r.is_num())
                    throw Phrase_Error(*binary, "domain error");
                return r;
            }
        default:
            assert(0);
        }
    }

    auto paren = dynamic_cast<Paren_Phrase*>(&expr);
    if (paren != nullptr) {
        return eval(*paren->arg_, names);
    }

    assert(0);
}
