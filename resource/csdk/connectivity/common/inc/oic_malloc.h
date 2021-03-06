//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _OIC_MALLOC_H_
#define _OIC_MALLOC_H_

// The purpose of this module is to allow custom dynamic memory allocation
// code to easily be added to the TB Stack by redefining the OICMalloc and
// OICFree functions.  Examples of when this might be needed are on TB
// platforms that do not support dynamic allocation or if a memory pool
// needs to be added.
//
// Note that these functions are intended to be used ONLY within the TB
// stack and NOT by the application code.  The application should be
// responsible for its own dynamic allocation.

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------

/**
 * Allocates a block of size bytes, returning a pointer to the beginning of
 * the allocated block.
 *
 * NOTE: This function is intended to be used internally by the TB Stack.
 *       It is not intended to be used by applications.
 *
 * @param size - Size of the memory block in bytes, where size > 0
 *
 * @return
 *     on success, a pointer to the allocated memory block
 *     on failure, a null pointer is returned
 */
void *OICMalloc(size_t size);

/**
 * Deallocate a block of memory previously allocated by a call to OCMalloc
 *
 * NOTE: This function is intended to be used internally by the TB Stack.
 *       It is not intended to be used by applications.
 *
 * @param ptr - Pointer to block of memory previously allocated by OCMalloc.
 *              If ptr is a null pointer, the function does nothing.
 */
void OICFree(void *ptr);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* _OIC_MALLOC_H_ */
