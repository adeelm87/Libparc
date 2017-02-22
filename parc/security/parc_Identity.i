%module (jniclassname="ParcIdentity") test
%{
#include "parc_Identity.h"
%}

%include "parc_Identity.h"

/* Helper Functions */
%inline %{
void parcIdentity_Release1(PARCIdentity *identityPtr) {
     parcIdentity_Release(&identityPtr);
}
%}
