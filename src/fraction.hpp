// GENERIC EUCLIDEAN ALGORITHM FOR TYPES WITH %
template<typename T> inline T gcd (T a, T b)
{
	T c;
	while (b != 0) {
		c = a % b;
		a = b;
		b = c;
	}

	return a;
}

// GENERIC FRACTION TYPE
template<typename T> struct fraction {
	T num, denom;

	fraction(T Num, T Denom=1)
	{
		T g = gcd(Num, Denom);

		num = Num / g;
		denom = Denom / g;
	};

	fraction<T> operator -() const {return fraction<T>(-num, denom);}
	operator bool() const {return (num != 0);}
};

template<typename T>
fraction<T> operator +(const fraction<T> &a, const fraction<T> &b)
{
    return fraction<T>(a.num * b.denom + b.num * a.denom, a.denom * b.denom);
}

template<typename T>
fraction<T> operator -(const fraction<T> &a, const fraction<T> &b)
{
    return fraction<T>(a.num * b.denom - b.num * a.denom, a.denom * b.denom);
}

template<typename T>
fraction<T> operator *(const fraction<T> &a, const fraction<T> &b)
{
    return fraction<T>(a.num * b.num, a.denom * b.denom);
}

template<typename T>
fraction<T> operator /(const fraction<T> &a, const fraction<T> &b)
{
	return fraction<T>(a.num * b.denom, a.denom * b.num);
}

template<typename T>
bool operator ==(const fraction<T> &a, const fraction<T> &b)
{
	return a.num * b.denom == a.denom * b.num;
}
