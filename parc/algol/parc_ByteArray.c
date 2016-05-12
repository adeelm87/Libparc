/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 *
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_ByteArray.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_Hash.h>
#include <parc/algol/parc_DisplayIndented.h>

struct parc_byte_array {
    uint8_t *array;
    size_t length;
    void (*freeFunction)(void **);
};
#define MAGIC 0x0ddba11c1a55e5

static inline void
_trapIfOutOfBounds(const PARCByteArray *array, const size_t index)
{
    trapOutOfBoundsIf(index >= array->length, "parcByteArray index %zd exceeds the length %zd", index, array->length);
}

static bool
_parcByteArray_Destructor(PARCByteArray **byteArrayPtr)
{
    PARCByteArray *byteArray = *byteArrayPtr;

    if (byteArray->freeFunction != NULL) {
        if (byteArray->array != NULL) {
            byteArray->freeFunction((void **) &(byteArray->array));
        }
    }
    return true;
}

parcObject_Override(PARCByteArray, PARCObject,
                    .destructor = (PARCObjectDestructor *) _parcByteArray_Destructor,
                    .copy       = (PARCObjectCopy *) parcByteArray_Copy,
                    .equals     = (PARCObjectEquals *) parcByteArray_Equals,
                    .compare    = (PARCObjectCompare *) parcByteArray_Compare,
                    .hashCode   = (PARCObjectHashCode *) parcByteArray_HashCode,
                    .display    = (PARCObjectDisplay *) parcByteArray_Display);

void
parcByteArray_AssertValid(const PARCByteArray *instance)
{
    trapInvalidValueIf(parcByteArray_IsValid(instance) == false,
                       "PARCByteArray instance is invalid.");
}

bool
parcByteArray_IsValid(const PARCByteArray *instance)
{
    bool result = false;

    if (instance != NULL) {
        if (instance->length == 0 || instance->array != NULL) {
            result = true;
        }
    }

    return result;
}

PARCByteArray *
parcByteArray_Allocate(const size_t length)
{
    uint8_t *array = NULL;
    if (length > 0) {
        array = parcMemory_AllocateAndClear(sizeof(uint8_t) * length);
        if (array == NULL) {
            return NULL;
        }
    }
    PARCByteArray *result = parcObject_CreateInstance(PARCByteArray);

    if (result != NULL) {
        result->array = array;
        result->length = length;
        result->freeFunction = parcMemory_DeallocateImpl;
        return result;
    } else {
        parcMemory_Deallocate(&array);
    }
    return NULL;
}

PARCByteArray *
parcByteArray_Wrap(const size_t length, uint8_t array[length])
{
    if (array != NULL) {
        PARCByteArray *result = parcObject_CreateInstance(PARCByteArray);
        if (result != NULL) {
            result->array = array;
            result->length = length;
            result->freeFunction = NULL;
            return result;
        }
    }
    return NULL;
}

parcObject_ImplementAcquire(parcByteArray, PARCByteArray);

parcObject_ImplementRelease(parcByteArray, PARCByteArray);

PARCByteArray *
parcByteArray_Copy(const PARCByteArray *original)
{
    parcByteArray_OptionalAssertValid(original);

    PARCByteArray *result = NULL;

    if (original != NULL) {
        result = parcByteArray_Allocate(original->length);
        memcpy(result->array, original->array, result->length);
    }

    return result;
}

size_t
parcByteArray_Capacity(const PARCByteArray *byteArray)
{
    parcByteArray_OptionalAssertValid(byteArray);
    return byteArray->length;
}

PARCByteArray *
parcByteArray_PutByte(PARCByteArray *result, size_t index, uint8_t byte)
{
    parcByteArray_OptionalAssertValid(result);
    _trapIfOutOfBounds(result, index);

    result->array[index] = byte;
    return result;
}

uint8_t
parcByteArray_GetByte(const PARCByteArray *array, size_t index)
{
    parcByteArray_OptionalAssertValid(array);
    _trapIfOutOfBounds(array, index);

    return array->array[index];
}

