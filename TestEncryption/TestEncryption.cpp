// TestEncryption.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "../FK_Encryption/Include/RawRandomGenerator.h"
#include "../FK_Encryption/Include/FKEncrypt.h"

#if defined( _DEBUG ) || defined( DEBUG )
	#pragma comment( lib, "../FK_Encryption/Lib/FK_Encryption_D.lib")
#else
	#pragma comment( lib, "../FK_Encryption/Lib/FK_Encryption.lib")
#endif

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char					g_DESKey[8];				// 当前使用 DES 密钥
vector<unsigned char>			g_vecEncryptBuffer;			// 消息加密缓存
vector<unsigned char>			g_vecDecryptBuffer;			// 消息解密缓存
unsigned char					g_DataBuffer[128];			// 明文数据		（暂时以128为标准大小，在游戏中为其他长度也可，可随时随意变化。）
unsigned int					g_EncryptedLen;				// 加密后的密文长度

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//客户端服务器端未验证时采用的固定加密算法密钥（该密钥将会被作为模板参数传入，自动处理为汇编立即数，请勿定义为常量。）
#define			FK_DEFAULT_ENCRYPT_KEY1		981345123
#define			FK_DEFAULT_ENCRYPT_KEY2		165252668
#define			FK_DEFAULT_ENCRYPT_KEY3		496212351
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// MD5验证 http://www.md5.com.cn/index.php


int _tmain(int argc, _TCHAR* argv[])
{
	// 生成DES密钥
	RawRandomGenerator sKeyGenerator;
	sKeyGenerator.GenerateBytesByTime( g_DESKey, 8 );

	cout << "====================================" << endl;
	cout << "生成的DES密钥为:" << endl;
	for( int i = 0; i < 8; ++i )
	{
		cout << i << " : "<< unsigned int( g_DESKey[i] ) << "  \t\t-----\t" << g_DESKey[i] << endl;
	}
	cout << "====================================" << endl;
	cout << "请输入一段字符串作为数据明文( 由于测试需求，请小于128字节 )" << endl;
	cin >> g_DataBuffer;

	// 计算密文长度
	unsigned char* pEncryptedBuffer = NULL;					// 加密后的密文
	unsigned int nDataSize			= sizeof(g_DataBuffer);	// 加密前的明文长度
	g_EncryptedLen = FKGetEncryptedSize( sizeof(g_DataBuffer) );
	g_vecEncryptBuffer.resize( g_EncryptedLen );
	pEncryptedBuffer = &g_vecEncryptBuffer[0];

	FKEncrypt< FK_DEFAULT_ENCRYPT_KEY1, FK_DEFAULT_ENCRYPT_KEY2, FK_DEFAULT_ENCRYPT_KEY3 >( pEncryptedBuffer, g_EncryptedLen, g_DataBuffer, sizeof(g_DataBuffer) );
	cout << "====================================" << endl;
	cout << "加密后密文长度为:  " << g_EncryptedLen << endl;
	for( int i = 0; i < g_EncryptedLen; ++i )
	{
		cout << i << " : "<< unsigned int( pEncryptedBuffer[i] ) << "  \t\t-----\t" << pEncryptedBuffer[i] << endl;
	}

	cout << "====================================" << endl;
	nDataSize = FKDecrypt< FK_DEFAULT_ENCRYPT_KEY1, FK_DEFAULT_ENCRYPT_KEY2, FK_DEFAULT_ENCRYPT_KEY3 >( pEncryptedBuffer, g_EncryptedLen );
	cout << "解密后明文长度为:  " << nDataSize << endl;
	for( int i = 0; i < nDataSize; ++i )
	{
		if ( unsigned int( pEncryptedBuffer[i] ) != 0 )
		{
			cout << i << " : "<< unsigned int( pEncryptedBuffer[i] ) << "  \t\t-----\t" << pEncryptedBuffer[i] << endl;
		}
	}
	cout << "====================================" << endl;
	cout << "完整明文为 : " << pEncryptedBuffer << endl;

	// MD5加密
	for ( ;; )
	{
		cout << "====================================================================================" << endl;
		char szSrcBuffer[512] = { 0 };
		cout << "请输入一段明文:( 若输入0则退出本程序 ) " << endl;
		cin >> szSrcBuffer;
		if ( szSrcBuffer == 0 )
		{
			break;
		}
		char szBuffer[33] = { 0 };
		memcpy( szBuffer, FK_MD5Encrypt( szSrcBuffer ).c_str(), 33 );

		cout << "====================================" << endl;
		cout << "MD加密后为 : " << endl;
		for( unsigned int i = 0; i < 32; ++i )
		{
			cout << szBuffer[i];
		}
		cout << endl;
		cout << "====================================" << endl;
	}

	return 0;
}

