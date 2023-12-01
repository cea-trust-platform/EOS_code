%module eosihm

%include "typemaps.i"
%include "cstring.i"

%{
#include "eosihmSrc1.hxx"
%}

// Helpers functions

%{

  PyObject *float_array_to_list(double *array, int size) {

    PyObject *py_list;
    int i, err;

    
    py_list = PyList_New(size);
    for (i=0; i < size; i++){
      err = PyList_SetItem(py_list, i, Py_BuildValue("d", array[i]));
      if(err){
        char message[] = "Error in float_array_to_list";
        PyErr_SetString(PyExc_RuntimeError, message);
        return NULL;}
    }
    return Py_BuildValue("O", py_list);
  }

  %}

%{

  PyObject *int_array_to_list(int *array, int size) {

    PyObject *py_list;
    int i, err;

    
    py_list = PyList_New(size);
    for (i=0; i < size; i++){
      err = PyList_SetItem(py_list, i, Py_BuildValue("i", array[i]));
      if(err){
        char message[] = "Error in float_array_to_list";
        PyErr_SetString(PyExc_RuntimeError, message);
        return NULL;}
    }
    return Py_BuildValue("O", py_list);
  }

  %}

PyObject *
float_array_to_list(double * array, int size);


PyObject *
int_array_to_list(int * array, int size);


// arrays as output argument
// *************************
// float array
%typemap(in) (double *ARRAYOUT, int LEN){
  $2 = (int) PyInt_AsLong($input);
  $1 = (double *) malloc(sizeof(double) * $2);
}
%typemap(argout) (double *ARRAYOUT, int LEN) {
  PyObject* o;
  o = float_array_to_list($1, $2);
//  $result = t_output_helper($result, o);
        $result = o;
}  
%typemap(freearg) (double *ARRAYOUT, int LEN) { 
  if ($1) free($1); 
}
// int array
%typemap(in) (int *ARRAYOUT, int LEN){
  $2 = (int) PyInt_AsLong($input);
  $1 = (int *) malloc(sizeof(int) * $2);
}
%typemap(argout) (int *ARRAYOUT, int LEN) {
  PyObject* o;
  o = int_array_to_list($1, $2);
//  $result = t_output_helper($result, o);
        $result = o;
}  
%typemap(freearg) (int *ARRAYOUT, int LEN) { 
  if ($1) free($1); 
}

////////////////////////////////////////
#if defined(SWIGPYTHON)
%typemap(in) double * , const double * , const double * const
#endif
{
  /* typemap in for double * , const double * , const double * const */
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (double *) malloc(size*sizeof(double));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyFloat_Check(o))
        $1[i] = PyFloat_AsDouble(PyList_GetItem($input,i));
      else {
        PyErr_SetString(PyExc_TypeError,"list must contain floats");
        free($1);
        return NULL;
      }
    }
  }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}

%cstring_bounded_output(char *mychar, 1024);

////////////////////////////////////////


class EosIhm {

public:

  EosIhm();
  ~EosIhm();
  void createValue(int);
  void initValueByStep(double,double,int);
  void getValueAsList(double *ARRAYOUT, int LEN);
  void getErrorAsList(int *ARRAYOUT, int LEN);
  void computeValue(char *,char *,char *,char *,char *,double);
  int getNumberOfValue();
  void putTheArrays(double *,double *,double *,int,int,int,char *,char *,char *,char *,char *,char *,char *);

  void getTheErrorI(int  *OUTPUT);

  void getTheErrorC(char *mychar);

};

