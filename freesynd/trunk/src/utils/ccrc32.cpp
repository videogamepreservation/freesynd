///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright � NetworkDLS 2002, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//
// Modified by Bohdan Stelmakh for use in Freesynd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CCRC32_CPP
#define _CCRC32_CPP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ccrc32.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
\tThis function initializes "CRC Lookup Table". You only need to call it once to
\t\tinitalize the table before using any of the other CRC32 calculation functions.
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCRC32::CCRC32(void)
{
    this->Initialize();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCRC32::~CCRC32(void)
{
    //No destructor code.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
\tThis function initializes "CRC Lookup Table". You only need to call it once to
\t\tinitalize the table before using any of the other CRC32 calculation functions.
*/

void CCRC32::Initialize(void)
{
\t//0x04C11DB7 is the official polynomial used by PKZip, WinZip and Ethernet.
\tunsigned int ulPolynomial = 0x04C11DB7;

\tmemset(&this->ulTable, 0, sizeof(this->ulTable));

\t// 256 values representing ASCII character codes.
\tfor(int iCodes = 0; iCodes <= 0xFF; iCodes++)
\t{
\t\tthis->ulTable[iCodes] = this->Reflect(iCodes, 8) << 24;

\t\tfor(int iPos = 0; iPos < 8; iPos++)
\t\t{
\t\t\tthis->ulTable[iCodes] = (this->ulTable[iCodes] << 1)
\t\t\t\t^ ((this->ulTable[iCodes] & (1 << 31)) ? ulPolynomial : 0);
\t\t}

\t\tthis->ulTable[iCodes] = this->Reflect(this->ulTable[iCodes], 32);
\t}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
\tReflection is a requirement for the official CRC-32 standard.
\tYou can create CRCs without it, but they won't conform to the standard.
*/

unsigned int CCRC32::Reflect(unsigned int ulReflect, const char cChar)
{
\tunsigned int ulValue = 0;

\t// Swap bit 0 for bit 7, bit 1 For bit 6, etc....
\tfor(int iPos = 1; iPos < (cChar + 1); iPos++)
\t{
\t\tif(ulReflect & 1)
\t\t{
\t\t\tulValue |= (1 << (cChar - iPos));
\t\t}
\t\tulReflect >>= 1;
\t}

\treturn ulValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
\tCalculates the CRC32 by looping through each of the bytes in sData.
\t
\tNote: For Example usage example, see FileCRC().
*/

void CCRC32::PartialCRC(unsigned int *ulCRC, const unsigned char *sData, size_t ulDataLength)
{
\twhile(ulDataLength--)
\t{
\t\t//If your compiler complains about the following line, try changing
\t\t//\teach occurrence of *ulCRC with ((unsigned int)*ulCRC).

\t\t*ulCRC = (*ulCRC >> 8) ^ this->ulTable[(*ulCRC & 0xFF) ^ *sData++];
\t}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
\tReturns the calculated CRC32 (through ulOutCRC) for the given string.
*/

void CCRC32::FullCRC(const unsigned char *sData, size_t ulDataLength, unsigned int *ulOutCRC)
{
    *((unsigned int *)ulOutCRC) = 0xffffffff; //Initilaize the CRC.

\tthis->PartialCRC(ulOutCRC, sData, ulDataLength);

\t*((unsigned int *)ulOutCRC) ^= 0xffffffff; //Finalize the CRC.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
\tReturns the calculated CRC23 for the given string.
*/

unsigned int CCRC32::FullCRC(const unsigned char *sData, size_t ulDataLength)
{
    unsigned int ulCRC = 0xffffffff; //Initilaize the CRC.

\tthis->PartialCRC(&ulCRC, sData, ulDataLength);

\treturn(ulCRC ^ 0xffffffff); //Finalize the CRC and return.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
\tCalculates the CRC32 of a file using the a user defined buffer.

\tNote: The buffer size DOES NOT affect the resulting CRC,
\t\t\tit has been provided for performance purposes only.
*/

bool CCRC32::FileCRC(const char *sFileName, unsigned int *ulOutCRC, size_t ulBufferSize)
{
    *((unsigned int *)ulOutCRC) = 0xffffffff; //Initilaize the CRC.

\tFILE *fSource = NULL;
\tunsigned char *sBuf = NULL;
\tsize_t iBytesRead = 0;

\tif((fSource = fopen(sFileName, "rb")) == NULL)
\t{
\t\treturn false; //Failed to open file for read access.
\t}

\tif(!(sBuf = (unsigned char *)malloc(ulBufferSize))) //Allocate memory for file buffering.
\t{
\t\tfclose(fSource);
\t\treturn false; //Out of memory.
\t}

\twhile((iBytesRead = fread(sBuf, sizeof(char), ulBufferSize, fSource)))
\t{
\t\tthis->PartialCRC(ulOutCRC, sBuf, iBytesRead);
\t}

\tfree(sBuf);
\tfclose(fSource);

\t*((unsigned int *)ulOutCRC) ^= 0xffffffff; //Finalize the CRC.

\treturn true;
}

#endif
