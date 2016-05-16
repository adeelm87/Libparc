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
 * @header parcCryptoHash.c
 * <#Abstract#>
 *
 *     <#Discussion#>
 *
 * @author Mosko, Marc
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
#include <config.h>
#include <stdio.h>

#include <LongBow/runtime.h>

#include <parc/security/parc_CryptoHash.h>
#include <parc/algol/parc_Memory.h>

#include <parc/algol/parc_Object.h>

struct parc_crypto_hash {
    PARCCryptoHashType type;
    PARCBuffer *digestBuffer;
};

static void
_parcCryptoHash_FinalRelease(PARCCryptoHash **hashP)
{
    PARCCryptoHash *hash = (PARCCryptoHash *) *hashP;
    if (hash->digestBuffer != NULL) {
        parcBuffer_Release(&hash->digestBuffer);
    }
}

parcObject_ExtendPARCObject(PARCCryptoHash, _parcCryptoHash_FinalRelease, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementAcquire(parcCryptoHash, PARCCryptoHash);

parcObject_ImplementRelease(parcCryptoHash, PARCCryptoHash);

PARCCryptoHash *
parcCryptoHash_Create(PARCCryptoHashType digestType, PARCBuffer *digestBuffer)
{
    PARCCryptoHash *parcDigest = parcObject_CreateAndClearInstance(PARCCryptoHash);
    assertNotNull(parcDigest, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(PARCCryptoHash));
    parcDigest->type = digestType;

    parcDigest->digestBuffer = parcBuffer_Acquire(digestBuffer);

    return parcDigest;
}

/**
 * Create a digest, copying the buffer
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCCryptoHash *
parcCryptoHash_CreateFromArray(PARCCryptoHashType digestType, const void *buffer, size_t length)
{
    PARCCryptoHash *parcDigest = parcObject_CreateAndClearInstance(PARCCryptoHash);
    assertNotNull(parcDigest, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(PARCCryptoHash));
    parcDigest->type = digestType;

    // create a reference counted copy
    parcDigest->digestBuffer =
        parcBuffer_Flip(parcBuffer_PutArray(parcBuffer_Allocate(length), length, buffer));

    return parcDigest;
}

/**
 * Returns the digest algorithm.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCCryptoHashType
parcCryptoHash_GetDigestType(const PARCCryptoHash *parcDigest)
{
    assertNotNull(parcDigest, "Parameter must be non-null");
    return parcDigest->type;
}

bool
parcCryptoHash_Equals(const PARCCryptoHash *a, const PARCCryptoHash *b)
{
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }

    if (a->type == b->type) {
        if (parcBuffer_Equals(a->digestBuffer, b->digestBuffer)) {
            return true;
        }
    }

    return false;
}

PARCBuffer *
parcCryptoHash_GetDigest(const PARCCryptoHash *parcDigest)
{
    assertNotNull(parcDigest, "Parameter must be non-null");
    return parcDigest->digestBuffer;
}
