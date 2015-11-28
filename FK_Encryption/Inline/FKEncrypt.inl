/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Inline\FKEncrypt.inl
// Date:     2007年11月30日
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/

#pragma once
#include "../Include/md5.h"
#include <string>
#include <windows.h>

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//根据密钥生成四字节换位索引值
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
void FKGetEncryptFourByteIndex( unsigned int pByteIndex[4] )
{
	pByteIndex[0] = ( Key1 & 0xFF ) % 4;

	pByteIndex[1] = ( ( Key1 >> 7 ) & 0x7FF ) % 4;
	while( pByteIndex[1] == pByteIndex[0] )
		pByteIndex[1] = ( pByteIndex[1] + 1 ) % 4;

	pByteIndex[2] = ( ( Key1 >> 18 ) & 0x3FF ) % 4;
	while( pByteIndex[2] == pByteIndex[0] || pByteIndex[2] == pByteIndex[1] )
		pByteIndex[2] = ( pByteIndex[2] + 1 ) % 4;

	pByteIndex[3] = 0;
	while( pByteIndex[3] == pByteIndex[0] || pByteIndex[3] == pByteIndex[1] || pByteIndex[3] == pByteIndex[2] )
		pByteIndex[3] = ( pByteIndex[3] + 1 ) % 4;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//获取加密后的数据长度
__inline unsigned int FKGetEncryptedSize( const unsigned int nInLen )
{
	//计算循环处理次数
	const unsigned int nRound = ( nInLen / 4 ) + ( ( ( nInLen % 4 ) != 0 ) ? 1 : 0 );

	//计算每 4 字节补充 1 字节后的长度
	const unsigned int nAfterPadLen = nRound * ( 4 + 1 );

	//计算 4 字节对齐后的长度并附加原始长度记录值占用的长度
	const unsigned int nEncryptedLen = ( ( nAfterPadLen / 4 ) + ( ( ( nAfterPadLen % 4 ) != 0 ) ? 1 : 0 ) ) * 4 + sizeof(unsigned int);

	return nEncryptedLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//使用指定的密钥加密数据并返回加密后的数据长度
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
unsigned int FKEncrypt( void* pOut, const unsigned int nOutLen, const void* pIn, const unsigned int nInLen )
{
	const unsigned int nEncryptedLen = FKGetEncryptedSize( nInLen );
	if( nOutLen < nEncryptedLen )
		return 0;

	const unsigned char* pInPtr = (const unsigned char*)pIn;

	*(unsigned int*)pOut = nInLen;
	unsigned char* pOutPtr = (unsigned char*)pOut + sizeof(unsigned int);

	//根据密钥计算随机种子
	unsigned int pMixKey[4];
	pMixKey[0] = Key1;
	pMixKey[1] = Key2;
	pMixKey[2] = Key3;
	pMixKey[3] = ( Key1 & 0x3FF ) | ( Key2 & ( 0x3F << 10 ) ) | ( Key3 & ( 0xFFFF << 16 ) );

	RawRandomGenerator sRawRandomGenerator;
	sRawRandomGenerator.ResetSeed();
	sRawRandomGenerator.MixSeed( (unsigned char*)&pMixKey, 16 );

	//根据密钥生成四字节换位索引值
	unsigned int pByteIndex[4];
	FKGetEncryptFourByteIndex< Key1, Key2, Key3 >( pByteIndex );

	//进行每 4 字节输入数据增加 1 字节随机值输出的循环处理
	unsigned int nOutByte = 0;
	for( unsigned int nInByte=0; nInByte<nInLen; nInByte+=4, nOutByte+=5 )
	{
		//填充 1 字节的随机数据
		sRawRandomGenerator.GenerateBytes( &pOutPtr[nOutByte], 1 );

		//如果到达输入数据结尾则补零
		unsigned char pInBytes[4] = { 0 };
		for( unsigned int i=0; i<4; ++i )
		{
			if( nInByte + i < nInLen )
			{
				pInBytes[i] = pInPtr[ nInByte + i ];
			}
			else
			{
				break;
			}
		}

		//进行逐字节换位
		pOutPtr[nOutByte+1] = pInBytes[ pByteIndex[0] ];
		pOutPtr[nOutByte+2] = pInBytes[ pByteIndex[1] ];
		pOutPtr[nOutByte+3] = pInBytes[ pByteIndex[2] ];
		pOutPtr[nOutByte+4] = pInBytes[ pByteIndex[3] ];

		//根据密钥进行循环位移
		unsigned int* pBlock = (unsigned int*)&pOutPtr[nOutByte+1];

		const unsigned char nKey3 = ( Key3 & 0xFF );

		__asm
		{
			mov ebx, pBlock;
			mov eax, [ebx];
			mov cl, nKey3;
			ror eax, cl;
			mov [ebx], eax;
		}
	}

	const unsigned int nRemLen = nEncryptedLen - sizeof(unsigned int);

	//如果输出数据未 4 字节对齐则补零
	for(; nOutByte<nRemLen; ++nOutByte )
	{
		pOutPtr[ nOutByte ] = 0;
	}

	//进行每 4 字节的循环处理
	for( unsigned int nByte=0; nByte<nRemLen; nByte+=4 )
	{
		//根据密钥进行循环位移
		const unsigned char nKey2 = ( Key2 & 0xFF );

		__asm
		{
			mov ebx, pOutPtr;
			mov eax, [ebx];
			mov cl, nKey2;
			rol eax, cl;
			mov [ebx], eax;
		}

		//根据密钥进行部分位位反
		const unsigned int nOriginalBlock = *(unsigned int*)pOutPtr;
		*(unsigned int*)pOutPtr = ( nOriginalBlock & ( ~Key2 ) ) | ( ~nOriginalBlock & Key2 );

		pOutPtr += 4;
	}

	return nEncryptedLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//使用指定的密钥解密数据并返回解密后的数据长度
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
unsigned int FKDecrypt( void* pInOut, const unsigned int nInLen )
{
	//获取原数据长度
	const unsigned int nOutLen = *(unsigned int*)pInOut;

	const unsigned char* pInPtr = (const unsigned char*)pInOut + sizeof(unsigned int);
	unsigned char* pOutPtr = (unsigned char*)pInOut;

	//进行每 4 字节的循环处理
	const unsigned int nRemLen = nInLen - sizeof(unsigned int);
	for( unsigned int nByte=0; nByte<nRemLen; nByte+=4 )
	{
		//根据密钥进行部分位位反
		unsigned int nBlock = *(unsigned int*)&pInPtr[ nByte ];
		nBlock = ( nBlock & ( ~Key2 ) ) | ( ~nBlock & Key2 );

		//根据密钥进行循环位移
		const unsigned char nKey2 = ( Key2 & 0xFF );

		__asm
		{
			mov eax, nBlock;
			mov cl, nKey2;
			ror eax, cl;
			lea ebx, nBlock;
			mov [ebx], eax;
		}

		*(unsigned int*)&pOutPtr[ nByte ] = nBlock;
	}

	//根据密钥生成四字节换位索引值
	unsigned int pByteIndex[4];
	FKGetEncryptFourByteIndex< Key1, Key2, Key3 >( pByteIndex );

	//进行每 5 字节输入数据循环处理
	unsigned int nInByte = 0;
	for( unsigned int nOutByte=0; nOutByte<nOutLen; nOutByte+=4, nInByte+=5 )
	{
		//抛弃 1 字节的随机值
		unsigned int nBlock = *(unsigned int*)&pOutPtr[ nInByte + 1 ];

		//根据密钥进行循环位移
		const unsigned char nKey3 = ( Key3 & 0xFF );

		__asm
		{
			mov eax, nBlock;
			mov cl, nKey3;
			rol eax, cl;
			lea ebx, nBlock;
			mov [ebx], eax;
		}

		//进行逐字节换位
		unsigned char* pBytes = (unsigned char*)&nBlock;

		pOutPtr[ nOutByte + pByteIndex[0] ] = pBytes[0];
		pOutPtr[ nOutByte + pByteIndex[1] ] = pBytes[1];
		pOutPtr[ nOutByte + pByteIndex[2] ] = pBytes[2];
		pOutPtr[ nOutByte + pByteIndex[3] ] = pBytes[3];
	}

	return nOutLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 将 unsigned char 类型转换为 ASC2 字符
static std::string ByteToHEX( unsigned char p_ucIn )
{
	static const char szDigitStr [16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char* szOutBuffer = new char[2];
	szOutBuffer[0] = szDigitStr[(p_ucIn >> 4) & 0x0F];	// 取高4位
	szOutBuffer[1] = szDigitStr[ p_ucIn & 0x0F ];		// 取低4位
	std::string szOut( szOutBuffer );
	return szOut;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 是否是UTF8字符集
static bool IsUTF8( const char* p_szInfo )
{
	int nSize = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, p_szInfo, -1, NULL, 0 );
	int nError = GetLastError();
	if ( nError == ERROR_NO_UNICODE_TRANSLATION )
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 是否是GB2312字符集
static bool IsUnicode( const char* p_szInfo )
{
	int nSize = MultiByteToWideChar( 936, MB_ERR_INVALID_CHARS, p_szInfo, -1, NULL, 0 );
	int nError = GetLastError();
	if ( nError == ERROR_NO_UNICODE_TRANSLATION )
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 是否是GB
static int IsGB( char* p_szInfo )
{
	unsigned char* pChar = ( unsigned char* )p_szInfo;
	if ( pChar[0] >= 0xa1 )
	{
		if ( pChar[0] == 0xa3 )
		{
			return 1;	// 全角字符
		}
		else
		{
			return 2;	// 汉字
		}
	}
	else
	{
		return 0;		// 英文，数字，英文标点
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 是否有中文字符
static bool IsHaveChinese( const char* p_szInfo )
{
	int i = 0;
	char* pInfo = ( char* )p_szInfo;
	while( *pInfo != '\0' )
	{
		i = IsGB( pInfo );
		switch( i )
		{
		case 0:
			{
				pInfo++;
			}
			break;
		case 1:
			{
				pInfo++;
				pInfo++;
			}
			break;
		case 2:
			{
				pInfo++;
				pInfo++;
				return true;
			}
			break;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Unicode 转换为 UTF8
void UnicodeToUtf8(char *utf8) 
{
	int len = 0;
	int size_d = 8;
	DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, utf8, -1, NULL, 0);
	wchar_t *wchar;
	wchar = new wchar_t[dwNum];
	if(!wchar)
	{
		delete []wchar;
	}
	MultiByteToWideChar (CP_ACP, 0,utf8, -1, wchar, dwNum);
	for(int i = 0; i <dwNum; i++)
	{
		if ((wchar[i]) < 0x80)
		{  
			utf8[len++] = (char)(wchar[i]);
		}
		else if((wchar[i]) < 0x800)
		{
			utf8[len++] = 0xc0 | ( (*wchar) >> 6 );
			utf8[len++] = 0x80 | ( (*wchar) & 0x3f );
		}
		else if((wchar[i]) < 0x10000 )
		{
			utf8[len++] = 0xE0 | ((wchar[i]) >> 12);
			utf8[len++] = 0x80 | (((wchar[i])>>6) & 0x3F);
			utf8[len++] = 0x80 | ((wchar[i]) & 0x3F);  
		}
		else if((wchar[i]) < 0x200000 ) 
		{
			utf8[len++] = 0xf0 | ( (int)(wchar[i]) >> 18 );
			utf8[len++] = 0x80 | ( ((wchar[i]) >> 12) & 0x3f );
			utf8[len++] = 0x80 | ( ((wchar[i]) >> 6) & 0x3f );
			utf8[len++] = 0x80 | ( (wchar[i]) & 0x3f );
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _WINDOWS_
	#if _MSC_VER <= 1310
		// GBK 转换为 UTF8
		static char* Utf8ToGBK(const char* strUtf8)
		{
			int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0); 
			unsigned short * wszGBK = new unsigned short[len+1]; 
			memset(wszGBK, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, wszGBK, len); 
			len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL); 
			char *szGBK=new char[len + 1]; 
			memset(szGBK, 0, len + 1); 
			WideCharToMultiByte (CP_ACP, 0, wszGBK, -1, szGBK, len, NULL,NULL); 
			return szGBK; 
		}
		// UTF8 转换到 GBK
		static char* GBKToUtf8(const char* strGBK)
		{ 
			int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL,0); 
			unsigned short * wszUtf8 = new unsigned short[len+1]; 
			memset(wszUtf8, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, wszUtf8, len); 
			len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL); 
			char *szUtf8=new char[len + 1]; 
			memset(szUtf8, 0, len + 1); 
			WideCharToMultiByte (CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL,NULL); 
			return szUtf8; 
		} 

	#else
		// GBK 转换为 UTF8
		static char* Utf8ToGBK(const char* strUtf8)
		{
			int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0); 
			unsigned short * wszGBK = new unsigned short[len+1];       
			memset(wszGBK, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, (LPWSTR)wszGBK, len); 
			len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL); 
			char *szGBK=new char[len + 1]; 
			memset(szGBK, 0, len + 1); 
			WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL); 
			return szGBK; 
		}
		// UTF8 转换到 GBK
		static char* GBKToUtf8(const char* strGBK)
		{ 
			int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL,0); 
			unsigned short * wszUtf8 = new unsigned short[len+1]; 
			memset(wszUtf8, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, (LPWSTR)wszUtf8, len); 
			len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL); 
			char *szUtf8=new char[len + 1]; 
			memset(szUtf8, 0, len + 1); 
			WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, (LPSTR)szUtf8, len, NULL,NULL); 
			return szUtf8; 
		}
	#endif // _MSC_VER <= 1310
#else
	static char* Utf8ToGBK(const char* strUtf8)
	{
		#pragma message( "非Windows系统下请使用 UnicodeToUtf8 函数.")
		#error 非Windows系统下请使用 UnicodeToUtf8 函数.
		return NULL;
	}
	static char* GBKToUtf8(const char* strGBK)
	{ 
		#pragma message( "非Windows系统下请使用 UnicodeToUtf8 函数.")
		#error 非Windows系统下请使用 UnicodeToUtf8 函数.
		return NULL;
	}
#endif // _WINDOWS_
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 进行MD5加密
std::string FK_MD5Encrypt( char* pIn )
{
	char* pTemp = NULL;
	if( IsHaveChinese( pIn ) )
	{
		cout << "有中文" << endl;
		pTemp = GBKToUtf8( pIn );
	}
	else
	{
		pTemp = pIn;
	}

	unsigned char szOutUnit[16];
	unsigned int unInLen = strlen( pTemp );
	MD5_CTX sMD5Contex;
	MD5Init( &sMD5Contex );
	MD5Update( &sMD5Contex, (unsigned char*)pTemp, unInLen );
	MD5Final( szOutUnit, &sMD5Contex );

	char p_szOut[33] = { 0 };

	for( unsigned int i=0; i<16; i++ )
	{	
		sprintf( &( p_szOut[2*i] ), "%02x", szOutUnit[i] );
	}
	return p_szOut;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------