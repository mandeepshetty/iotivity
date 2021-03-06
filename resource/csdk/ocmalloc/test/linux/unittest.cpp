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


extern "C" {
    #include "ocmalloc.h"
}

#include "gtest/gtest.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <stdint.h>
using namespace std;

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static uint8_t *pBuffer;

//-----------------------------------------------------------------------------
//  Tests
//-----------------------------------------------------------------------------

TEST(OCMalloc, MallocPass1)
{
    // Try to allocate a small buffer
    pBuffer = (uint8_t *)OCMalloc(1);
    EXPECT_TRUE(pBuffer);
    OCFree(pBuffer);
}

TEST(OCMalloc, MallocPass2)
{
    // Try to allocate a small buffer
    pBuffer = (uint8_t *)OCMalloc(128);
    EXPECT_TRUE(pBuffer);
    OCFree(pBuffer);
}

TEST(OCMalloc, MallocFail1)
{
    // Try to allocate a buffer of size 0
    pBuffer = (uint8_t *)OCMalloc(0);
    EXPECT_TRUE(NULL == pBuffer);
    OCFree(pBuffer);
}

TEST(OCMalloc, MallocFail2)
{
    // Try to allocate a ridiculous amount of RAM
    pBuffer = (uint8_t *)OCMalloc((size_t)0x7FFFFFFFFFFFFFFF);
    EXPECT_TRUE(NULL == pBuffer);
    OCFree(pBuffer);
}

TEST(OCCalloc, CallocPass1)
{
    // Try to allocate a small buffer
    pBuffer = (uint8_t *)OCCalloc(1, 1);
    EXPECT_TRUE(pBuffer);
    OCFree(pBuffer);
}

TEST(OCCalloc, CallocPass2)
{
    // Try to allocate a small buffer
    pBuffer = (uint8_t *)OCCalloc(1, 128);
    EXPECT_TRUE(pBuffer);
    OCFree(pBuffer);
}

TEST(OCCalloc, CallocPass3)
{
    // Try to allocate a buffer for an array
    pBuffer = (uint8_t *)OCCalloc(5, 128);
    EXPECT_TRUE(pBuffer);
    OCFree(pBuffer);
}

TEST(OCCalloc, CallocFail1)
{
    // Try to allocate a buffer of size 0
    pBuffer = (uint8_t *)OCCalloc(1, 0);
    EXPECT_TRUE(NULL == pBuffer);
    OCFree(pBuffer);
}

TEST(OCCalloc, CallocFail2)
{
    // Try to allocate a buffer with num of 0
    pBuffer = (uint8_t *)OCCalloc(0, 5);
    EXPECT_TRUE(NULL == pBuffer);
    OCFree(pBuffer);
}

TEST(OCCalloc, CallocFail3)
{
    // Try to allocate a buffer with size and num 0
    pBuffer = (uint8_t *)OCCalloc(0, 0);
    EXPECT_TRUE(NULL == pBuffer);
    OCFree(pBuffer);
}

TEST(OCCalloc, CallocFail4)
{
    // Try to allocate a ridiculous amount of RAM
    pBuffer = (uint8_t *)OCCalloc(1, (size_t)0x7FFFFFFFFFFFFFFF);
    EXPECT_TRUE(NULL == pBuffer);
    OCFree(pBuffer);
}
