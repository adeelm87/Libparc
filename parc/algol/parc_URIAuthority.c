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
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <LongBow/runtime.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdarg.h>

#include <parc/algol/parc_URIAuthority.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_SafeMemory.h>

struct parc_uri_authority {
    char *userinfo;
    char *hostName;
    long port;
};

static void
_parcURIAuthority_Finalize(PARCURIAuthority **authorityPtr)
{
    PARCURIAuthority *authority = *authorityPtr;
    parcMemory_SafeFree(authority->userinfo);
    parcMemory_SafeFree(authority->hostName);
}

parcObject_ExtendPARCObject(PARCURIAuthority, _parcURIAuthority_Finalize, NULL, NULL, parcURIAuthority_Equals, NULL, NULL, NULL);

PARCURIAuthority *
parcURIAuthority_Create(void)
{
    PARCURIAuthority *result = parcObject_CreateInstance(PARCURIAuthority);
    result->userinfo = 0;
    result->hostName = 0;
    result->port = 0;
    return result;
}

parcObject_ImplementAcquire(parcURIAuthority, PARCURIAuthority);

parcObject_ImplementRelease(parcURIAuthority, PARCURIAuthority);

PARCURIAuthority *
parcURIAuthority_Parse(const char *authority)
{
    PARCURIAuthority *result = parcURIAuthority_Create();

    char *atSign = strchr(authority, '@');
    if (atSign != NULL) {
        result->userinfo = parcMemory_StringDuplicate(authority, atSign - authority);
        authority = ++atSign;
    }
    // Support literal IPv6 address specifications (i.e. [::0]:80)
    char *rightBracket = strrchr(authority, ']');
    char *lastColon = strrchr(authority, ':');
    if (rightBracket != NULL) {
        result->hostName = parcMemory_StringDuplicate(authority, rightBracket - authority + 1);
        if ((lastColon - rightBracket) > 0) {
            result->port = (short) strtol(++lastColon, NULL, 10);
        }
    } else if (lastColon != NULL) {
        result->hostName = parcMemory_StringDuplicate(authority, lastColon - authority);
        result->port = (short) strtol(++lastColon, NULL, 10);
    } else {
        result->hostName = parcMemory_StringDuplicate(authority, strlen(authority));
    }

    return result;
}

const char *
parcURIAuthority_GetUserInfo(const PARCURIAuthority *authority)
{
    return authority->userinfo;
}

const char *
parcURIAuthority_GetHostName(const PARCURIAuthority *authority)
{
    return authority->hostName;
}

long
parcURIAuthority_GetPort(const PARCURIAuthority *authority)
{
    return authority->port;
}

bool
parcURIAuthority_Equals(const PARCURIAuthority *authA, const PARCURIAuthority *authB)
{
    if (authA == authB) {
        return true;
    }
    if (authA == NULL || authB == NULL) {
        return false;
    }

    if (strcmp(authA->hostName, authB->hostName) == 0) {
        if (strcmp(authA->userinfo, authB->userinfo) == 0) {
            if (authA->port == authB->port) {
                return true;
            }
            return false;
        }
        return false;
    }

    return false;
}
