#include "../Include/RawRandomGenerator.h"


#include <memory.h>
#include <time.h>
#include "../Include/md5.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
RawRandomGenerator::RawRandomGenerator()
{
	ResetSeed();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//重置随机种子
void RawRandomGenerator::ResetSeed()
{
	mnSeedMixBytesNeed = 16;
	memset( mpSeedState, 0, sizeof(mpSeedState) );

	mnOutputAvailable = 0;
	memset( mpOutput, 0, sizeof(mpOutput) );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//混合随机种子
unsigned int RawRandomGenerator::MixSeed( const unsigned char* pMixData, unsigned int nMixDataLen )
{
	MD5_CTX sMD5Contex;

	MD5Init( &sMD5Contex );
	MD5Update( &sMD5Contex, (unsigned char*)pMixData, nMixDataLen );

	unsigned char pDigest[16];
	MD5Final( pDigest, &sMD5Contex );

	for( unsigned int j=0, i=16; i>0; --i )
	{
		j += mpSeedState[i-1] + pDigest[i-1];
		mpSeedState[i-1] = (unsigned char)j;
		j >>= 8;
	}

	mnSeedMixBytesNeed = ( mnSeedMixBytesNeed < nMixDataLen ) ? 0 : ( mnSeedMixBytesNeed - nMixDataLen );

	memset( pDigest, 0, sizeof(pDigest) );

	return mnSeedMixBytesNeed;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//获取随机种子需要的混合数据字节数
unsigned int RawRandomGenerator::GetSeedMixBytesNeed() const
{
	return mnSeedMixBytesNeed;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//根据当前时间设置随机种子
void RawRandomGenerator::SetSeedByTime()
{
	MD5_CTX sMD5Contex;

	for( unsigned int i=0; i<16; ++i )
	{
		mpSeedState[i] ^= (char)clock();
		mpSeedState[15-i] ^= (char)time(NULL);
	}

	MD5Init( &sMD5Contex );
	MD5Update( &sMD5Contex, mpSeedState, 16 );
	MD5Final( mpOutput, &sMD5Contex );

	mnOutputAvailable = 16;
	mnSeedMixBytesNeed = 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//生成指定长度的随机数据
bool RawRandomGenerator::GenerateBytes( unsigned char* pOutData, unsigned int nOutLen )
{
	if( mnSeedMixBytesNeed > 0 )
		return false;

	MD5_CTX sMD5Contex;

	unsigned int nAvailableData = mnOutputAvailable;
	while( nAvailableData < nOutLen )
	{
		if( nAvailableData > 0 )
		{
			memcpy( pOutData, &mpOutput[ 16 - nAvailableData ], nAvailableData );
			nOutLen -= nAvailableData;
			pOutData += nAvailableData;
		}

		MD5Init( &sMD5Contex );
		MD5Update( &sMD5Contex, mpSeedState, 16 );
		MD5Final( mpOutput, &sMD5Contex );

		nAvailableData = 16;

		for( unsigned int i=16; i>0; --i )
		{
			if( mpSeedState[i-1]++ != 0 )
				break;
		}
	}

	memcpy( pOutData, &mpOutput[ 16 - nAvailableData ], nOutLen );
	mnOutputAvailable = nAvailableData - nOutLen;

	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//根据当前时间生成指定长度的随机数据
void RawRandomGenerator::GenerateBytesByTime( unsigned char* pOutData, unsigned int nOutLen )
{
	SetSeedByTime();
	GenerateBytes( pOutData, nOutLen );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
