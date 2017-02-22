%module (jniclassname="ParcMemory") test
%{
#include "parc_Memory.h"
%}

%include "stdint.i"

/* Helper Functions */
%inline %{
void parcMemory_Deallocate1(char *pointer) {
    parcMemory_Deallocate((void **)&pointer);
}
%}
