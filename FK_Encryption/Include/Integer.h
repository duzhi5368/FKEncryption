/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Include\Integer.h
// Date:     2007年11月21日
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/

#pragma once

#include <vector>


typedef unsigned short		IntHUnit;
typedef unsigned int		IntUnit;
typedef unsigned __int64	IntDUnit;

const unsigned int	IntUnitSize		= sizeof(IntUnit);							// 一个大整型单元( 无符号整形 )对象长度
const unsigned int	IntUnitBits		= IntUnitSize * 8;							// 一个大整型单元( 无符号整形 )对象字节数
const unsigned int	IntUnitMask		= 0xFFFFFFFF;								// 一个大整型单元( 无符号整形 )对象字节掩码
const unsigned int	IntUnits32		= 4 / sizeof(IntUnit);						// 一个大整型单元( 无符号整形 )对象在 32 位机上标准单元个数
const unsigned int	IntUnitsMin		= ( IntUnits32 > 2 ) ? IntUnits32 : 2;		// 一个大整型单元( 无符号整形 )对象最小标准单元个数

/*
	大整型类
	-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	说明：
	1：一个大整型由一组大整型单元以及一个符号标志符组成。
	2：一个大整型的单元向量表中，越靠后的元素，位越高。
	   例如 vector[0] = 0x00000010 vector[1] = 0x11111101 那么这个大整型的值为 0x1111110100000010
	-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
class Integer
{
public:
	// 正负标志
	enum Sign
	{
		Positive,
		Negative,
	};

public:
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 构造/析构函数
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Integer();
	Integer( const Integer& rOther );
	Integer( const unsigned int nValue );
	Integer( const int nValue );
	~Integer();
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 重载操作符
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const Integer& operator = ( const Integer& rOther );
	const Integer& operator = ( const unsigned int nValue );
	const Integer& operator = ( const int nValue );

	bool operator ! () const;

	bool operator == ( const Integer& rOther );
	bool operator != ( const Integer& rOther );

	const Integer operator + ( const Integer& rOther ) const;
	void operator += ( const Integer& rOther );

	const Integer operator - ( const Integer& rOther ) const;
	void operator -= ( const Integer& rOther );

	const Integer operator * ( const Integer& rOther ) const;
	void operator *= ( const Integer& rOther );

	const Integer operator / ( const Integer& rOther ) const;
	void operator /= ( const Integer& rOther );

	const Integer operator % ( const Integer& rOther ) const;
	void operator %= ( const Integer& rOther );

	const Integer& operator ++ ();
	const Integer operator ++ ( int );
	const Integer& operator -- ();
	const Integer operator -- ( int );

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 常用功能函数
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 将一个大整数转换为 unsigned int 类型
	// return : 若转换失败依旧会返回 0
	operator unsigned int () const;
	// 将一个大整数转换为 int 类型
	// return : 若转换失败依旧会返回 0
	operator int () const;
	// 除法运算（被除数即为本大整数）
	/*
		param: OUT Remainder 余数
		param: OUT Quotient  商
		param: IN  Divisor	 除数，分母
	*/
	void Divide( Integer& Remainder, Integer& Quotient, const Integer& Divisor );
	// 判断一个大整数能否转换为 unsigned int 类型
	bool CanConvertToUInt() const;
	// 判断一个大整数能否转换为 int 类型
	bool CanConvertToInt() const;
	// 比较两个大整数（必须两个均为正数）
	int PositiveCompare( const Integer& rOther ) const;
	// 比较两个大整型
	/*
		return : 1 本大整数大于目标大整数
				 0 两个大整数完全相等
				-1 本大整数小目标大整数
	*/
	int Compare( const Integer& rOther ) const;
	// 将一个大整数设为 0
	void SetZero();
	// 判断一个大整数是否为 0
	bool IsZero() const { return !*this; }
	// 判断一个大整数是否不为 0
	bool NotZero() const { return !IsZero(); }
	// 对一个大整数取负
	void Negate();
	// 判断一个大整数是否是负数
	bool IsNegative() const { return ( mSign == Negative ); }
	// 判断一个大整数是否不是负数
	bool NotNegative() const { return ( mSign != Negative ); }
	// 判断一个大整数是否是正数
	bool IsPositive() const { return ( mSign == Positive ); }
	// 判断一个大整数是否不是正数
	bool NotPositive() const { return ( mSign != Positive ); }
	// 取一个大整数的绝对值
	Integer AbsoluteValue() const;

