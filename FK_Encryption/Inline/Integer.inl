/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Inline\Integer.inl
// Date:     2007年11月21日
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/

#pragma once


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 由两个 unsigned int 组合为一个 unsigned __int64 类型
IntDUnit _MakeDoubleUnits( const IntUnit Low, const IntUnit High )
{
	return ( ( (IntDUnit)High << IntUnitBits ) | (unsigned int)Low );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 获取一个Unsigned __int64的低位，组合为一个 unsigned int 
IntUnit _GetLowUnit( const IntDUnit N )
{
	return (IntUnit)( N & IntUnitMask );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 获取一个Unsigned __int64的高位，组合为一个 unsigned int 
IntUnit _GetHighUnit( const IntDUnit N )
{
	return (IntUnit)( ( N >> IntUnitBits ) & IntUnitMask );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 获取一个Unsigned __int64的高位，取反后组合为一个 unsigned int 
IntUnit _GetHighUnitAsBorrow( const IntDUnit N )
{
	return 0 - _GetHighUnit( N );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
template < typename T > unsigned int _BitPrecision( const T& nValue )
{
	if( nValue == 0 )
		return 0;

	unsigned int l = 0;
	unsigned int h = 8 * sizeof(T);

	while( h - l > 1 )
	{
		unsigned int t = ( l + h ) / 2;
		if( nValue >> t )
			l = t;
		else
			h = t;
	}

	return h;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 左格式化大整数单元( 标准化大整数单元 )
inline IntUnit _ShiftUnitsLeftByBits( IntUnit* pResult, unsigned int n, unsigned int nShiftBits )
{
	IntUnit nCarry = 0;

	if( nShiftBits != 0 )
	{
		for( unsigned int i=0; i<n; ++i )
		{
			IntUnit u = pResult[i];
			pResult[i] = ( u << nShiftBits ) | nCarry;
			nCarry = u >> ( IntUnitBits - nShiftBits );
		}
	}

	return nCarry;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 右格式化大整数单元( 反标准化大整数单元 )
inline IntUnit _ShiftUnitsRightByBits( IntUnit* pResult, unsigned int n, unsigned int nShiftBits )
{
	IntUnit nCarry = 0;

	if( nShiftBits != 0 )
	{
		for( unsigned int i=n; i>0; --i )
		{
			IntUnit u = pResult[i-1];
			pResult[i-1] = ( u >> nShiftBits ) | nCarry;
			nCarry = u << ( IntUnitBits - nShiftBits );
		}
	}

	return nCarry;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 设置位数表（根据大整数单元个数设计分配长度）
const unsigned int RoundupSizeTable[] = { 2, 2, 2, 4, 4, 8, 8, 8, 8 };
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 标准化，获取位数大小（根据大整数单元个数设计分配长度）
inline unsigned int _RoundupSize( unsigned int n )
{
	if( n <= 8 )
		return RoundupSizeTable[n];
	else if( n <= 16 )
		return 16;
	else if( n <= 32 )
		return 32;
	else if( n <= 64 )
		return 64;
	else
		return 1 << _BitPrecision( n - 1 );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 比较两个整形向量大小（不考虑符号）
inline int _Compare( const IntUnit* pA, const IntUnit* pB, unsigned int nUnits )
{
	while( nUnits-- != 0 )
	{
		if( pA[nUnits] > pB[nUnits] )
			return 1;
		else if( pA[nUnits] < pB[nUnits] )
			return -1;
	}

	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline int _Increment( IntUnit* pA, unsigned int nUnits, IntUnit B = 1 )
{
	IntUnit T = pA[0];

	pA[0] = T + B;

	if( pA[0] >= T )
		return 0;

	for( unsigned int i=1; i<nUnits; ++i )
	{
		if( ++pA[i] != 0 )
			return 0;
	}

	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
inline int _Decrement( IntUnit *pA, unsigned int nUnits, IntUnit B = 1 )
{
	IntUnit T = pA[0];

	pA[0] = T - B;

	if( pA[0] <= T )
		return 0;

	for( unsigned int i=1; i<nUnits; ++i )
	{
		if( pA[i]-- != 0 )
			return 0;
	}

	return 1;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define Declare2Units(x)			IntDUnit x;
#define MultiplyUnits(p, a, b)		p = (IntDUnit)a*b;
#define AssignWord(a, b)			a = b;
#define Add2UnitsBy1(a, b, c)		a = b + c;
#define Acc2UnitsBy2(a, b)			a += b;
#define LowUnit(a)					IntUnit(a)
#define HighUnit(a)					IntUnit(a>>IntUnitBits)
#define Double3Units(c, d)			d = 2*d + (c>>(IntUnitBits-1)); c *= 2;
#define AddWithCarry(u, a, b)		u = IntDUnit(a) + b + GetCarry(u);
#define SubtractWithBorrow(u, a, b)	u = IntDUnit(a) - b - GetBorrow(u);
#define GetCarry(u)					HighUnit(u)
#define GetBorrow(u)				IntUnit(u>>(IntUnitBits*2-1))
#define MulAcc(c, d, a, b)			MultiplyUnits(p, a, b); Acc2UnitsBy1(p, c); c = LowUnit(p); Acc2UnitsBy1(d, HighUnit(p));
#define Acc2UnitsBy1(a, b)			Add2UnitsBy1(a, a, b)
#define Acc3UnitsBy2(c, d, e)		Acc2UnitsBy1(e, c); c = LowUnit(e); Add2UnitsBy1(e, d, HighUnit(e));

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define Mul_Begin(n)				\
	Declare2Units(p)				\
	IntUnit c;	\
	Declare2Units(d)				\
	MultiplyUnits(p, pA[0], pB[0])	\
	c = LowUnit(p);		\
	AssignWord(d, HighUnit(p))

#define Mul_Acc(i, j)				\
	MulAcc(c, d, pA[i], pB[j])

#define Mul_SaveAcc(k, i, j) 		\
	pResult[k] = c;				\
	c = LowUnit(d);	\
	AssignWord(d, HighUnit(d))	\
	MulAcc(c, d, pA[i], pB[j])

#define Mul_End(k, i)					\
	pResult[k] = c;			\
	MultiplyUnits(p, pA[i], pB[i])	\
	Acc2UnitsBy2(p, d)				\
	pResult[k+1] = LowUnit(p);			\
	pResult[k+2] = HighUnit(p);

#define Bot_SaveAcc(k, i, j)		\
	pResult[k] = c;				\
	c = LowUnit(d);	\
	c += A[i] * B[j];

#define Bot_Acc(i, j)	\
	c += A[i] * B[j];

#define Bot_End(n)		\
	pResult[n-1] = c;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define Squ_Begin(n)				\
	Declare2Units(p)				\
	IntUnit c;				\
	Declare2Units(d)				\
	Declare2Units(e)				\
	MultiplyUnits(p, pA[0], pA[0])	\
	pResult[0] = LowUnit(p);				\
	AssignWord(e, HighUnit(p))		\
	MultiplyUnits(p, pA[0], pA[1])	\
	c = LowUnit(p);		\
	AssignWord(d, HighUnit(p))		\
	Squ_NonDiag						\

#define Squ_NonDiag				\
	Double3Units(c, d)

#define Squ_SaveAcc(k, i, j) 		\
	Acc3UnitsBy2(c, d, e)			\
	pResult[k] = c;				\
	MultiplyUnits(p, pA[i], pA[j])	\
	c = LowUnit(p);		\
	AssignWord(d, HighUnit(p))		\

#define Squ_Acc(i, j)				\
	MulAcc(c, d, pA[i], pA[j])

#define Squ_Diag(i)					\
	Squ_NonDiag						\
	MulAcc(c, d, pA[i], pA[i])

#define Squ_End(n)					\
	Acc3UnitsBy2(c, d, e)			\
	pResult[2*n-3] = c;			\
	MultiplyUnits(p, pA[n-1], pA[n-1])\
	Acc2UnitsBy2(p, e)				\
	pResult[2*n-2] = LowUnit(p);			\
	pResult[2*n-1] = HighUnit(p);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define Mul_2 \
	Mul_Begin(2) \
	Mul_SaveAcc(0, 0, 1) Mul_Acc(1, 0) \
	Mul_End(1, 1)

#define Mul_4 \
	Mul_Begin(4) \
	Mul_SaveAcc(0, 0, 1) Mul_Acc(1, 0) \
	Mul_SaveAcc(1, 0, 2) Mul_Acc(1, 1) Mul_Acc(2, 0)  \
	Mul_SaveAcc(2, 0, 3) Mul_Acc(1, 2) Mul_Acc(2, 1) Mul_Acc(3, 0)  \
	Mul_SaveAcc(3, 1, 3) Mul_Acc(2, 2) Mul_Acc(3, 1)  \
	Mul_SaveAcc(4, 2, 3) Mul_Acc(3, 2) \
	Mul_End(5, 3)

#define Mul_8 \
	Mul_Begin(8) \
	Mul_SaveAcc(0, 0, 1) Mul_Acc(1, 0) \
	Mul_SaveAcc(1, 0, 2) Mul_Acc(1, 1) Mul_Acc(2, 0)  \
	Mul_SaveAcc(2, 0, 3) Mul_Acc(1, 2) Mul_Acc(2, 1) Mul_Acc(3, 0)  \
	Mul_SaveAcc(3, 0, 4) Mul_Acc(1, 3) Mul_Acc(2, 2) Mul_Acc(3, 1) Mul_Acc(4, 0) \
	Mul_SaveAcc(4, 0, 5) Mul_Acc(1, 4) Mul_Acc(2, 3) Mul_Acc(3, 2) Mul_Acc(4, 1) Mul_Acc(5, 0) \
	Mul_SaveAcc(5, 0, 6) Mul_Acc(1, 5) Mul_Acc(2, 4) Mul_Acc(3, 3) Mul_Acc(4, 2) Mul_Acc(5, 1) Mul_Acc(6, 0) \
	Mul_SaveAcc(6, 0, 7) Mul_Acc(1, 6) Mul_Acc(2, 5) Mul_Acc(3, 4) Mul_Acc(4, 3) Mul_Acc(5, 2) Mul_Acc(6, 1) Mul_Acc(7, 0) \
	Mul_SaveAcc(7, 1, 7) Mul_Acc(2, 6) Mul_Acc(3, 5) Mul_Acc(4, 4) Mul_Acc(5, 3) Mul_Acc(6, 2) Mul_Acc(7, 1) \
	Mul_SaveAcc(8, 2, 7) Mul_Acc(3, 6) Mul_Acc(4, 5) Mul_Acc(5, 4) Mul_Acc(6, 3) Mul_Acc(7, 2) \
	Mul_SaveAcc(9, 3, 7) Mul_Acc(4, 6) Mul_Acc(5, 5) Mul_Acc(6, 4) Mul_Acc(7, 3) \
	Mul_SaveAcc(10, 4, 7) Mul_Acc(5, 6) Mul_Acc(6, 5) Mul_Acc(7, 4) \
	Mul_SaveAcc(11, 5, 7) Mul_Acc(6, 6) Mul_Acc(7, 5) \
	Mul_SaveAcc(12, 6, 7) Mul_Acc(7, 6) \
	Mul_End(13, 7)

#define Mul_16 \
	Mul_Begin(16) \
	Mul_SaveAcc(0, 0, 1) Mul_Acc(1, 0) \
	Mul_SaveAcc(1, 0, 2) Mul_Acc(1, 1) Mul_Acc(2, 0) \
	Mul_SaveAcc(2, 0, 3) Mul_Acc(1, 2) Mul_Acc(2, 1) Mul_Acc(3, 0) \
	Mul_SaveAcc(3, 0, 4) Mul_Acc(1, 3) Mul_Acc(2, 2) Mul_Acc(3, 1) Mul_Acc(4, 0) \
	Mul_SaveAcc(4, 0, 5) Mul_Acc(1, 4) Mul_Acc(2, 3) Mul_Acc(3, 2) Mul_Acc(4, 1) Mul_Acc(5, 0) \
	Mul_SaveAcc(5, 0, 6) Mul_Acc(1, 5) Mul_Acc(2, 4) Mul_Acc(3, 3) Mul_Acc(4, 2) Mul_Acc(5, 1) Mul_Acc(6, 0) \
	Mul_SaveAcc(6, 0, 7) Mul_Acc(1, 6) Mul_Acc(2, 5) Mul_Acc(3, 4) Mul_Acc(4, 3) Mul_Acc(5, 2) Mul_Acc(6, 1) Mul_Acc(7, 0) \
	Mul_SaveAcc(7, 0, 8) Mul_Acc(1, 7) Mul_Acc(2, 6) Mul_Acc(3, 5) Mul_Acc(4, 4) Mul_Acc(5, 3) Mul_Acc(6, 2) Mul_Acc(7, 1) Mul_Acc(8, 0) \
	Mul_SaveAcc(8, 0, 9) Mul_Acc(1, 8) Mul_Acc(2, 7) Mul_Acc(3, 6) Mul_Acc(4, 5) Mul_Acc(5, 4) Mul_Acc(6, 3) Mul_Acc(7, 2) Mul_Acc(8, 1) Mul_Acc(9, 0) \
	Mul_SaveAcc(9, 0, 10) Mul_Acc(1, 9) Mul_Acc(2, 8) Mul_Acc(3, 7) Mul_Acc(4, 6) Mul_Acc(5, 5) Mul_Acc(6, 4) Mul_Acc(7, 3) Mul_Acc(8, 2) Mul_Acc(9, 1) Mul_Acc(10, 0) \
	Mul_SaveAcc(10, 0, 11) Mul_Acc(1, 10) Mul_Acc(2, 9) Mul_Acc(3, 8) Mul_Acc(4, 7) Mul_Acc(5, 6) Mul_Acc(6, 5) Mul_Acc(7, 4) Mul_Acc(8, 3) Mul_Acc(9, 2) Mul_Acc(10, 1) Mul_Acc(11, 0) \
	Mul_SaveAcc(11, 0, 12) Mul_Acc(1, 11) Mul_Acc(2, 10) Mul_Acc(3, 9) Mul_Acc(4, 8) Mul_Acc(5, 7) Mul_Acc(6, 6) Mul_Acc(7, 5) Mul_Acc(8, 4) Mul_Acc(9, 3) Mul_Acc(10, 2) Mul_Acc(11, 1) Mul_Acc(12, 0) \
	Mul_SaveAcc(12, 0, 13) Mul_Acc(1, 12) Mul_Acc(2, 11) Mul_Acc(3, 10) Mul_Acc(4, 9) Mul_Acc(5, 8) Mul_Acc(6, 7) Mul_Acc(7, 6) Mul_Acc(8, 5) Mul_Acc(9, 4) Mul_Acc(10, 3) Mul_Acc(11, 2) Mul_Acc(12, 1) Mul_Acc(13, 0) \
	Mul_SaveAcc(13, 0, 14) Mul_Acc(1, 13) Mul_Acc(2, 12) Mul_Acc(3, 11) Mul_Acc(4, 10) Mul_Acc(5, 9) Mul_Acc(6, 8) Mul_Acc(7, 7) Mul_Acc(8, 6) Mul_Acc(9, 5) Mul_Acc(10, 4) Mul_Acc(11, 3) Mul_Acc(12, 2) Mul_Acc(13, 1) Mul_Acc(14, 0) \
	Mul_SaveAcc(14, 0, 15) Mul_Acc(1, 14) Mul_Acc(2, 13) Mul_Acc(3, 12) Mul_Acc(4, 11) Mul_Acc(5, 10) Mul_Acc(6, 9) Mul_Acc(7, 8) Mul_Acc(8, 7) Mul_Acc(9, 6) Mul_Acc(10, 5) Mul_Acc(11, 4) Mul_Acc(12, 3) Mul_Acc(13, 2) Mul_Acc(14, 1) Mul_Acc(15, 0) \
	Mul_SaveAcc(15, 1, 15) Mul_Acc(2, 14) Mul_Acc(3, 13) Mul_Acc(4, 12) Mul_Acc(5, 11) Mul_Acc(6, 10) Mul_Acc(7, 9) Mul_Acc(8, 8) Mul_Acc(9, 7) Mul_Acc(10, 6) Mul_Acc(11, 5) Mul_Acc(12, 4) Mul_Acc(13, 3) Mul_Acc(14, 2) Mul_Acc(15, 1) \
	Mul_SaveAcc(16, 2, 15) Mul_Acc(3, 14) Mul_Acc(4, 13) Mul_Acc(5, 12) Mul_Acc(6, 11) Mul_Acc(7, 10) Mul_Acc(8, 9) Mul_Acc(9, 8) Mul_Acc(10, 7) Mul_Acc(11, 6) Mul_Acc(12, 5) Mul_Acc(13, 4) Mul_Acc(14, 3) Mul_Acc(15, 2) \
	Mul_SaveAcc(17, 3, 15) Mul_Acc(4, 14) Mul_Acc(5, 13) Mul_Acc(6, 12) Mul_Acc(7, 11) Mul_Acc(8, 10) Mul_Acc(9, 9) Mul_Acc(10, 8) Mul_Acc(11, 7) Mul_Acc(12, 6) Mul_Acc(13, 5) Mul_Acc(14, 4) Mul_Acc(15, 3) \
	Mul_SaveAcc(18, 4, 15) Mul_Acc(5, 14) Mul_Acc(6, 13) Mul_Acc(7, 12) Mul_Acc(8, 11) Mul_Acc(9, 10) Mul_Acc(10, 9) Mul_Acc(11, 8) Mul_Acc(12, 7) Mul_Acc(13, 6) Mul_Acc(14, 5) Mul_Acc(15, 4) \
	Mul_SaveAcc(19, 5, 15) Mul_Acc(6, 14) Mul_Acc(7, 13) Mul_Acc(8, 12) Mul_Acc(9, 11) Mul_Acc(10, 10) Mul_Acc(11, 9) Mul_Acc(12, 8) Mul_Acc(13, 7) Mul_Acc(14, 6) Mul_Acc(15, 5) \
	Mul_SaveAcc(20, 6, 15) Mul_Acc(7, 14) Mul_Acc(8, 13) Mul_Acc(9, 12) Mul_Acc(10, 11) Mul_Acc(11, 10) Mul_Acc(12, 9) Mul_Acc(13, 8) Mul_Acc(14, 7) Mul_Acc(15, 6) \
	Mul_SaveAcc(21, 7, 15) Mul_Acc(8, 14) Mul_Acc(9, 13) Mul_Acc(10, 12) Mul_Acc(11, 11) Mul_Acc(12, 10) Mul_Acc(13, 9) Mul_Acc(14, 8) Mul_Acc(15, 7) \
	Mul_SaveAcc(22, 8, 15) Mul_Acc(9, 14) Mul_Acc(10, 13) Mul_Acc(11, 12) Mul_Acc(12, 11) Mul_Acc(13, 10) Mul_Acc(14, 9) Mul_Acc(15, 8) \
	Mul_SaveAcc(23, 9, 15) Mul_Acc(10, 14) Mul_Acc(11, 13) Mul_Acc(12, 12) Mul_Acc(13, 11) Mul_Acc(14, 10) Mul_Acc(15, 9) \
	Mul_SaveAcc(24, 10, 15) Mul_Acc(11, 14) Mul_Acc(12, 13) Mul_Acc(13, 12) Mul_Acc(14, 11) Mul_Acc(15, 10) \
	Mul_SaveAcc(25, 11, 15) Mul_Acc(12, 14) Mul_Acc(13, 13) Mul_Acc(14, 12) Mul_Acc(15, 11) \
	Mul_SaveAcc(26, 12, 15) Mul_Acc(13, 14) Mul_Acc(14, 13) Mul_Acc(15, 12) \
	Mul_SaveAcc(27, 13, 15) Mul_Acc(14, 14) Mul_Acc(15, 13) \
	Mul_SaveAcc(28, 14, 15) Mul_Acc(15, 14) \
	Mul_End(29, 15)

#define Squ_2 \
	Squ_Begin(2) \
	Squ_End(2)

#define Squ_4 \
	Squ_Begin(4) \
	Squ_SaveAcc(1, 0, 2) Squ_Diag(1) \
	Squ_SaveAcc(2, 0, 3) Squ_Acc(1, 2) Squ_NonDiag \
	Squ_SaveAcc(3, 1, 3) Squ_Diag(2) \
	Squ_SaveAcc(4, 2, 3) Squ_NonDiag \
	Squ_End(4)

#define Squ_8 \
	Squ_Begin(8) \
	Squ_SaveAcc(1, 0, 2) Squ_Diag(1) \
	Squ_SaveAcc(2, 0, 3) Squ_Acc(1, 2) Squ_NonDiag \
	Squ_SaveAcc(3, 0, 4) Squ_Acc(1, 3) Squ_Diag(2) \
	Squ_SaveAcc(4, 0, 5) Squ_Acc(1, 4) Squ_Acc(2, 3) Squ_NonDiag \
	Squ_SaveAcc(5, 0, 6) Squ_Acc(1, 5) Squ_Acc(2, 4) Squ_Diag(3) \
	Squ_SaveAcc(6, 0, 7) Squ_Acc(1, 6) Squ_Acc(2, 5) Squ_Acc(3, 4) Squ_NonDiag \
	Squ_SaveAcc(7, 1, 7) Squ_Acc(2, 6) Squ_Acc(3, 5) Squ_Diag(4) \
	Squ_SaveAcc(8, 2, 7) Squ_Acc(3, 6) Squ_Acc(4, 5)  Squ_NonDiag \
	Squ_SaveAcc(9, 3, 7) Squ_Acc(4, 6) Squ_Diag(5) \
	Squ_SaveAcc(10, 4, 7) Squ_Acc(5, 6) Squ_NonDiag \
	Squ_SaveAcc(11, 5, 7) Squ_Diag(6) \
	Squ_SaveAcc(12, 6, 7) Squ_NonDiag \
	Squ_End(8)

#define Squ_16 \
	Squ_Begin(16) \
	Squ_SaveAcc(1, 0, 2) Squ_Diag(1) \
	Squ_SaveAcc(2, 0, 3) Squ_Acc(1, 2) Squ_NonDiag \
	Squ_SaveAcc(3, 0, 4) Squ_Acc(1, 3) Squ_Diag(2) \
	Squ_SaveAcc(4, 0, 5) Squ_Acc(1, 4) Squ_Acc(2, 3) Squ_NonDiag \
	Squ_SaveAcc(5, 0, 6) Squ_Acc(1, 5) Squ_Acc(2, 4) Squ_Diag(3) \
	Squ_SaveAcc(6, 0, 7) Squ_Acc(1, 6) Squ_Acc(2, 5) Squ_Acc(3, 4) Squ_NonDiag \
	Squ_SaveAcc(7, 0, 8) Squ_Acc(1, 7) Squ_Acc(2, 6) Squ_Acc(3, 5) Squ_Diag(4) \
	Squ_SaveAcc(8, 0, 9) Squ_Acc(1, 8) Squ_Acc(2, 7) Squ_Acc(3, 6) Squ_Acc(4, 5) Squ_NonDiag \
	Squ_SaveAcc(9, 0, 10) Squ_Acc(1, 9) Squ_Acc(2, 8) Squ_Acc(3, 7) Squ_Acc(4, 6) Squ_Diag(5) \
	Squ_SaveAcc(10, 0, 11) Squ_Acc(1, 10) Squ_Acc(2, 9) Squ_Acc(3, 8) Squ_Acc(4, 7) Squ_Acc(5, 6) Squ_NonDiag \
	Squ_SaveAcc(11, 0, 12) Squ_Acc(1, 11) Squ_Acc(2, 10) Squ_Acc(3, 9) Squ_Acc(4, 8) Squ_Acc(5, 7) Squ_Diag(6) \
	Squ_SaveAcc(12, 0, 13) Squ_Acc(1, 12) Squ_Acc(2, 11) Squ_Acc(3, 10) Squ_Acc(4, 9) Squ_Acc(5, 8) Squ_Acc(6, 7) Squ_NonDiag \
	Squ_SaveAcc(13, 0, 14) Squ_Acc(1, 13) Squ_Acc(2, 12) Squ_Acc(3, 11) Squ_Acc(4, 10) Squ_Acc(5, 9) Squ_Acc(6, 8) Squ_Diag(7) \
	Squ_SaveAcc(14, 0, 15) Squ_Acc(1, 14) Squ_Acc(2, 13) Squ_Acc(3, 12) Squ_Acc(4, 11) Squ_Acc(5, 10) Squ_Acc(6, 9) Squ_Acc(7, 8) Squ_NonDiag \
	Squ_SaveAcc(15, 1, 15) Squ_Acc(2, 14) Squ_Acc(3, 13) Squ_Acc(4, 12) Squ_Acc(5, 11) Squ_Acc(6, 10) Squ_Acc(7, 9) Squ_Diag(8) \
	Squ_SaveAcc(16, 2, 15) Squ_Acc(3, 14) Squ_Acc(4, 13) Squ_Acc(5, 12) Squ_Acc(6, 11) Squ_Acc(7, 10) Squ_Acc(8, 9) Squ_NonDiag \
	Squ_SaveAcc(17, 3, 15) Squ_Acc(4, 14) Squ_Acc(5, 13) Squ_Acc(6, 12) Squ_Acc(7, 11) Squ_Acc(8, 10) Squ_Diag(9) \
	Squ_SaveAcc(18, 4, 15) Squ_Acc(5, 14) Squ_Acc(6, 13) Squ_Acc(7, 12) Squ_Acc(8, 11) Squ_Acc(9, 10) Squ_NonDiag \
	Squ_SaveAcc(19, 5, 15) Squ_Acc(6, 14) Squ_Acc(7, 13) Squ_Acc(8, 12) Squ_Acc(9, 11) Squ_Diag(10) \
	Squ_SaveAcc(20, 6, 15) Squ_Acc(7, 14) Squ_Acc(8, 13) Squ_Acc(9, 12) Squ_Acc(10, 11) Squ_NonDiag \
	Squ_SaveAcc(21, 7, 15) Squ_Acc(8, 14) Squ_Acc(9, 13) Squ_Acc(10, 12) Squ_Diag(11) \
	Squ_SaveAcc(22, 8, 15) Squ_Acc(9, 14) Squ_Acc(10, 13) Squ_Acc(11, 12) Squ_NonDiag \
	Squ_SaveAcc(23, 9, 15) Squ_Acc(10, 14) Squ_Acc(11, 13) Squ_Diag(12) \
	Squ_SaveAcc(24, 10, 15) Squ_Acc(11, 14) Squ_Acc(12, 13) Squ_NonDiag \
	Squ_SaveAcc(25, 11, 15) Squ_Acc(12, 14) Squ_Diag(13) \
	Squ_SaveAcc(26, 12, 15) Squ_Acc(13, 14) Squ_NonDiag \
	Squ_SaveAcc(27, 13, 15) Squ_Diag(14) \
	Squ_SaveAcc(28, 14, 15) Squ_NonDiag \
	Squ_End(16)

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
