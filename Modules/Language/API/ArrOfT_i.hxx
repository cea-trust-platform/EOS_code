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




#ifndef ArrOfT_inlines
#define ArrOfT_inlines 1
#include <string.h>
#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"
#else
#define ZoneScopedNS(x, y)
#define ZoneText(x, y)
#endif

namespace LANGUAGE_KERNEL
{

  template <class T> 
  inline Vdata<T>::
  Vdata() : 
    sz(0), 
    data(0), 
    ref_count(1), 
    owner(1)
  {
  }
  template <class T> 
  inline Vdata<T>::
  Vdata(int s) : 
    sz(s), 
    ref_count(1), 
    owner(1)
  { ZoneScopedNS("Vdata::Vdata", 5);
    if (s <= 0)
      data = 0;
    else
      try {
        data = new T[s];
        // test
        for (int i=0;i<s;i++) data[i]=0;
        // test
      }
      catch(...) {
        cerr << "Not enough memory." << endl;
        assert(0);
        exit(-1);
      }
  }
  template <class T> 
  inline Vdata<T>::
  Vdata(int s, const T* ptr) : 
    sz(s), 
    data((T*) ptr),
    ref_count(1), 
    owner(0)
  {
  }
  template <class T> 
  inline Vdata<T>::
  ~Vdata() 
  { ZoneScopedNS("Vdata<T>::~Vdata", 5);
    if(owner && data)
      delete[] data;
  }
  template <class T> 
  inline int Vdata<T>::add_one_ref()
  {
    return ++ref_count;
  }
  template <class T> 
  inline int Vdata<T>::supr_one_ref()
  {
    return (--ref_count) ;
  }
  template <class T> 
  inline ArrOf<T>::
  ~ArrOf()
  { ZoneScopedNS("ArrOf<T>::~ArrOf", 5);
    detach();
  }
  template <class T> 
  inline ArrOf<T>::
  ArrOf(int n, const T& x) :
    p(new Vdata<T>(n)), data(p->data)
  {
    *this = x;
  }
  template <class T> 
  inline ArrOf<T>::
  ArrOf(const T* ptr, int n, const T& x) :
    p(new Vdata<T>(n,ptr)), data(p->data)
  {
    *this = x;
  }
  template <class T> 
  inline ArrOf<T>::
  ArrOf(int n, const T* ptr) :
    p(new Vdata<T>(n,ptr)), data(p->data)
  {
  }
  template <class T> 
  inline ArrOf<T>::
  ArrOf(const ArrOf<T>& A):
    p(new Vdata<T>(A.size())), data(p->data)
  {
    copy(A);
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  copy(const ArrOf<T>& A)
  {
    ZoneScopedNS("ArrOf<T>::copy", 5);
    resize(A.size());
    return inject(A);
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  inject(const ArrOf<T>& A)
  {
    ZoneScopedNS("ArrOf<T>::inject", 5);
    assert(A.size() <= size());
    memcpy(data, A.get_ptr(), A.size()*sizeof(T));
    return *this;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator=(const ArrOf<T>& A)
  {
    ZoneScopedNS("ArrOf<T>::operator=", 5);
    ZoneText("by copy", sizeof("by copy"));
    copy(A);
    return *this;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator=(const T& x)
  {
    ZoneScopedNS("ArrOf<T>::operator=", 5);
    ZoneText("by filling", sizeof("by filling"));
    if (data) {
      int i = size();
      T* vv=&data[i];
      while(i--) *--vv=x;
    // alternative aux 3 lignes precedentes
    /*    int n = size();
    for (int i=0;i<n;i++)
      {
	data[i]=x;
	}*/
    // alternative aux 3 lignes precedentes
    }
    return *this;
  }
  template <class T> 
  inline T& ArrOf<T>::
  operator[](int i)
  {
    assert(i<size()); // commenter pour gagner en perf en mode optim
    assert(i >= 0);
    return data[i];
  }
  template <class T> 
  inline const T& ArrOf<T>::
  operator[](int i) const
  {
    assert(data != NULL);
    assert(i<size());
    assert(i >= 0);
    return data[i];
  }
  template <class T> 
  inline T& ArrOf<T>::
  operator()(int i)
  {
    assert(data != NULL);
    assert(i<size());
    assert(i >= 0);
    return data[i];
  }
  template <class T> 
  inline const T& ArrOf<T>::
  operator()(int i) const
  {
    assert(data != NULL);
    assert(i<size());
    assert(i >= 0);
    return data[i];
  }
  template <class T> 
  inline int ArrOf<T>::
  size() const
  {
    return p->sz;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  resize(int n)
  {
    ZoneScopedNS("ArrOf<T>::resize", 5);
    assert(p);
    assert(n>=0);
    if(size()==n) return *this;
    Vdata<T>* np=new Vdata<T>(n);
    data=np->data;
    int oldsz=size();
    int m= ((n) < (oldsz) ? (n) : (oldsz));
    memcpy(data,p->data,m*sizeof(T));
    // alternative au memcpy
    /*    for (int kk=0;kk<m;kk++)
	  {
	  data[kk]=p->data[kk];
	  }*/
    // alternative au memcpy
    if (n>oldsz)
      {
	for (int i=oldsz; i<n; i++)
	  data[i]=0;
      }
    detach();
    p=np;
    assert(p);
    return *this;
  }
  template <class T> 
  inline int ArrOf<T>::
  ref_count() const
  {
    return p->ref_count;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator+=(const ArrOf<T>& y)
  {
    ZoneScopedNS("ArrOf<T>::operator+=", 5);
    ZoneText("with another array", sizeof("with another array"));
    assert(size()==y.size());
    assert(p);
     T* dx=(T*) get_ptr();
     const T* dy= (const T*) y.get_ptr();
    int sz=size();
    for(int i=0; i<sz; i++)
      *dx++ +=*dy++;
    assert(p);
    return *this;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator-=(const ArrOf<T>& y)
  {
    ZoneScopedNS("ArrOf<T>::operator-=", 5);
    ZoneText("with another array", sizeof("with another array"));
    assert(size()==y.size());
    assert(p);
     T* dx=(T*) get_ptr();
     const T* dy= (const T*) y.get_ptr();
    int sz=size();
    for(int i=0; i<sz; i++)
      *dx++ -=*dy++;
    assert(p);
    return *this;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator+=(const T& y)
  {
    ZoneScopedNS("ArrOf<T>::operator+=", 5);
    ZoneText("with an element", sizeof("with an element"));
    assert(p);
     T* dx=(T*) get_ptr();
    int sz=size();
    for(int i=0; i<sz; i++)
      *dx++ +=y;
    assert(p);
    return *this;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator-=(const T& y)
  {
    ZoneScopedNS("ArrOf<T>::operator-=", 5);
    ZoneText("with an element", sizeof("with an element"));
    assert(p);
     T* dx=(T*) get_ptr();
    int sz=size();
    for(int i=0; i<sz; i++)
      *dx++ -=y;
    assert(p);
    return *this;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator*=(const T& y)
  {
    ZoneScopedNS("ArrOf<T>::operator*=", 5);
    assert(p);
     T* dx=(T*) get_ptr();
    int sz=size();
    for(int i=0; i<sz; i++)
      *dx++ *=y;
    assert(p);
    return *this;
  }
  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  operator/=(const T& y)
  {
    ZoneScopedNS("ArrOf<T>::operator/=", 5);
    assert(p);
     T* dx=(T*) get_ptr();
    int sz=size();
    for(int i=0; i<sz; i++)
      *dx++ /=y;
    assert(p);
    return *this;
  }
  template <class T> 
  inline int ArrOf<T>::
  detach()
  {
    ZoneScopedNS("ArrOf<T>::detach", 5);
    int retour=0;
    if(p)
      if ((p->supr_one_ref()) == 0)
	{
	  delete p;
	  p = 0;
	  retour = 1;
	}
    return retour;
  }
  template <class T> 
  inline void ArrOf<T>::
  attach(const ArrOf<T>& m)
  {
    ZoneScopedNS("ArrOf<T>::attach", 5);
    detach();
    m.add_one_ref();
    p = m.p;
    data = m.data;
  }

template <class T>
inline void Vdata<T>::set_view(int s, const T* ptr)
{
    if (owner && data)
        delete[] data;

    sz = s;
    data = (T*)ptr;
    owner = 0;
}

  template <class T> 
  inline ArrOf<T>& ArrOf<T>::
  set_ptr(int nsz, const T* ptr)
  {
    ZoneScopedNS("ArrOf<T>::set_ptr", 5);
    if (p && p->ref_count == 1)
    {
      p->set_view(nsz, ptr);   // réutilise le Vdata en place, pas de new/delete
      data = p->data;
    }
    else
    {
      detach();
      p = new Vdata<T>(nsz, ptr);
      data = p->data;
    }
    return *this;
  }
  template <class T> 
  inline const T* ArrOf<T>::
  get_ptr() const
  {
    return data;
  }
  template <class T> 
  inline void ArrOf<T>::
  add_one_ref() const
  {
    p->add_one_ref();
  }
  template <class T> 
  inline void ArrOf<T>::clear()
  { ZoneScopedNS("ArrOf<T>::clear", 5);
    if (p)
    {
      if (p->owner && p->data)
        delete[] p->data;  

      p->data = nullptr;
      p->sz = 0;
      p->ref_count = 1; 
      p->owner = 1;
    }
  }
}
#endif
