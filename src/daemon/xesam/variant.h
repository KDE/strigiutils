#ifndef VARIANT_H
#define VARIANT_H

#include <string>
#include <vector>
#include <sstream>

/**
 * Simple inefficient implementation of a variant type as needed by xesam
 **/
class Variant {
public:
    enum VarType {b_val, i_val, s_val, as_val};
private:
    int32_t i_value;
    std::string s_value;
    std::vector<std::string> as_value;
    VarType vartype;
public:
    Variant() :i_value(0), vartype(b_val) {}
    Variant(bool v) { *this = v; }
    Variant(int32_t v) { *this = v; }
    Variant(const std::string& v) { *this = v; }
    Variant(const std::vector<std::string>& v) { *this = v; }
    Variant(const Variant& v) { *this = v; }
    VarType type() const { return vartype; }
    const Variant& operator=(bool v) {
        i_value = v;
        vartype = b_val;
        return *this;
    } 
    const Variant& operator=(int32_t v) {
        i_value = v;
        vartype = i_val;
        return *this;
    } 
    const Variant& operator=(const std::string& v) {
        s_value = v;
        vartype = s_val;
        return *this;
    }
    const Variant& operator=(const std::vector<std::string>& v) {
        as_value = v;
        vartype = as_val;
        return *this;
    }
    const Variant& operator=(const Variant& v) {
        i_value = v.i_value;
        s_value = v.s_value;
        as_value = v.as_value;
        vartype = v.vartype;
        return *this;
    }
    bool b() const {
        switch (vartype) {
        case b_val:
        case i_val:
             return i_value;
        case s_val:
             return s_value == "1" || s_value == "true" || s_value == "True"
                 || s_value == "TRUE";
        case as_val:
             return as_value.size();
        default:
             return false;
        }
    }
    int32_t i() const {
        switch (vartype) {
        case b_val:
        case i_val:
             return i_value;
        case s_val:
             return atoi(s_value.c_str());
        case as_val:
             return as_value.size();
        default:
             return -1;
        }
    }
    static std::string itos(int32_t i) {
        std::ostringstream o;
        o << i;
        return o.str();
    }
    std::string s() const {
        switch (vartype) {
        case b_val:
             return i_value ?"true" :"false";
        case i_val:
             return itos(i_value);
        case s_val:
             return s_value;
        case as_val:
             return as_value.size() ?as_value[0] :"";
        default:
             return "";
        }
    }
    std::vector<std::string> as() const {
        if (vartype == as_val) {
            return as_value;
        }
        std::vector<std::string> v;
        if (b()) {
            v.push_back(s());
        }
        return v;
    }
};

#endif
