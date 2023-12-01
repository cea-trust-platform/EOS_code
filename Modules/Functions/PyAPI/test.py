#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

from Language import *
from Functions import *

Language_init()

var=Strings(2)
var.set_value_at(0,AString("x"))
var.set_value_at(1,AString("y"))

f1=AFunction(AString("f1"), AString("x+y"), var)
f2=AFunction(AString("f2"),AString("3*x+y*x+y*y"),var)
print("_________________________________________________")
print(f1.get_value(1,1))
print(f2.get_value(1,1))
print("f1 =", f1.asString().aschar())
print("f1 variables ")
f1.get_main_variables().print_On()
print("f2 =", f2.asString().aschar())
f3= AFunction(f1.derivative(0),var)
f4= AFunction(f2.derivative(0),var)
print("f3 =", f3.asString().aschar())
print("f4 =", f4.asString().aschar())
print(f1.derivative(1).aschar())
print(f2.asString().aschar())

print(AFunction(AString("x"), AString("x")).get_value(1.0))
print(AFunction(AString("x"), AString("x")).get_value(2.0))
AFunction.print_All_Known_Functions()
