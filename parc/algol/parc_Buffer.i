%module (jniclassname="ParcBuffer") test
%{
#include "parc_Buffer.h"
%}

%include "stdint.i"

%include "parc_Buffer.h"

/* Helper Functions */
%inline %{
void parcBuffer_Release1(PARCBuffer *bufferPtr) {
    parcBuffer_Release(&bufferPtr);
}
%}