public:
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 大整数单元操作函数
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 扩展/或缩减 一个大整型单元个数
	void _SetUnits( const unsigned int nCount );
	// 扩展一个大整型单元个数，保持其值不变。（该函数不会缩减大整型单元个数）
	void _SetUnitsZeroPadding( const unsigned int nCount );
	// 扩展/或缩减 一个大整型单元个数，并设置大整型各单元为 0
	void _SetUnitsZero( const unsigned int nCount );
	// 获取大整型单元个数
	unsigned int _GetUnits() const;
	// 获取第一个非 0 的大整型单元编号
	unsigned int _GetUnitsNotZero() const;

public:
	Sign							mSign;					// 大整数符号
	std::vector<IntUnit>		mArrayUnits;				// 大整数的整数单元
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 比较两个无符号整型，通常用途是比较两个大整型长度
__inline const unsigned int Max( const unsigned int A, const unsigned int B )
{
	return ( A > B ) ? A : B;
}
// 复制一个大整型单元
__inline void CopyUnits( IntUnit* pA, const IntUnit* pB, unsigned int nUnits )
{
	memcpy( pA, pB, nUnits * sizeof(IntUnit) );
}
// 填充一个大整型单元
__inline void SetUnits( IntUnit* pData, IntUnit nValue, unsigned int nUnits )
{
	for( unsigned int i=0; i<nUnits; i++ )
		pData[i] = nValue;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 双个大整数的实际计算方法
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void _Add( Integer& Sum, const Integer& A, const Integer& B );
extern void _Subtract( Integer& Diff, const Integer& A, const Integer& B );
extern void _Mutiply( Integer& Product, const Integer& A, const Integer& B );
// 两个大整型除法运算
/*
	param: OUT Remainder 余数
	param: OUT Quotient  商
	param: IN  Dividend  被除数，分子
	param: IN  Divisor	 除数，分母
*/
extern void _Divide( Integer& Remainder, Integer& Quotient, const Integer& Dividend, const Integer& Divisor );
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 双个正大整数的计算方法（不应被调用）
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void _PositiveAdd( Integer& Sum, const Integer& A, const Integer& B );
extern void _PositiveSubtract( Integer& Diff, const Integer& A, const Integer& B );
extern void _PositiveMutiply( Integer& Product, const Integer& A, const Integer& B );
extern void _PositiveDivide( Integer& Remainder, Integer& Quotient, const Integer& A, const Integer& B );
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 双个正大整数的计算方法（不应被调用）
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 两个大整数单元向量的乘法（两个向量可能不等长）
/*
		param: OUT pResult	计算结果
					pTemp	临时的计算空间
					pA		源单元向量A
					nUnitsA 源单元向量A长度
					pB		源单元向量B
					nUnitsB 源单元向量B长度
*/
extern void _AsymmetricMultiply( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnitsA, const IntUnit* pB, unsigned int nUnitsB );
extern IntUnit _LinearMultiply( IntUnit* pC, const IntUnit* pA, IntUnit B, unsigned int N );
// 两个大整数单元向量的递归乘法（两个向量等长）
extern void _RecursiveMultiply( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, const IntUnit* pB, unsigned int nUnits );
// 平方
extern void _RecursiveSquare( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnits );

extern void _Divide( IntUnit* pRem, IntUnit* pQuot, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnitsA, const IntUnit* pB, unsigned int nUnitsB );
extern IntUnit _DivideThreeUnitsByTwo( IntUnit* A, IntUnit B0, IntUnit B1 );
// 4个单元除以2个单元的大整数，结果返回2个单元的大整数
extern unsigned int _DivideFourUnitsByTwo( IntUnit* T, const unsigned int& Al, const unsigned int& Ah, const unsigned int& B );
// 求两个大整数商
extern void _AtomicDivide( IntUnit* pQ, const IntUnit* pA, const IntUnit* pB );
// 纠正商误差
extern void _CorrectQuotientEstimate( IntUnit* pRem, IntUnit* pTemp, IntUnit* pQuot, const IntUnit* pB, unsigned int nUnits );

// 标准加法（以一个 unsigned __int64 为基本单位的运算）
extern int _DefaultAdd( IntUnit* pC, const IntUnit* pA, const IntUnit* pB, const unsigned int nUnits );
// 标准减法（以一个 unsigned __int64 为基本单位的运算）
extern int _DefaultSubtract( IntUnit* pC, const IntUnit* pA, const IntUnit* pB, const unsigned int nUnits );

extern void _DefaultMultiply2( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );
extern void _DefaultMultiply4( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );
extern void _DefaultMultiply8( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );
extern void _DefaultMultiply16( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );

extern void _DefaultSquare2( IntUnit* pResult, const IntUnit* pA );
extern void _DefaultSquare4( IntUnit* pResult, const IntUnit* pA );
extern void _DefaultSquare8( IntUnit* pResult, const IntUnit* pA );
extern void _DefaultSquare16( IntUnit* pResult, const IntUnit* pA );