uint8_t *
parcByteArray_Array(const PARCByteArray *byteArray)
{
    parcByteArray_OptionalAssertValid(byteArray);
    return byteArray->array;
}

uint8_t *
parcByteArray_AddressOfIndex(const PARCByteArray *array, const size_t index)
{
    parcByteArray_OptionalAssertValid(array);
    _trapIfOutOfBounds(array, index);

    return &array->array[index];
}

int
parcByteArray_Compare(const PARCByteArray *x, const PARCByteArray *y)
{
    if (x == y) {
        return 0;
    }

    if (x == NULL) {
        return -1;
    }

    if (y == NULL) {
        return +1;
    }

    if (parcByteArray_Capacity(x) > parcByteArray_Capacity(y)) {
        return +1;
    }
    if (parcByteArray_Capacity(x) < parcByteArray_Capacity(y)) {
        return -1;
    }

    return memcmp(x->array, y->array, parcByteArray_Capacity(x));
}

PARCByteArray *
parcByteArray_PutBytes(PARCByteArray *result, size_t offset, size_t length, const uint8_t source[length])
{
    parcByteArray_OptionalAssertValid(result);
    trapOutOfBoundsIf(offset > result->length,
                      "The offset (%zd) exeeds the length (%zd) of the PARCByteArray.", offset, result->length);

    size_t available = result->length - offset;

    trapOutOfBoundsIf(length > available, "%zd available bytes, %zd required.", available, length);

    memcpy(&result->array[offset], source, length);
    return result;
}

PARCByteArray *
parcByteArray_GetBytes(const PARCByteArray *result, size_t offset, size_t length, uint8_t array[length])
{
    parcByteArray_OptionalAssertValid(result);

    size_t available = result->length - offset;

    trapOutOfBoundsIf(length > available, "parcByteArray_CopyOut %zd available bytes, %zd required", available, length);

    memcpy(array, &result->array[offset], length);
    return (PARCByteArray *) result;
}

PARCByteArray *
parcByteArray_ArrayCopy(PARCByteArray *destination, size_t destOffset, const PARCByteArray *source, size_t srcOffset, size_t length)
{
    parcByteArray_OptionalAssertValid(destination);
    parcByteArray_OptionalAssertValid(source);

    memcpy(&destination->array[destOffset], &source->array[srcOffset], length);
    return destination;
}

bool
parcByteArray_Equals(const PARCByteArray *a, const PARCByteArray *b)
{
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }
    if (a->length == b->length) {
        return memcmp(a->array, b->array, a->length) == 0;
    }
    return false;
}

static void
_parcByteArray_PrettyPrintLine(const unsigned char *memory, size_t offset, size_t length)
{
    int bytesPerLine = 16;
    char accumulator[bytesPerLine + 1];
    memset(accumulator, ' ', bytesPerLine);
    accumulator[bytesPerLine] = 0;

    printf("%5zd: ", offset);

    for (int i = 0; i < bytesPerLine; i++) {
        if (offset + i >= length) {
            printf("      ");
            accumulator[i] = ' ';
        } else {
            char c = memory[offset + i];
            printf("0x%02x, ", c & 0xFF);
            if (isprint(c)) {
                accumulator[i] = c;
            } else {
                accumulator[i] = '.';
            }
        }
    }
    printf("   %s\n", accumulator);
}

void
parcByteArray_Display(const PARCByteArray *array, int indentation)
{
    int bytesPerLine = 16;

    if (array->array == NULL) {
        parcDisplayIndented_PrintLine(indentation, "PARCByteArray@NULL");
    } else {
        parcDisplayIndented_PrintLine(indentation, "PARCByteArray@%p = [0,%zd)", (void *) array, array->length);

        for (size_t offset = 0; offset < array->length; offset += bytesPerLine) {
            _parcByteArray_PrettyPrintLine(array->array, offset, array->length);
        }
    }
}

PARCHashCode
parcByteArray_HashCode(const PARCByteArray *array)
{
    parcByteArray_OptionalAssertValid(array);
    return parcHashCode_Hash(array->array, array->length);
}
