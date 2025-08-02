/*
 * val.cpp
 *
 * CS280
 * Spring 2024
*/

#include "val.h"

using namespace std;

//Int+Int = Int, Int+Real / Real+Int / Real+Real = Real, Else ERROR
Value Value::operator+(const Value& op) const{
    if (op.IsInt() && IsInt()) {return GetInt()+op.GetInt();}
    else if (op.IsReal() && IsReal()) {return GetReal()+op.GetReal();}
    else if (op.IsReal() && IsInt()) {return GetInt()+op.GetReal();}
    else if (op.IsInt() && IsReal()) {return GetReal()+op.GetInt();}
    else {return Value();}
}

//Int-Int = Int, Int-Real / Real-Int / Real-Real = Real, Else ERROR
Value Value::operator-(const Value& op) const{
    if (op.IsInt() && IsInt()) {return GetInt()-op.GetInt();}
    else if (op.IsReal() && IsReal()) {return GetReal()-op.GetReal();}
    else if (op.IsReal() && IsInt()) {return GetInt()-op.GetReal();}
    else if (op.IsInt() && IsReal()) {return GetReal()-op.GetInt();}
    else {return Value();}
}

//Int*Int = Int, Int*Real / Real*Int / Real*Real = Real, Else ERROR.
Value Value::operator*(const Value& op) const{
    if (op.IsInt() && IsInt()) {return GetInt()*op.GetInt();}
    else if (op.IsReal() && IsReal()) {return GetReal()*op.GetReal();}
    else if (op.IsReal() && IsInt()) {return GetInt()*op.GetReal();}
    else if (op.IsInt() && IsReal()) {return GetReal()*op.GetInt();}
    else {return Value();}
}

//Int/Int = Int, Int/Real | Real/Int | Real/Real = Real, Else ERROR
Value Value::operator/(const Value& op) const{
    if (op.IsInt() && IsInt()) {return GetInt()/op.GetInt();}
    else if (op.IsReal() && IsReal()) {return GetReal()/op.GetReal();}
    else if (op.IsReal() && IsInt()) {return GetInt()/op.GetReal();}
    else if (op.IsInt() && IsReal()) {return GetReal()/op.GetInt();}
    else {return Value();}
}

//String Catenate String = String, Else ERROR.
Value Value::Catenate(const Value& op) const{
    if (op.IsString() && IsString()) {return (GetString() + op.GetString());}
    else {return Value();}
}

//Int Power Int / Int Power Real / Real Power Int / Real Power Real = Real, Else ERROR.
Value Value::Power(const Value& op) const{
    if (op.IsInt() && IsInt()) {return pow(GetInt(), op.GetInt());}
    else if (op.IsReal() && IsReal()) {return pow(GetReal(), op.GetReal());}
    else if (op.IsReal() && IsInt()) {return pow(GetInt(), op.GetReal());}
    else if (op.IsInt() && IsReal()) {return pow(GetReal(), op.GetInt());}
    else {return Value();}
}

//Int==Int / Int==Real / Real==Int / Real==Real = true / false, Else ERROR.
Value Value::operator==(const Value& op) const{
    if (op.IsInt() && IsInt()) {return GetInt() == op.GetInt();}
    else if (op.IsReal() && IsReal()) {return GetReal() == op.GetReal();}
    else if (op.IsReal() && IsInt()) {return GetInt() == op.GetReal();}
    else if (op.IsInt() && IsReal()) {return GetReal() == op.GetInt();}
    else {return Value();}
}

//Int>Int / Int>Real / Real>Int / Real>Real = true / false, Else ERROR.
Value Value::operator>(const Value& op) const{
    if (op.IsInt() && IsInt()) {return GetInt() > op.GetInt();}
    else if (op.IsReal() && IsReal()) {return GetReal() > op.GetReal();}
    else if (op.IsReal() && IsInt()) {return GetInt() > op.GetReal();}
    else if (op.IsInt() && IsReal()) {return GetReal() > op.GetInt();}
    else {return Value();}
}

//Int<Int / Int<Real / Real<Int / Real<Real = true / false, Else ERROR.
Value Value::operator<(const Value& op) const{
    if (op.IsInt() && IsInt()) {return GetInt() < op.GetInt();}
    else if (op.IsReal() && IsReal()) {return GetReal() < op.GetReal();}
    else if (op.IsReal() && IsInt()) {return GetInt() < op.GetReal();}
    else if (op.IsInt() && IsReal()) {return GetReal() < op.GetInt();}
    else {return Value();}
}