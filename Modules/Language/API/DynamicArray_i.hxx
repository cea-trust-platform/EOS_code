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




// Parameterized Class DynamicArray 

namespace LANGUAGE_KERNEL
{
  template <class T>
  inline T& DynamicArray<T>::operator[](const int index) const
  {
    assert(index < size());
    return objs[index];
  }


  template <class T>
  inline int DynamicArray<T>::size () const
  {
    return nb_objs;
  }


  template <class T>
  inline const T* DynamicArray<T>::get_objs () const
  {
    return objs;
  }

  template <class T>
  inline void DynamicArray<T>::set_objs (T* value)
  {
    objs = value;
  }

  template <class T>
  int DynamicArray<T>::step = 64;

  template <class T>
  inline DynamicArray<T>::DynamicArray(int sz)
    : objs(new T[sz]),
      nb_objs(sz),
      nb_objs_max(sz)
  {
  }
  template <class T>
  inline DynamicArray<T>::DynamicArray()
    : objs(0),
      nb_objs(0),
      nb_objs_max(0)
  {
  }

  template <class T>
  inline DynamicArray<T>::DynamicArray(const DynamicArray<T> &right)
    : objs(new T(right.nb_objs)),
      nb_objs(right.nb_objs),
      nb_objs_max(right.nb_objs_max)
  {
    for(int i=0; i<nb_objs;i++)
      objs[i]=right.objs[i];
  }


  template <class T>
  inline DynamicArray<T>::~DynamicArray()
  {
    if(objs)
      delete[] objs;
  }


  template <class T>
  inline DynamicArray<T> & DynamicArray<T>::operator=(const DynamicArray<T> &right)
  {
    if(objs)
      delete[] objs;
    objs=new T(right.nb_objs);
    nb_objs=right.nb_objs;
    nb_objs_max=right.nb_objs_max;
    for(int i=0; i<nb_objs;i++)
      objs[i]=right.objs[i];
    return *this;
  }


  template <class T>
  inline int DynamicArray<T>::operator==(const DynamicArray<T> &right) const
  {
    if(nb_objs!=right.nb_objs)
      return False;

    for(int i=0; i<nb_objs;i++)
      if(objs[i]!=right.objs[i])
	return False;
    return True;
  }

  template <class T>
  inline int DynamicArray<T>::operator!=(const DynamicArray<T> &right) const
  {
    return !( (*this)==(right));
  }


  template <class T>
  inline ostream & operator<<(ostream &stream,const DynamicArray<T> &right)
  {
    for(int i=0; i<right.nb_objs;i++)
      stream << right[i] << " ";
    return stream;
  }

  template <class T>
  inline istream & operator>>(istream &stream,DynamicArray<T> &object)
  {
    for(int i=0; i<object.nb_objs;i++)
      stream >> object[i];
    return stream;
  }



  template <class T>
  inline int DynamicArray<T>::add_value (const T& to_add)
  {
    if(nb_objs==nb_objs_max)
      {
	int i;
	nb_objs_max+=step;
	T* newobjs=new T[nb_objs_max];
	for(i=0; i<nb_objs; i++)
	  newobjs[i]=objs[i];
	for(;i<nb_objs_max; i++)
	  newobjs[i]=-1;
	if(objs)
	  delete[] objs;
	objs=newobjs;
      }
    objs[nb_objs] = to_add;
    nb_objs++;
    return nb_objs-1;
  }

  template <class T>
  inline NEPTUNE::Boolean DynamicArray<T>::delete_value (const T& to_suppr)
  {
    int i;
    int index=-1;
    for(i=0; i<nb_objs; i++)
      if(objs[i]==to_suppr)
	{
	  index=i;
	  break;
	}
    if(index==-1)
      return false;
    for(i=index; i<nb_objs; i++)
      objs[i]=objs[i+1];
    nb_objs--;
    return True;
  }
}
