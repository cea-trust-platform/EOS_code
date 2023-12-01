/****************************************************************************
* Copyright (c) 2023, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/



#ifndef ArrOfT_h
#define ArrOfT_h 1

namespace LANGUAGE_KERNEL
{
  template <class T> 
  class Vdata
  {
  public :
    Vdata();
    Vdata(int);
    Vdata(int, const T* );
    ~Vdata();
    int add_one_ref();
    int supr_one_ref();
    int   sz;
    T* data;
    int   ref_count;
    int owner;
  };

  
  template <class T> class ArrOf 
  {
  public :
    virtual ~ArrOf();
    ArrOf(int n=0, const T& x=0);
    ArrOf(const T* ptr, int n, const T& x=0);
    ArrOf(int n, const T* ptr);
    ArrOf(const ArrOf<T>& );
    ArrOf<T>& operator=(const ArrOf<T>&);
    ArrOf<T>& operator=(const T& x);
    
    T& operator[](int i);
    const T& operator[](int i) const ;
    T& operator()(int i);
    const T& operator()(int i) const ;
    
    int size() const;
    ArrOf<T>& resize(int) ;
    int ref_count() const;
    
    T max_array() const;
    T min_array() const;
    T max_abs_array() const;
    T min_abs_array() const;
    void carre_array();
    void racine_carree_array();
    T norme_array() const;
    ArrOf<T>& ajoute_array(T alpha, const ArrOf<T>& y); //x+=alpha*y
    ArrOf<T>& ajoute_carre_array(T alpha, const ArrOf<T>& y);
    
    ArrOf<T>& operator+=(const ArrOf<T>&);
    ArrOf<T>& operator+=(const T&);
    ArrOf<T>& operator-=(const ArrOf<T>&);
    ArrOf<T>& operator-=(const T&);
    ArrOf<T>& operator *= (const T&) ;
    ArrOf<T>& operator /= (const T&) ;
    
    ArrOf<T>& set_ptr(int nsz, const T* ptr); 
    const T*  get_ptr() const; 
    ArrOf<T>& inject(const ArrOf<T>&);
    ArrOf<T>& copy(const ArrOf<T>&);
    void attach(const ArrOf<T>&);
    int detach();
    
  protected :
    Vdata<T>* p;
    T* data;
    void add_one_ref() const;
  };
}

#include "Language/API/ArrOfT_i.hxx"
#endif
