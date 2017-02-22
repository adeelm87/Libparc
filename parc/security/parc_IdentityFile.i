%module (jniclassname="ParcIdentityFile") test
%{
#include "parc_IdentityFile.h"
%}

%include "parc_IdentityFile.h"

/* Helper Functions */
%inline %{
void parcIdentityFile_Release1(PARCIdentityFile *identityPtr) {
     parcIdentityFile_Release(&identityPtr);
}
%}
