/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Include\RawRandomGenerator.h
// Date:     2007年11月27日
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/


#pragma once


//随机数据生成器
class RawRandomGenerator
{
public:
	RawRandomGenerator();

	//重置随机种子
	void ResetSeed();

	//混合随机种子
	unsigned int MixSeed( const unsigned char* pMixData, unsigned int nMixDataLen );

	//获取随机种子需要的混合数据字节数
	unsigned int GetSeedMixBytesNeed() const;

	//根据当前时间设置随机种子
	void SetSeedByTime();

	//生成指定长度的随机数据
	bool GenerateBytes( unsigned char* pOutData, unsigned int nOutLen );

	//根据当前时间生成指定长度的随机数据
	void GenerateBytesByTime( unsigned char* pOutData, unsigned int nOutLen );

protected:
	unsigned int	mnSeedMixBytesNeed;		//混合种子需要字节数
	unsigned char	mpSeedState[16];		//混合后种子状态
	unsigned int	mnOutputAvailable;		//剩余可用输出随机数据字节数
	unsigned char	mpOutput[16];			//输出随机数据
};
