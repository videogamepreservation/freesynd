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
#ifndef _CCRC32_H
#define _CCRC32_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCRC32{

\tpublic:
\t\tCCRC32(void);
\t\t~CCRC32(void);

\t\tvoid Initialize(void);

\t\tbool FileCRC(const char *sFileName, unsigned int *ulOutCRC, size_t ulBufferSize);

\t\tunsigned int FullCRC(const unsigned char *sData, size_t ulDataLength);
\t\tvoid FullCRC(const unsigned char *sData, size_t ulLength, unsigned int *ulOutCRC);

\t\tvoid PartialCRC(unsigned int *ulCRC, const unsigned char *sData, size_t ulDataLength);

\tprivate:
\t\tunsigned int Reflect(unsigned int ulReflect, const char cChar);
\t\tunsigned int ulTable[256]; // CRC lookup table array.
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
