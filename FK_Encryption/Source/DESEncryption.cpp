

// Author : FreeKnightDuzhi


#include <memory.h>
#include <stdlib.h>

#include "../Include/DESEncryption.h"


typedef void (*DES_FnKey)( unsigned char*, short );
typedef void (*DES_FnProcess)( unsigned char*, unsigned char* );

// DES库的加密Key表
static const DES_FnKey DES_pFnKey[3] =
{
	deskey,
	des2key,
	des3key,
};
// DES库的加密功能函数表
static const DES_FnProcess DES_pFnProcess[3] =
{
	Ddes,
	D2des,
	D3des,
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// DES数据加工
void DES_Process( DES_Layer eLayer, void* pData, const size_t nLen )
{
	unsigned char* pDataPtr = (unsigned char*)pData;

	const size_t nRound = nLen / DES_pBlockLength[ eLayer ];

	for( size_t i=0; i<nRound; ++i )
	{
		DES_pFnProcess[ eLayer ]( pDataPtr, pDataPtr );
		pDataPtr += DES_pBlockLength[ eLayer ];
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 预计算加密一段数据所需空间大小
size_t DES_GetEncryptedSize( DES_Layer eLayer, const size_t nLen )
{
	const size_t nFinalLen = nLen + sizeof(size_t);

	size_t nRound = nFinalLen / DES_pBlockLength[ eLayer ];
	const size_t nPad = nFinalLen % DES_pBlockLength[ eLayer ];

	if( nPad > 0 )
	{
		++nRound;
	}

	return nRound * DES_pBlockLength[ eLayer ];
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 加密一段数据
/*
param : eLayer	加密方式
pKey	Key
OUT pOutData	输出密文
OUT	nOutLen		输出密文长度
pInData			需要加密的明文
nInLen			明文长度
return : 输出的密文长度

//-------------------------------------------------------------------------------
Warning!!! 密文必定与明文相同大小，甚至大于明文，不可给予一个小于明文的加密空间。
//-------------------------------------------------------------------------------
*/		
size_t DES_Encrypt( DES_Layer eLayer, const unsigned char* pKey, void* pOutData, const size_t nOutLen, const void* pInData, const size_t nInLen )
{
	const size_t nFinalLen = nInLen + sizeof(size_t);

	size_t nRound = nFinalLen / DES_pBlockLength[ eLayer ];
	const size_t nPad = nFinalLen % DES_pBlockLength[ eLayer ];

	if( nPad > 0 )
	{
		++nRound;
	}

	const size_t nRoundUpSize = nRound * DES_pBlockLength[ eLayer ];;

	if( nOutLen < nRoundUpSize )
		return 0;

	DES_pFnKey[ eLayer ]( (unsigned char*)pKey, EN0 );

	*( (size_t*)pOutData ) = nInLen;

	memcpy( (size_t*)pOutData + 1, pInData, nInLen );

	memset( (char*)pOutData + nFinalLen, 0, nPad );

	DES_Process( eLayer, pOutData, nOutLen );

	return nOutLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 解密一段数据
/*
param : eLayer	解密方式
pKey	Key
pData	需要解密的文件 / 同时，解密后的明文也会 OUT 出来
nInLen	需要解密的文件长度
return : 解密后文件长度
*/
size_t DES_Decrypt( DES_Layer eLayer, const unsigned char* pKey, void* pData, const size_t nInLen )
{
	if( ( nInLen % DES_pBlockLength[ eLayer ] ) != 0 )
		return 0;

	DES_pFnKey[ eLayer ]( (unsigned char*)pKey, DE1 );

	DES_Process( eLayer, pData, nInLen );

	const size_t nOutLen = *( (size_t*)pData );
	if( nOutLen > nInLen )
		return 0;

	memcpy( pData, (size_t*)pData + 1, nOutLen );

	return nOutLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
