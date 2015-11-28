/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Include\FKEncrypt.h
// Date:     2007年11月29日
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/
#pragma once

#include "RawRandomGenerator.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 获取DES加密后的数据长度
__inline unsigned int FKGetEncryptedSize( const unsigned int nInLen );
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 使用指定的密钥加密数据并返回DES加密后的数据长度
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
unsigned int FKEncrypt( void* pOut, const unsigned int nOutLen, const void* pIn, const unsigned int nInLen );
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 使用指定的密钥解密数据并返回DES解密后的数据长度
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
unsigned int FKDecrypt( void* pInOut, const unsigned int nInLen );
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 进行MD5加密（加密后长度永远为 32 字节）
std::string FK_MD5Encrypt( char* pIn );
//-------------------------------------------------------------------------------------------------


#include "../Inline/FKEncrypt.inl"
