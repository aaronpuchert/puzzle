// GENERIC FRACTION TYPE
template<typename T> struct fraction {
	T num, denom;

	fraction(T Num, T Denom=1) : num(Num), denom(Denom) {}

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
