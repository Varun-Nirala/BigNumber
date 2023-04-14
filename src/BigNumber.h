/*
	@file	: BigNumber.h
	@breif	: Implementing BigNumber(no limit in number of digits) numbers
	@date	: 03-March-2017

	@NOTE	: Currently only supports, Integers

	@Assumption : No operation will reach the limit of number of digit crossing the size of __int64
*/

#ifndef __BIGNUMBER_H__
#define __BIGNUMBER_H__

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "helper.h"

namespace nsNumber
{
// storing number in reverse
// at 0 index we will have right most value
class BigNumber
{
	using ValueType		= std::string;
	using SizeType		= size_t;
	using CharType		= std::string::value_type;
public:
	BigNumber() = default;
	~BigNumber() { clear(); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	explicit BigNumber(const Number number) { parse(std::to_string(number)); }

	explicit BigNumber(const ValueType& num, const ValueType& frac, bool isNegative) { parse((isNegative ? "-" : "") + num + "." + frac); }
	explicit BigNumber(const ValueType& dataString) { parse(dataString); }
	
	// Copy and Move
	BigNumber(const BigNumber& other) { copyFrom(other); }
	BigNumber& operator=(const BigNumber& other) { copyFrom(other); return *this; }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	BigNumber& operator=(const Number number) { parse(std::to_string(number)); return *this; }

	BigNumber(BigNumber&& other) noexcept { moveFrom(std::move(other)); }
	BigNumber& operator=(BigNumber&& other) noexcept { moveFrom(std::move(other)); return *this; }
	
	inline bool empty() const { return m_szFraction.empty() && m_szNumber.empty(); }

	inline SizeType size() const { return (m_bNegative ? 1 : 0) + m_szNumber.size() + m_szFraction.size(); }
	inline void clear() { m_szNumber.clear(); m_szFraction.clear(); m_bNegative = false; }

	inline bool isNegative() const { return m_bNegative; };
	inline bool isInteger() const { return m_szFraction.empty(); };
	inline bool isFloatingPoint() const { return !m_szFraction.empty(); };

	inline void makeFloatingPoint() { m_szFraction.empty() ? m_szFraction.push_back('0') : void(); }

	inline bool isEven() const { return m_szFraction.empty() ? isEven(m_szNumber) : isEven(m_szFraction, true); }
	inline bool isOdd() const { return !isEven(); }

	inline void setMaxPrecision(SizeType val) { m_precision = val; }
	inline SizeType getMaxPrecision() const { return m_precision; }
	
	ValueType significand() const { return (m_bNegative ? "-" : "") + m_szNumber; }
	ValueType fraction() const { return m_szFraction; }

	explicit inline operator ValueType() const { return asString(true, true); }

	// Member overload operators
	// Conversion operator
	template <typename Floating, std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
	explicit inline operator Floating() const { Floating val{}; asFloatingPoint(val); return val; }
	
	template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
	explicit inline operator Integer() const { Integer val{}; asInteger(val); return val; }

	// Member Overloaded operators Comparision operators for Integeral and Floating point numbers
	inline BigNumber operator-() const { BigNumber copy(*this); copy.flipSign(); return copy; }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool operator==(const Number& num) const { return isEqual(num); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool operator!=(const Number& num) const { return !(isEqual(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool operator<(const Number& num) const { return isLessThan(num); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool operator>(const Number& num) const { return !(isLessThan(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool operator<=(const Number& num) const { return (isLessThan(num) || isEqual(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool operator>=(const Number& num) const { return (!isLessThan(num) || isEqual(num)); }

	// Member Overloaded operators Arithmetic operators for Integeral and Floating point numbers
	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline BigNumber& operator+=(const Number& num) { (*this) = add(BigNumber(num)); return *this; }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline BigNumber& operator-=(const Number& num) { (*this) = add(BigNumber(-num)); return *this; }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline BigNumber& operator*=(const Number& num) { (*this) = multiply(BigNumber(num)); return *this; }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline BigNumber& operator/=(const Number& num) { (*this) = divide(BigNumber(num)); return *this; }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline BigNumber& operator%=(const Number& num) { (*this) = modulo(BigNumber(num)); return *this; }

	
	inline BigNumber& operator+=(const BigNumber& other) { (*this) = add(other); return *this; }
	inline BigNumber& operator-=(const BigNumber& other) { (*this) = add(-other); return *this; }
	inline BigNumber& operator*=(const BigNumber& other) { (*this) = multiply(other); return *this; }
	inline BigNumber& operator/=(const BigNumber& other) { (*this) = divide(other); return *this; }
	inline BigNumber& operator%=(const BigNumber& other) { (*this) = modulo(other); return *this; }

	// Prefix
	inline BigNumber operator++() { increment(); return *this; }
	inline BigNumber operator--() { decrement(); return *this; }

	// Postfix
	inline BigNumber operator++(int) { BigNumber temp(*this); increment(); return temp; }
	inline BigNumber operator--(int) { BigNumber temp(*this); decrement(); return temp; }

	inline bool operator<(const BigNumber& other) const { return isLessThan(other); };
	inline bool operator==(const BigNumber& other) const { return isEqual(other); }
	inline bool operator!=(const BigNumber& other) const { return !(isEqual(other)); }
	inline bool operator>(const BigNumber& other) const { return !(isLessThan(other)); }
	inline bool operator<=(const BigNumber& other) const { return (isLessThan(other) || isEqual(other)); }
	inline bool operator>=(const BigNumber& other) const { return (!isLessThan(other) || isEqual(other)); }

	// Friend Overloaded operators
	friend inline BigNumber operator+(const BigNumber& lhs, const BigNumber& rhs) { return lhs.add(rhs); }
	friend inline BigNumber operator-(const BigNumber& lhs, const BigNumber& rhs) { return lhs.add(-rhs); }
	friend inline BigNumber operator*(const BigNumber& lhs, const BigNumber& rhs) { return lhs.multiply(rhs); }
	friend inline BigNumber operator/(const BigNumber& lhs, const BigNumber& rhs) { return lhs.divide(rhs); }
	friend inline BigNumber operator%(const BigNumber& lhs, const BigNumber& rhs) { return lhs.modulo(rhs); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator+(const BigNumber& lhs, const Number& num) { return lhs.add(BigNumber(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator-(const BigNumber& lhs, const Number& num) { return lhs.add(BigNumber(-num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator*(const BigNumber& lhs, const Number& num) { return lhs.multiply(BigNumber(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator/(const BigNumber& lhs, const Number& num) { return lhs.divide(BigNumber(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator%(const BigNumber& lhs, const Number& num) { return lhs.modulo(BigNumber(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator+(const Number& num, const BigNumber& rhs) { return rhs.add(BigNumber(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator-(const Number& num, const BigNumber& rhs) { return BigNumber(num).add(-rhs); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator*(const Number& num, const BigNumber& rhs) { return rhs.multiply(BigNumber(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator/(const Number& num, const BigNumber& rhs) { return BigNumber(num).divide(rhs); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber operator%(const Number& num, const BigNumber& rhs) { return BigNumber(num).modulo(rhs); }

	// friend functions
	friend inline BigNumber pow(const BigNumber& lhs, const BigNumber& rhs) { return lhs.power(rhs); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber pow(const BigNumber& lhs, const Number& num) { return lhs.power(BigNumber(num)); }

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	friend inline BigNumber pow(const Number& num, const BigNumber& rhs) { return BigNumber(num).power(rhs); }

	friend inline std::ostream& operator<< (std::ostream& out, const BigNumber& obj) { out << (ValueType)obj << '\n'; return out; }

public:
	inline int to_int(const CharType ch) const { return ch - '0'; }
	inline CharType to_char(const int digit) const { return char(digit + '0'); }
	inline void flipSign() { m_bNegative = !m_bNegative; }

	inline long long stoll_s(const ValueType& str) const { return (str.find_first_not_of("0.") == ValueType::npos) ? 0 : std::stoll(str); }
	inline long double stold_s(const ValueType& str) const { return (str.find_first_not_of("0.") == ValueType::npos) ? 0 : std::stold(str); }

	inline bool isLessThan(const BigNumber& other) const { return compareStringAsNumber(m_szNumber, other.m_szNumber); }

	inline ValueType asString(const bool withSign, const bool combineWithDecimal) const;

	inline bool isEqual(const BigNumber& other) const;

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool isLessThan(const Number other) const;

	template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool> = true>
	inline bool isEqual(const Number other) const;

	template <typename T>
	inline bool asInteger(T& val, const ValueType &str = "", const bool quiet = true) const;

	template <typename T>
	inline bool asFloatingPoint(T& val, const ValueType &str = "", const bool quiet = true) const;

	inline void copyFrom(const BigNumber& other);
	inline void moveFrom(BigNumber&& other);

	inline bool isValid(const ValueType& str, SizeType& posOfDecimalPoint) const;
	inline void parse(const ValueType& str);

	inline BigNumber add(const BigNumber &other) const;
	inline BigNumber multiply(const BigNumber& other) const;
	inline BigNumber divide(const BigNumber& other) const;
	inline BigNumber modulo(const BigNumber& other) const;
	inline BigNumber power(const BigNumber &exp) const;

	inline ValueType addHelper(const ValueType &in1, const ValueType &in2) const;
	inline ValueType subHelper(const ValueType &in1, const ValueType &in2) const;

	inline ValueType addHelper(ValueType in1, ValueType in2, int &carry, const bool isFractionPart = false) const;
	inline ValueType subHelper(ValueType in1, ValueType in2, int &borrow, const bool isFractionPart = false) const;

	inline ValueType multiplyHelper(ValueType in1, ValueType in2) const;

	inline ValueType longMultiplication(ValueType in1, ValueType in2) const;
	inline ValueType karatsubaMultiplication(const ValueType &in1, const ValueType &in2) const;

	inline ValueType divideAsIntegers(ValueType numerator, ValueType denominator, ValueType&remainder) const;
	inline ValueType divideAsFloatingPoint(const ValueType&num1, const ValueType& frac1, const ValueType& num2, const ValueType& frac2) const;

	inline ValueType powerHelperIntegerExponent(const ValueType&base, const ValueType&exp) const;

	inline ValueType nth_Root(const ValueType &num, const ValueType &fraction) const;

	inline void multiplyBy10(uint64_t times = 1);
	inline void divideBy10(uint64_t times = 1);

	inline void increment();
	inline void decrement();

	inline void increment(ValueType &str) const;
	inline void decrement(ValueType &str) const;

	inline bool isEven(const ValueType& str, const bool isFractionPart = false) const;

	inline void split_at(const ValueType str, const SizeType digitCountInLow, ValueType& high, ValueType& low) const;
	inline std::pair<ValueType, ValueType> split_at(const ValueType& str, const CharType delimeter = '.') const;
	inline void makeEqualLength(ValueType& str1, ValueType& str2, const bool isFractionPart = false) const;
	inline void trimZeros(ValueType& str, const bool isFractionPart = false) const;
	inline int compareStringAsNumber(const ValueType& str1, const ValueType& str2, const bool isFractionPart) const;
	inline int compareStringAsNumber(const ValueType& str1, const ValueType& str2) const;
	inline int fillUpDstForDivision(ValueType &dst, ValueType &numerator, const ValueType &denominator) const;

	inline void roundOff(ValueType& num, ValueType& frac, const SizeType precision) const;
	inline void roundOff(ValueType& number, const SizeType precision) const;

	inline int rounder(ValueType& str, const SizeType pos, const SizeType precision) const;
	
private:
	static const int kBLOCK_SIZE;
	static const SizeType kPRECISION;
private:
	bool					m_bNegative{false};
	ValueType				m_szNumber{"0"};
	ValueType				m_szFraction;
	SizeType				m_precision{ kPRECISION };
};

const BigNumber::SizeType BigNumber::kPRECISION = 6;
const int BigNumber::kBLOCK_SIZE = 12;

static const BigNumber sNAN("NAN");
static const BigNumber& sDIVIDE_BY_ZERO(sNAN);
static const BigNumber sINFINITY("INFINITY");

// Private Methods
BigNumber::ValueType BigNumber::asString(const bool withSign, const bool combineWithDecimal) const
{
	ValueType szRet = m_szNumber.empty() ? "0" : m_szNumber;
	if (withSign && m_bNegative)
	{
		szRet.insert(0, 1, '-');
	}
	if (!m_szFraction.empty())
	{
		if (combineWithDecimal)
		{
			szRet.insert(szRet.size(), 1, '.');
		}
		szRet.insert(szRet.size(), m_szFraction);
	}
	return szRet;
}

bool BigNumber::isEqual(const BigNumber& other) const
{
	return m_szNumber == other.m_szNumber && m_szFraction == other.m_szFraction && m_bNegative == other.m_bNegative;
}

template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool>>
bool BigNumber::isLessThan(const Number other) const
{
	Number val{};
	if (std::is_integral<Number>::value)
	{
		return asInteger(val) && val < other;
	}
	return asFloatingPoint(val) && val < other;
}

template <typename Number, std::enable_if_t<std::is_floating_point<Number>::value || std::is_integral<Number>::value, bool>>
bool BigNumber::isEqual(const Number other) const
{
	Number val{};
	if (std::is_integral<Number>::value)
	{
		return isInteger() && asInteger(val) && val == other;
	}
	return asFloatingPoint(val) && val == other;
}

template <typename T>
bool BigNumber::asInteger(T& val, const ValueType &str, const bool quiet) const
{
	val = static_cast<T>(0);
	try
	{
		val = str.empty() ? static_cast<T>(stoll_s(significand())) : static_cast<T>(stoll_s(str));
	}
	catch (std::exception& e)
	{
		if (!quiet)
		{
			LOG_ERROR("Exception : " + ValueType(e.what()));
		}
		return false;
	}
	return true;
}

template <typename T>
bool BigNumber::asFloatingPoint(T& val, const ValueType &str, const bool quiet) const
{
	val = static_cast<T>( 0.0 );
	try
	{
		val = str.empty() ? static_cast<T>(stold_s(static_cast<ValueType>(*this))) : static_cast<T>(stold_s(str));
	}
	catch (std::exception& e)
	{
		if (!quiet)
		{
			LOG_ERROR("Exception : " + ValueType(e.what()));
		}
		return false;
	}
	return true;
}

void BigNumber::copyFrom(const BigNumber& other)
{
	if (this != &other)
	{
		m_szNumber = other.m_szNumber;
		m_bNegative = other.m_bNegative;
		m_szFraction = other.m_szFraction;
	}
}

void BigNumber::moveFrom(BigNumber&& other)
{
	assert(this != &other);
	m_szNumber = std::exchange(other.m_szNumber, ValueType());
	m_szFraction = std::exchange(other.m_szFraction, ValueType());
	m_bNegative = std::exchange(other.m_bNegative, false);
}

bool BigNumber::isValid(const ValueType& str, SizeType& posOfDecimalPoint) const
{
	if (str.empty())
	{
		return false;
	}
	posOfDecimalPoint = 0;
	SizeType i = (str[0] == '-' ? 1 : 0);
	bool decimalPoint = false;
	while (i < str.size())
	{
		if (str[i] == '.')
		{
			if (decimalPoint)
			{
				return false;
			}
			decimalPoint = true;
			posOfDecimalPoint = i;
		}
		else if (!std::isdigit(str[i]))
		{
			return false;
		}
		i++;
	}
	return true;
}

void BigNumber::parse(const ValueType& str)
{
	clear();

	static int count = 0;
	if (count < 3 && (str == "NAN" || str == "INFINITY"))
	{
		count++;
		m_szNumber = str;
		return;
	}

	SizeType posOfDecimalPoint{};
	if (!isValid(str, posOfDecimalPoint))
	{
		LOG_ERROR("Invalid number.");
		return;
	}
	SizeType startPos = 0;
	if (str[startPos] == '-')
	{
		m_bNegative = true;
		startPos++;
	}
	if (str[posOfDecimalPoint] != '.')
	{
		m_szNumber = str.substr(startPos);
	}
	else
	{
		m_szNumber = str.substr(startPos, posOfDecimalPoint - startPos);
		m_szFraction = str.substr(posOfDecimalPoint + 1);
	}
	trimZeros(m_szNumber);
	trimZeros(m_szFraction, true);

	roundOff(m_szNumber, m_szFraction, m_precision);

	if (empty())
	{
		m_bNegative = false;
		LOG_ERROR("Invalid number.");
	}
}

BigNumber BigNumber::add(const BigNumber& other) const
{
	if (empty() || other.empty())
	{
		LOG_ERROR("INVALID Operation!");
		return BigNumber();
	}

	bool bNegative = false;
	ValueType strFraction;
	ValueType strNumber("0");

	if (m_bNegative == other.m_bNegative)
	{
		// Add but sign is negative
		int carry{};
		bNegative = m_bNegative;
		strFraction = addHelper(m_szFraction, other.m_szFraction, carry, true);
		strNumber = addHelper(m_szNumber, other.m_szNumber, carry);
	}
	else if (m_bNegative ^ other.m_bNegative)
	{
		int borrow{};
		int cmpVal = compareStringAsNumber(m_szNumber, other.m_szNumber, false);
		// subtract with sign if bigger number
		if (cmpVal < 0)
		{
			strFraction = subHelper(other.m_szFraction, m_szFraction, borrow, true);
			strNumber = subHelper(other.m_szNumber, m_szNumber, borrow);
			bNegative = other.m_bNegative;
		}
		else if (cmpVal > 0)
		{
			strFraction = subHelper(m_szFraction, other.m_szFraction, borrow, true);
			strNumber = subHelper(m_szNumber, other.m_szNumber, borrow);
			bNegative = m_bNegative;
		}
		else
		{
			cmpVal = compareStringAsNumber(m_szFraction, other.m_szFraction, true);
			if (cmpVal < 0)
			{
				strFraction = subHelper(other.m_szFraction, m_szFraction, borrow, true);
			}
			else if (cmpVal > 0)
			{
				strFraction = subHelper(m_szFraction, other.m_szFraction, borrow, true);
			}
		}
	}
	return BigNumber(strNumber, strFraction, bNegative);
}

BigNumber BigNumber::multiply(const BigNumber& other) const
{
	if (empty() || other.empty())
	{
		LOG_ERROR("INVALID Operation!");
		return BigNumber();
	}
	if (isEqual(0) || other.isEqual(0))
	{
		return BigNumber(0);
	}
	if (isEqual(1) || isEqual(-1))
	{
		return m_bNegative ? -other : other;
	}
	if (other.isEqual(1) || other.isEqual(-1))
	{
		return other.m_bNegative ? -(*this) : (*this);
	}
	return BigNumber(multiplyHelper(asString(true, true), other.asString(true, true)));
}

BigNumber BigNumber::divide(const BigNumber& other) const
{
	if (empty() || other.empty())
	{
		LOG_ERROR("INVALID Operation!");
		return sNAN;
	}
	if (isEqual(0))
	{
		return BigNumber(0);
	}
	if (other.isEqual(0))
	{
		return sDIVIDE_BY_ZERO;
	}
	if (other.isEqual(1) || other.isEqual(-1))
	{
		return (m_bNegative == other.m_bNegative) ? (*this) : -(*this);
	}
	if (isEqual(other))
	{
		return BigNumber("1");
	}
	if (isEqual(-other))
	{
		return BigNumber("-1");
	}
	if (isInteger() && other.isInteger())
	{
		ValueType rem;
		return BigNumber(((m_bNegative == other.m_bNegative) ? "" : "-") + divideAsIntegers(m_szNumber, other.m_szNumber, rem));
	}
	return BigNumber(((m_bNegative == other.m_bNegative) ? "" : "-") + divideAsFloatingPoint(m_szNumber, m_szFraction, other.m_szNumber, other.m_szFraction));
}

BigNumber BigNumber::modulo(const BigNumber& other) const
{
	if (empty() || other.empty())
	{
		LOG_ERROR("INVALID Operation!");
		return BigNumber();
	}
	if (isEqual(0) || other.isEqual(1))
	{
		return BigNumber("0");
	}
	if (other.isEqual(0))
	{
		return sDIVIDE_BY_ZERO;
	}
	if (isEqual(other) || isEqual(-other))
	{
		return BigNumber("0");
	}

	ValueType rem;
	if (!isFloatingPoint() && !other.isFloatingPoint())
	{
		divideAsIntegers(m_szNumber, other.m_szNumber, rem);
	}
	else
	{
		divideAsIntegers(asString(false, false), other.asString(false, false), rem);
		const SizeType decimalPos = m_szFraction.size() + other.m_szFraction.size();
		if (rem.size() < decimalPos)
		{
			rem.insert(0, decimalPos - rem.size(), '0');
		}
		rem.insert(rem.size() - decimalPos, 1, '0');
	}
	return BigNumber((other.isNegative() ? "-" : "") + rem);
}

BigNumber BigNumber::power(const BigNumber& exp) const
{
	if (empty() || exp.empty())
	{
		LOG_ERROR("INVALID Operation!");
		return BigNumber();
	}

	if (isEqual(0))
	{
		return exp.m_bNegative ? sNAN : BigNumber("0");
	}
	if (isEqual(1))
	{
		return BigNumber(1);
	}
	if (exp.isEqual(1))
	{
		return (*this);
	}
	if (exp.isEqual(0))
	{
		return BigNumber("1");
	}
	if (exp.isNegative())
	{
		const std::pair<ValueType, ValueType> p = split_at( static_cast<ValueType>( this->power( -exp)) );
		return BigNumber(divideAsFloatingPoint("1", "", p.first, p.second));
	}
	bool bNegative{ false };
	if (m_bNegative && !exp.isEven())
	{
		bNegative = true;
	}

	ValueType szAns = powerHelperIntegerExponent(asString(false, true), exp.m_szNumber);
	if (exp.isFloatingPoint())
	{	
		szAns = multiplyHelper(szAns, nth_Root(asString(false, true), exp.m_szFraction));
	}
	//If n is an even integer, then(−1)n = 1.
	//If n is an odd integer, then(−1)n = −1.
	return BigNumber((bNegative ? "-" : "") + szAns);
}

BigNumber::ValueType BigNumber::addHelper(const ValueType &in1, const ValueType &in2) const
{
	const std::pair<ValueType, ValueType> p1 = split_at(in1);
	const std::pair<ValueType, ValueType> p2 = split_at(in2);
	int carry = 0;
	ValueType szAns;
	if (!p1.second.empty() || !p2.second.empty())
	{
		szAns = addHelper(p1.second, p2.second, carry, true);
		trimZeros(szAns, true);
		szAns.insert(0, 1, '.');
	}
	szAns.insert(0, addHelper(p1.first, p2.first, carry, false));
	trimZeros(szAns);
	
	roundOff(szAns, m_precision);
	if (szAns.find('.') == 0)
	{
		szAns.insert(0, 1, '0');
	}
	return szAns;
}

BigNumber::ValueType BigNumber::subHelper(const ValueType &in1, const ValueType &in2) const
{
	int cmpVal = compareStringAsNumber(in1, in2);
	if (cmpVal < 0)
	{
		return subHelper(in2, in1);
	}
	else if (cmpVal == 0)
	{
		return "0";
	}

	const std::pair<ValueType, ValueType> p1 = split_at(in1);
	const std::pair<ValueType, ValueType> p2 = split_at(in2);
	
	int borrow = 0;
	ValueType szAns;
	if (!p1.second.empty() || !p2.second.empty())
	{
		szAns = subHelper(p1.second, p2.second, borrow, true);
		trimZeros(szAns, true);
		szAns.insert(0, 1, '.');
	}
	szAns.insert(0, subHelper(p1.first, p2.first, borrow, false));
	trimZeros(szAns);
	roundOff(szAns, m_precision);
	if (szAns.find('.') == 0)
	{
		szAns.insert(0, 1, '0');
	}
	return szAns;
}

BigNumber::ValueType BigNumber::addHelper(ValueType in1, ValueType in2, int& carry, const bool isFractionPart) const
{
	ValueType szAns;
	if (in1.empty() && in2.empty())
	{
		return szAns;
	}
	else if (in1.empty() || in2.empty())
	{
		szAns = in1.empty() ? in2 : in1;
		return szAns;
	}
	
	makeEqualLength(in1, in2, isFractionPart);
	szAns.reserve(in1.size() + 1);

	if (isFractionPart)
	{
		carry = 0;
	}

	ValueType str;
	SizeType rt = in1.size();
	for (SizeType lt = 0; rt > kBLOCK_SIZE; rt -= kBLOCK_SIZE)
	{
		lt = rt - kBLOCK_SIZE;
		lt = lt > rt ? 0 : lt;
		str = std::to_string(stoll_s(in1.substr(lt, kBLOCK_SIZE)) + stoll_s(in2.substr(lt, kBLOCK_SIZE)) + carry);
		if (str.size() < kBLOCK_SIZE)
		{
			str.insert(0, kBLOCK_SIZE - str.size(), '0');
		}
		carry = 0;
		if (str.size() > kBLOCK_SIZE)
		{
			carry = to_int(str[0]);
			str.erase(str.begin());
		}
		szAns.insert(0, str);
	}
	str = std::to_string(stoll_s(in1.substr(0, rt)) + stoll_s(in2.substr(0, rt)) + carry);
	carry = 0;
	if (isFractionPart && str.size() > rt)
	{
		carry = to_int(str[0]);
		str.erase(str.begin());
	}
	szAns.insert(0, str);
	return szAns;
}

BigNumber::ValueType BigNumber::subHelper(ValueType in1, ValueType in2, int& borrow, const bool isFractionPart) const
{
	ValueType szAns;
	if (in1.empty() && in2.empty())
	{
		return szAns;
	}
	else if (in2.empty())
	{
		szAns = in1;
		return szAns;
	}
	else if (in1.empty())
	{
		szAns = in2;
		return szAns;
	}

	makeEqualLength(in1, in2, isFractionPart);
	szAns.reserve(in1.size());
	if (isFractionPart)
	{
		borrow = 0;
	}

	ValueType str;
	ValueType sz1;
	ValueType sz2;
	str.reserve(kBLOCK_SIZE);
	sz1.reserve(kBLOCK_SIZE);
	sz2.reserve(kBLOCK_SIZE);

	SizeType rt = in1.size();
	for (SizeType lt = 0; rt > kBLOCK_SIZE; rt -= kBLOCK_SIZE)
	{
		lt = rt - kBLOCK_SIZE;
		lt = lt > rt ? 0 : lt;

		sz1 = in1.substr(lt, kBLOCK_SIZE);
		sz2 = in2.substr(lt, kBLOCK_SIZE);
		if (borrow)
		{
			increment(sz2);
			borrow = 0;
		}
		int cmpVal = compareStringAsNumber(sz1, sz2, false);
		if (cmpVal > 0)
		{
			str = std::to_string(stoll_s(sz1) - stoll_s(sz2));
		}
		else if (cmpVal < 0)
		{
			if (sz1.size() != sz2.size())
			{
				makeEqualLength(sz1, sz2);
			}
			sz1.insert(0, 1, '1');
			str = std::to_string(stoll_s(sz1) - stoll_s(sz2));
			borrow = 1;
		}
		if (str.size() < kBLOCK_SIZE)
		{
			str.insert(0, kBLOCK_SIZE - str.size(), '0');
		}
		szAns.insert(0, str);
		str.clear();
	}
	sz1 = in1.substr(0, rt);
	sz2 = in2.substr(0, rt);
	if (borrow)
	{
		increment(sz2);
		borrow = 0;
	}
	int cmpVal = compareStringAsNumber(sz1, sz2, false);
	if (cmpVal > 0)
	{
		str = std::to_string(stoll_s(sz1) - stoll_s(sz2));
	}
	else if (cmpVal < 0)
	{
		if (sz1.size() != sz2.size())
		{
			makeEqualLength(sz1, sz2);
		}
		sz1.insert(0, 1, '1');
		str = std::to_string(stoll_s(sz1) - stoll_s(sz2));
		borrow = 1;
	}
	szAns.insert(0, str);
	return szAns;
}

BigNumber::ValueType BigNumber::multiplyHelper(ValueType in1, ValueType in2) const
{
	bool bNegative = false;
	ValueType szAns;
	if (in1.size() > 0 && in2.size() > 0)
	{
		if (in1[0] == '-')
		{
			in1.erase(0, 1);
			bNegative = true;
		}
		if (in2[0] == '-')
		{
			in2.erase(0, 1);
			bNegative = !bNegative;
		}
	}
	if (in1.empty() || in2.empty())
	{
		LOG_ERROR("INVALID Operation!");
		return szAns;
	}

	SizeType decimalPos = ValueType::npos;
	SizeType pos = in1.find_first_of('.');
	if (pos != ValueType::npos)
	{
		in1.erase(pos, 1);
		decimalPos = in1.size() - pos;
	}

	pos = in2.find_first_of('.');
	if (pos != ValueType::npos)
	{
		in2.erase(pos, 1);
		if (decimalPos == ValueType::npos)
		{
			decimalPos = in2.size() - pos;
		}
		else
		{
			decimalPos += in2.size() - pos;
		}
	}

	szAns.reserve(in1.size() + in2.size());
	if (in1.size() > 32 || in2.size() > 32)
	{
		szAns = karatsubaMultiplication(in1, in2);
	}
	else
	{
		szAns = longMultiplication(in1, in2);
	}
	trimZeros(szAns);
	if (decimalPos != ValueType::npos)
	{
		szAns.insert(szAns.size() - decimalPos, 1, '.');
	}
	trimZeros(szAns, true);
	if (bNegative)
	{
		szAns.insert(0, 1, '-');
	}
	roundOff(szAns, m_precision);

	if (szAns.find('.') == 0)
	{
		szAns.insert(0, 1, '0');
	}
	return szAns;
}

BigNumber::ValueType BigNumber::longMultiplication(ValueType in1, ValueType in2) const
{
	ValueType szAns;
	if (in1.empty() || in2.empty())
	{
		szAns.push_back('0');
		return szAns;
	}

	std::reverse(in1.begin(), in1.end());
	std::reverse(in2.begin(), in2.end());

	int base = 10;

	SizeType in1Max = in1.size();
	SizeType in2Max = in2.size();

	szAns.resize(in1Max + in2Max, '0');

	int carry = 0;
	int num;
	for (SizeType b_i = 0; b_i < in2Max; ++b_i)
	{
		carry = 0;
		for (SizeType a_i = 0; a_i < in1Max; ++a_i)
		{
			num = to_int(szAns[a_i + b_i]);

			num += carry + to_int(in2[b_i]) * to_int(in1[a_i]);
			carry = num / base;
			szAns[a_i + b_i] = to_char(num % base);
		}
		szAns[b_i + in1Max] = to_char(carry);
	}
	std::reverse(szAns.begin(), szAns.end());
	return szAns;
}

//function karatsuba(num1, num2)
//if (num1 < 10) or (num2 < 10)
//	return num1 × num2 /* fall back to traditional multiplication */
//
///* Calculates the size of the numbers. */
//m = min(size_base10(num1), size_base10(num2))
//m2 = floor(m / 2)
///* m2 = ceil (m / 2) will also work */
//
///* Split the digit sequences in the middle. */
//high1, low1 = split_at(num1, m2)
//high2, low2 = split_at(num2, m2)
//
///* 3 recursive calls made to numbers approximately half the size. */
//z0 = karatsuba(low1, low2)
//z1 = karatsuba(low1 + high1, low2 + high2)
//z2 = karatsuba(high1, high2)
//
//return (z2 × 10 ^ (m2 × 2)) + ((z1 - z2 - z0) × 10 ^ m2) + z0

BigNumber::ValueType BigNumber::karatsubaMultiplication(const ValueType &num1, const ValueType &num2) const
{
	if (num1.size() < 8 && num2.size() < 8)
	{
		return std::to_string(stoll_s(num1) * stoll_s(num2));
	}
	else if (num1.size() < 2 || num2.size() < 2)
	{
		return longMultiplication(num1, num2);
	}

	SizeType m2 = (std::min(num1.size(), num2.size())) / 2;

	//split_at("12345", 3) will extract the 3 final digits, giving: high = "12", low = "345".

	ValueType high1;
	ValueType low1;
	split_at(num1, m2, high1, low1);

	ValueType high2;
	ValueType low2;
	split_at(num2, m2, high2, low2);

	BigNumber z0{ karatsubaMultiplication(low1, low2) };
	BigNumber z1{ karatsubaMultiplication(addHelper(low1, high1), addHelper(low2, high2)) };
	BigNumber z2{ karatsubaMultiplication(high1, high2) };
	//return (z2 × 10 ^ (m2 × 2)) + ((z1 - z2 - z0) × 10 ^ m2) + z0
	z1 -= z2;
	z1 -= z0;
	z2.multiplyBy10(2 * m2);
	z1.multiplyBy10(m2);
	return static_cast<ValueType>(z2 + z1 + z0);
}

BigNumber::ValueType BigNumber::divideAsIntegers(ValueType numerator, ValueType denominator, ValueType& remainder) const
{
	// if a and b are +ve and b != 0
	// if a < b, then 0,a
	// let q = 0
	// let r = a
	// repeat
	//		let q = q + 1
	//		let r = r - b
	// until r < b
	// return q,r
	numerator = split_at(numerator).first;
	denominator = split_at(denominator).first;
	ValueType quotient("0");
	int cmpVal = compareStringAsNumber(numerator, denominator, false);
	if (cmpVal < 0)
	{
		remainder = numerator;
	}
	else if (cmpVal == 0)
	{
		remainder = "0";
		quotient = "1";
	}
	else if(numerator.size() == denominator.size() || numerator.size() == (1 + denominator.size()))
	{
		remainder = numerator;
		do
		{
			increment(quotient);
			remainder = subHelper(remainder, denominator);
		} while (compareStringAsNumber(remainder, denominator, false) >= 0);
	}
	else
	{
		quotient.clear();
		cmpVal = fillUpDstForDivision(remainder, numerator, denominator);
		while (cmpVal >= 0)
		{
			quotient += divideAsIntegers(remainder, denominator, remainder);
			if (remainder == "0")
			{
				remainder.clear();
			}
			SizeType zerosToPad = remainder.size();
			cmpVal = fillUpDstForDivision(remainder, numerator, denominator);
			zerosToPad = remainder.size() - zerosToPad;
			if (zerosToPad > 1)
			{
				zerosToPad--;
				quotient.insert(quotient.size(), zerosToPad, '0');
			}
		}
	}
	return quotient;
}

BigNumber::ValueType BigNumber::divideAsFloatingPoint(const ValueType& num1, const ValueType& frac1, const ValueType& num2, const ValueType& frac2) const
{
	bool bMultiply{ false };
	SizeType decimalPos{};
	if (frac1.size() > frac2.size())
	{
		decimalPos = frac1.size() - frac2.size();
	}
	else if (frac1.size() < frac2.size())
	{
		decimalPos = frac2.size() - frac1.size();
		bMultiply = true;
	}

	const ValueType denominator{ num2 + frac2 };
	ValueType remainder;
	ValueType szAns = divideAsIntegers(num1 + frac1, denominator, remainder);
	
	if (remainder != "0")
	{
		ValueType tempNumerator;
		ValueType fraction;
		SizeType denomSize = denominator.size();

		tempNumerator.insert(0, denomSize + 1, '0');

		while (remainder != "0" && fraction.size() < m_precision)
		{
			SizeType zerosToPad = remainder.size();
			fillUpDstForDivision(remainder, tempNumerator, denominator);
			zerosToPad = remainder.size() - zerosToPad;
			if (zerosToPad > 1)
			{
				zerosToPad--;
				fraction.insert(fraction.size(), zerosToPad, '0');
			}
			fraction += divideAsIntegers(remainder, denominator, remainder);
			tempNumerator.insert(0, 1 + denomSize - tempNumerator.size(), '0');
		}

		SizeType pos = fraction.find('.');
		if (pos != ValueType::npos)
		{
			pos = fraction.size() - pos;
			fraction.erase('.');
			if (bMultiply)
			{
				if (pos > decimalPos)
				{
					decimalPos = pos - decimalPos;
					bMultiply = false;
				}
				else
				{
					decimalPos = decimalPos - pos;
				}
			}
		}
		if (szAns == "0")
		{
			szAns.clear();
		}
		szAns += fraction;
		if (bMultiply)
		{
			if (decimalPos >= fraction.size())
			{
				decimalPos -= fraction.size();
			}
			else
			{
				decimalPos = fraction.size() - decimalPos;
				bMultiply = false;
			}
		}
		else
		{
			decimalPos += fraction.size();
		}


		if (!decimalPos)
		{
			fraction.insert(0, 1, '.');
		}
	}
	if (decimalPos)
	{
		if (bMultiply)
		{
			szAns.insert(szAns.size(), decimalPos, '0');
		}
		else
		{
			if (szAns.size() < decimalPos)
			{
				szAns.insert(0, decimalPos - szAns.size(), '0');
			}
			szAns.insert(szAns.size() - decimalPos, 1, '.');
		}
	}
	trimZeros(szAns);
	trimZeros(szAns, true);
	roundOff(szAns, m_precision);
	if (szAns.find('.') == 0)
	{
		szAns.insert(0, 1, '0');
	}
	return szAns;
}

BigNumber::ValueType BigNumber::powerHelperIntegerExponent(const ValueType &base, const ValueType &exp) const
{
	if (base == "0")
	{
		return "0";
	}
	if (exp == "0")
	{
		return "1";
	}
	if (exp == "1")
	{
		return base;
	}

	ValueType rem;
	const ValueType halfPower = powerHelperIntegerExponent(base, divideAsIntegers(exp, "2", rem));
	if (isEven(exp))
	{
		return multiplyHelper(halfPower, halfPower);
	}
	else
	{
		if (compareStringAsNumber(exp, "0", false) > 0)
		{
			return multiplyHelper(base, multiplyHelper(halfPower, halfPower));
		}
		else
		{
			const std::pair<ValueType, ValueType> p1{ split_at(multiplyHelper(halfPower, halfPower)) };
			const std::pair<ValueType, ValueType> p2{ split_at(base) };
			return divideAsFloatingPoint(p1.first, p1.second, p2.first, p2.second);
		}
	}
}

/*From geeksforgeeks
* double nthRoot(int A, int N)
*{
*	// initially guessing a random number between 0 and 9
*	double xPre = 1;// rand() % 10;
*	//  smaller eps, denotes more accuracy
*	double eps = 1e-3;
*	// initializing difference between two roots by INT_MAX
*	double delX = INT_MAX;
*	//  xK denotes current value of x
*	double xK;
*	//  loop until we reach desired accuracy
*	while (delX > eps)
*	{
*		//  calculating current value from previous
*		// value by newton's method
*		xK = ((N - 1.0) * xPre + (double)A / pow(xPre, N - 1)) / (double)N;
*		delX = abs(xK - xPre);
*		xPre = xK;
*	}
*	return xK;
*}
*/
BigNumber::ValueType BigNumber::nth_Root(const ValueType &num, const ValueType &fraction) const
{
	ValueType xPre{ std::to_string(rand() % 10) };

	const ValueType eps{ "0.0010000000000000" };

	ValueType delX{ std::to_string(INT_MAX) };
	
	ValueType xK;

	const ValueType n_minus_1{ subHelper(divideAsFloatingPoint("1", "", "", fraction), "1")};
	const std::pair<ValueType, ValueType> numPair = split_at(num);

	const ValueType frac{ "0." + fraction };
	std::pair<ValueType, ValueType> p;
	int counter = 0;
	while (compareStringAsNumber(delX, eps) > 0 && counter++ < 20)
	{
		p = split_at(powerHelperIntegerExponent(xPre, n_minus_1));
		ValueType b1{ multiplyHelper(n_minus_1, xPre) };
		ValueType b2{ divideAsFloatingPoint(numPair.first, numPair.second, p.first, p.second) };
		ValueType c = addHelper(b1, b2);
		xK = multiplyHelper(c, frac);
		if (compareStringAsNumber(xK, xPre) > 0)
		{
			delX = subHelper(xK, xPre);
		}
		else
		{
			delX = subHelper(xPre, xK);
		}
		printf("p = %s, b1 = %s, b2 = %s, c = %s, xk = %s, xPre = %s, delX = %s\n", (p.first + "." + p.second).c_str(), b1.c_str(), b2.c_str(), c.c_str(), xK.c_str(), xPre.c_str(), delX.c_str());
		xPre = xK;
	}
	return xK;
}

void BigNumber::multiplyBy10(uint64_t times)
{
	if (m_szFraction.size() > times)
	{
		m_szNumber.insert(m_szNumber.size(), m_szFraction.substr(0, times));
		m_szFraction.erase(0, times);
		trimZeros(m_szFraction, true);
	}
	else if (m_szFraction.size() == times)
	{
		m_szNumber += m_szFraction;
		m_szFraction.clear();
	}
	else
	{
		m_szNumber += m_szFraction;
		times -= m_szFraction.size();
		m_szNumber.insert(m_szNumber.size(), times, '0');
		m_szFraction.clear();
	}
	trimZeros(m_szNumber);
}

void BigNumber::divideBy10(uint64_t times)
{
	if (m_szNumber.size() > times)
	{
		m_szFraction.insert(0, m_szNumber.substr(m_szNumber.size() - times));
		m_szNumber.erase(m_szNumber.size() - times, times);
		trimZeros(m_szNumber);
	}
	else if (m_szNumber.size() == times)
	{
		m_szFraction.insert(0, m_szNumber);
		m_szNumber = "0";
	}
	else
	{
		m_szFraction.insert(0, m_szNumber);
		times -= m_szNumber.size();
		m_szFraction.insert(0, times, '0');
		m_szNumber = "0";
	}
	trimZeros(m_szFraction, true);
	roundOff(m_szNumber, m_szFraction, m_precision);
}

void BigNumber::increment()
{
	if (m_bNegative)
	{
		decrement(m_szNumber);
		trimZeros(m_szNumber);
		if (isEqual(0))
		{
			m_bNegative = false;
		}
	}
	else
	{
		increment(m_szNumber);
	}
}

void BigNumber::decrement()
{
	if (m_bNegative)
	{
		increment(m_szNumber);
	}
	else
	{
		if (isEqual(0))
		{
			m_bNegative = true;
		}
		decrement(m_szNumber);
		trimZeros(m_szNumber);
	}
}

void BigNumber::increment(ValueType& str) const
{
	int carry = 1;
	for (SizeType i = str.size(); i-- && carry;)
	{
		if (str[i] < '9' && str[i] >= '0')
		{
			str[i]++;
			carry = 0;
		}
		else
		{
			str[i] = '0';
		}
	}
	if (carry)
	{
		str.insert(0, 1, to_char(carry));
	}
}

void BigNumber::decrement(ValueType& str) const
{
	if (str.size() == 1 && str[0] == '0')
	{
		str[0] = '1';
		return;
	}
	int carry = 1;
	for (SizeType i = str.size(); i-- && carry;)
	{
		if (str[i] <= '9' && str[i] > '0')
		{
			str[i]--;
			carry = 0;
		}
		else
		{
			str[i] = '9';
		}
	}
}

bool BigNumber::isEven(const ValueType& str, const bool isFractionPart) const
{
	if (str.empty())
	{
		return false;
	}

	SizeType i = str.size() - 1;
	if (isFractionPart)
	{
		while (str[i] == '0')
		{
			i--;
		}
	}
	static const ValueType evenNums = "02468";
	return std::isdigit(str[i]) && (evenNums.find(str[i]) != ValueType::npos);
}

void BigNumber::split_at(const ValueType str, const SizeType digitCountInLow, ValueType& high, ValueType& low) const
{
	high = str.substr(0, str.size() - digitCountInLow);
	low = str.substr(str.size() - digitCountInLow);
}

std::pair<BigNumber::ValueType, BigNumber::ValueType> BigNumber::split_at(const ValueType& str, const CharType delimeter) const
{
	const SizeType pos = str.find(delimeter);
	if (pos != ValueType::npos)
	{
		return std::make_pair(str.substr(0, pos), str.substr(pos + 1));
	}
	return std::make_pair(str, "");
}

void BigNumber::makeEqualLength(ValueType& str1, ValueType& str2, const bool isFractionPart) const
{
	ValueType& small = str1.size() < str2.size() ? str1 : str2;
	ValueType& big = (small == str1) ? str2 : str1;
	if (isFractionPart)
	{
		small.insert(small.size(), big.size() - small.size(), '0');
		return;
	}
	small.insert(0, big.size() - small.size(), '0');
}

void BigNumber::trimZeros(ValueType& str, const bool isFractionPart) const
{
	if (isFractionPart)
	{
		str.erase(str.find_last_not_of('0') + 1, ValueType::npos);
		if (!str.empty() && str.back() == '.')
		{
			str.pop_back();
		}
		return;
	}
	str.erase(0, std::min(str.find_first_not_of('0'), str.size() - 1));
}

int BigNumber::compareStringAsNumber(const ValueType& str1, const ValueType& str2, const bool isFractionPart) const
{
	//0		if strings are equal
	//> 0	if the first non - matching character in str1 is greater(in ASCII) than that of str2.
	//< 0	if the first non - matching character in str1 is lower(in ASCII) than that of str2.
	// Should not contain leading or trailing zeros, trim before calling
	if (!isFractionPart)
	{
		if (str1[0] == '-' && str2[0] != '-')
		{
			return -1;
		}
		else if (str1[0] != '-' && str2[0] == '-')
		{
			return 1;
		}
		else if (str1[0] == '-' && str2[0] == '-')
		{
			return compareStringAsNumber(str1.substr(1), str2.substr(1), isFractionPart);
		}
		else if (str1 == "0" && str2.empty() || str1.empty() && str2 == "0")
		{
			return 0;
		}
		else if (str1.size() < str2.size())
		{
			return -1;
		}
		else if (str1.size() > str2.size())
		{
			return 1;
		}
		return strcmp(str1.c_str(), str2.c_str());
	}
	return strcmp(str1.c_str(), str2.c_str());
}

int BigNumber::compareStringAsNumber(const ValueType& str1, const ValueType& str2) const
{
	const std::pair<ValueType, ValueType> p1 = split_at(str1);
	const std::pair<ValueType, ValueType> p2 = split_at(str2);
	int cmpVal = compareStringAsNumber(p1.first, p2.first, false);
	if (cmpVal == 0)
	{
		cmpVal = compareStringAsNumber(p1.second, p2.second, true);
	}
	return cmpVal;
}

int BigNumber::fillUpDstForDivision(ValueType& dst, ValueType& numerator, const ValueType& denominator) const
{
	if (numerator.empty())
	{
		return -1;
	}
	if (dst == "0")
	{
		dst.clear();
	}
	SizeType insertCount = dst.size();
	if (insertCount < denominator.size())
	{
		insertCount = denominator.size() - insertCount;
		dst.insert(dst.size(), numerator.substr(0, insertCount));
		numerator.erase(0, insertCount);
	}
	int cmpVal = compareStringAsNumber(dst, denominator, false);
	while (cmpVal < 0 && !numerator.empty())
	{
		dst.push_back(numerator.front());
		numerator.erase(0, 1);
		cmpVal = compareStringAsNumber(dst, denominator, false);
	}
	return cmpVal;
}

void BigNumber::roundOff(ValueType& num, ValueType& frac, const SizeType precision) const
{
	if (precision == 0)
	{
		frac.clear();
	}
	else if (frac.size() > precision)
	{
		printf("Sent For Rounding = %s.%s\n", num.c_str(), frac.c_str());
		int carry = rounder(frac, 0, precision);
		while (carry--)
		{
			increment(num);
		}
		printf("Out From Rounding = %s.%s\n\n", num.c_str(), frac.c_str());
	}
}

void BigNumber::roundOff(ValueType& number, const SizeType precision) const
{
	SizeType pos = number.find('.');
	if (pos == ValueType::npos)
	{
		return;
	}
	if (precision == 0)
	{
		number.erase(pos);
		return;
	}
	if ((number.size() - pos - 1) > precision)
	{
		printf("Sent For Rounding = %s\n", number.c_str());
		int carry = rounder(number, pos + 1, precision);
		if (carry > 0)
		{
			number = addHelper(number, std::to_string(carry));
		}
		printf("Out From Rounding = %s\n\n", number.c_str());
	}
}

int BigNumber::rounder(ValueType& str, const SizeType pos, const SizeType precision) const
{
	static const ValueType down{ "01234" };
	static const ValueType up{ "6789" };

	static const ValueType odd{ "13579" };

	const CharType ch = str[pos + precision];
	int carry = 0;

	if (down.find(ch) != ValueType::npos)
	{
		// No changes to the ULP [unit at last position]
		carry = 0;
	}
	else if (up.find(ch) != ValueType::npos)
	{
		carry = 1;	
	}
	else
	{
		// If digit is 5
		// Banker's rounding / odd-even rounding
		const CharType prevChar = str[pos + precision - 1];
		if (odd.find(prevChar) != ValueType::npos)
		{
			carry = 1;
		}
	}

	str.erase(pos + precision);
	for (SizeType i = str.size(); i-- && str[i] != '.' && carry ;)
	{
		if (str[i] < '9')
		{
			str[i]++;
			carry = 0;
		}
		else
		{
			str[i] = '0';
		}
	}
	return carry;
}
}
#endif	// #ifndef __BIGNUMBER_H__