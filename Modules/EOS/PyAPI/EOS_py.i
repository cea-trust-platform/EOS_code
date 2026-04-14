%module eos_py

%{
#include "EOS_py.hxx"
%}

%include "std_string.i"
%include "std_vector.i"

namespace std {
    %template(DoubleVector) vector<double>;
    %template(StringVector) vector<string>;
    %template(DoubleVectorVector) vector<vector<double>>;
}

%include "EOS_py.hxx"