#pragma once

namespace ublas = boost::numeric::ublas;

namespace velocity {

	template<class T1, class T2>
	struct scalar_max: public ublas::scalar_binary_functor<T1, T2> {
		typedef typename ublas::scalar_binary_functor<T1, T2>::argument1_type argument1_type;
		typedef typename ublas::scalar_binary_functor<T1, T2>::argument2_type argument2_type;
		typedef typename ublas::scalar_binary_functor<T1, T2>::result_type result_type;

		static BOOST_UBLAS_INLINE
		result_type apply (argument1_type t1, argument2_type t2) {
			return std::max<T1>(t1,t2);
		}
	};

	template<class T1, class E2>
	BOOST_UBLAS_INLINE
	typename ublas::vector_binary_scalar1_traits<const T1, E2, scalar_max<T1, typename E2::value_type> >::result_type
	max( const T1 &e1, const ublas::vector_expression<E2> &e2) {
		typedef typename ublas::vector_binary_scalar1_traits<const T1, E2, scalar_max<T1, typename E2::value_type> >::expression_type expression_type;
		return expression_type (e1, e2 ());
	}

	template<class E1, class T2>
	BOOST_UBLAS_INLINE
	typename ublas::vector_binary_scalar2_traits<E1, const T2, scalar_max<typename E1::value_type, T2> >::result_type
	max(const ublas::vector_expression<E1> &e1, const T2 &e2) {
		typedef typename ublas::vector_binary_scalar2_traits<E1, const T2, scalar_max<typename E1::value_type, T2> >::expression_type expression_type;
		return expression_type (e1 (), e2);
	}

	template<typename T>
	BOOST_UBLAS_INLINE
	const T& max(const T& e1, const T& e2) {
		return std::max<T>(e1,e2);
	}


	template<class T1, class T2>
	struct scalar_lt:
		public ublas::scalar_binary_functor<T1, T2> {
		typedef typename ublas::scalar_binary_functor<T1, T2>::argument1_type argument1_type;
		typedef typename ublas::scalar_binary_functor<T1, T2>::argument2_type argument2_type;
		typedef typename ublas::scalar_binary_functor<T1, T2>::result_type result_type;

		static BOOST_UBLAS_INLINE
		result_type apply (argument1_type t1, argument2_type t2) {
			return t1 < t2;
		}
	};

	// (v1 < v2) [i] = v1 [i] < v2 [i]
	template<class E1, class E2>
	BOOST_UBLAS_INLINE
	typename ublas::vector_binary_traits<E1, E2, scalar_lt<typename E1::value_type, 
													typename E2::value_type> >::result_type
	operator < (const ublas::vector_expression<E1> &e1,
				const ublas::vector_expression<E2> &e2) {
		typedef typename ublas::vector_binary_traits<E1, E2, scalar_lt<typename E1::value_type,
																			  typename E2::value_type> >::expression_type expression_type;
		return expression_type (e1 (), e2 ());
	}

	// (t < v2) [i] = t < v2 [i]
	template<class T1, class E2>
	BOOST_UBLAS_INLINE
	typename ublas::vector_binary_scalar1_traits<const T1, E2, scalar_lt<T1, typename E2::value_type> >::result_type
	operator < ( const T1 &e1, const ublas::vector_expression<E2> &e2) {
		typedef typename ublas::vector_binary_scalar1_traits<const T1, E2, scalar_max<T1, typename E2::value_type> >::expression_type expression_type;
		return expression_type (e1, e2 ());
	}

	// (v1 < t) [i] = v1[i] < t
	template<class E1, class T2>
	BOOST_UBLAS_INLINE
	typename ublas::vector_binary_scalar2_traits<E1, const T2, scalar_lt<typename E1::value_type, T2> >::result_type
	operator < (const ublas::vector_expression<E1> &e1, const T2 &e2) {
		typedef typename ublas::vector_binary_scalar2_traits<E1, const T2, scalar_max<typename E1::value_type, T2> >::expression_type expression_type;
		return expression_type (e1 (), e2);
	}

	// Vector-Vector ops need special functions to prevent template errors with Vector-Scalar
	/// THREAD THESE!!!!! Also, too many copy constructor calls.

	inline
	bool boolean(const VDouble& v)
	{
		return (v(0)==0.0)?false:true;
	}

	inline
	bool boolean(const double& d)
	{
		return d;
	}

	inline
	bool boolean(const bool& b)
	{
		return b;
	}
	//---------------------------PROD-------------------------------------------------------


	// (v1 * v2) [i] = v1 [i] * v2 [i]
	inline 
	VDouble
	prod(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);
		vdMul(size,&v1[0],&v2[0], &y[0]);

		return y;
	}


	inline
	VDouble
	prod(const VDouble& v1, const double& t) {
		VDouble y=v1;
		blas::scal(t, y);
		return y;
	}

	inline
	VDouble
	prod(const double& t, const VDouble& v1) {
		VDouble y=v1;
		blas::scal(t, y);
		return y;
	}

	inline
	const double
	prod(const double& t1, const double& t2) {
		return t1*t2;
	}

	//---------------------------ADD-------------------------------------------------------

	inline
	VDouble
	add(const VDouble& v1, const double& t) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) + t;
		return y;
	}

	inline
	VDouble
	add(const double& t, const VDouble& v1) {
		return add(v1,t);
	}

	inline
	VDouble
	add(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);

		vdAdd(size,&v1[0],&v2[0], &y[0]);

		return y;
	}

	inline
	const double
	add(const double& t1, const double& t2) {
		return t1+t2;
	}


	//---------------------------SUB-------------------------------------------------------

	inline
	VDouble
	sub(const VDouble& v1, const double& t) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) - t;
		return y;
	}

	inline
	VDouble
	sub(const double& t, const VDouble& v1) {
		return sub(v1,t);
	}

	inline
	VDouble
	sub(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);

		vdSub(size,&v1[0],&v2[0], &y[0]);

		return y;
	}
	inline
	const double
	sub(const double& t1, const double& t2) {
		return t1-t2;
	}


	//---------------------------DIV-------------------------------------------------------

	inline
	VDouble
	div(const VDouble& v1, const double& t) {
		assert(t!=0.0);
		VDouble y=v1;
		blas::scal(1/t, y);
		return y;
	}

	VDouble
	div(const VDouble& v1, const VDouble& v2) {
		VDouble temp(v1.size());
		vdDiv(v1.size(),&v1[0],&v2[0], &temp[0]);
		return temp;
	}

	//VDouble
	//max(const VDouble& v1, const double& t)
	//{
	//	int size = v1.size();
	//	VDouble temp(size);
	//	for (int i=0; i<size; i++)
	//		temp(i) = std::max(v1(i),t);
	//	return temp;
	//}

	//----------------------------LT------------------------------------------------------

	// (v1 * v2) [i] = v1 [i] < v2 [i]
	inline
	VDouble
	lt(const VDouble& v1, const double& t) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) < t;
		return y;
	}

	inline
	VDouble
	lt(const double& t, const VDouble& v1) {
		return lt(v1,t);
	}

	inline
	VDouble
	lt(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) < v2(i);
		return y;
	}

	inline
	bool
	lt(const double& t1, const double& t2) {
		return t1<t2;
	}
	//----------------------------LTE------------------------------------------------------
	// (v1 * v2) [i] = v1 [i] < v2 [i]
	inline
	VDouble
	lte(const VDouble& v1, const double& t) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) <= t;
		return y;
	}

	inline
	VDouble
	lte(const double& t, const VDouble& v1) {
		return lte(v1,t);
	}

	inline
	VDouble
	lte(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) <= v2(i);
		return y;
	}

	inline
	bool
	lte(const double& t1, const double& t2) {
		return t1<=t2;
	}

	//----------------------------EQ------------------------------------------------------

	// (v1 == v2) [i] = v1 [i] == v2 [i]
	inline
	VDouble
	eq(const VDouble& v1, const double& t) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) == t;
		return y;
	}

	inline
	VDouble
	eq(const double& t, const VDouble& v1) {
		return eq(v1,t);
	}	

	inline
	VDouble
	eq(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) == v2(i);
		return y;
	}

	inline
	bool
	eq(const double& t1, const double& t2) {
		return t1==t2;
	}

	//-------------------------NEQ---------------------------------------------------------

	// (v1 == v2) [i] = v1 [i] == v2 [i]
	inline
	VDouble
	neq(const VDouble& v1, const double& t) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) != t;
		return y;
	}

	inline
	VDouble
	neq(const double& t, const VDouble& v1) {
		return neq(v1,t);
	}	

	inline
	VDouble
	neq(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) != v2(i);
		return y;
	}

	inline
	bool
	neq(const double& t1, const double& t2) {
		return t1!=t2;
	}

	//--------------------------GT--------------------------------------------------------


	// (v1 * v2) [i] = v1 [i] < v2 [i]
	inline
	VDouble
	gt(const VDouble& v1, const double& t1) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) > t1;
		return y;
	}

	inline
	VDouble
	gt(const double& t1, const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) > t1;
		return y;
	}

	inline
	VDouble
	gt(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) > v2(i);
		return y;
	}


	bool
	gt(const double& t1, const double& t2) {
		return t1>t2;
	}

	//--------------------------GTE--------------------------------------------------------


	// (v1 * v2) [i] = v1 [i] < v2 [i]
	inline
	VDouble
	gte(const VDouble& v1, const double& t1) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) >= t1;
		return y;
	}

	inline
	VDouble
	gte(const double& t1, const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) >= t1;
		return y;
	}

	inline
	VDouble
	gte(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);
		for (int i=0; i<size; i++)
			y(i) = v1(i) >= v2(i);
		return y;
	}


	bool
	gte(const double& t1, const double& t2) {
		return t1>=t2;
	}

	//----------------------------------------------------------------------------------

	inline
	void
	assign(VDouble& v1, const VDouble& v2)
	{
		v1 = v2;
	}

	inline
	void
	assign(VDouble& v1, const double& t1)
	{
		v1.resize(1);
		v1(0)=t1;
	}

	void
	inline
	assign(double* t1, const VDouble& v)
	{
		memcpy(t1,&v(0),v.size()*sizeof(double));
	}

	// Too expensive? Maybe change to expanded assignment
	void
	inline
	assign(double& t1, const double t2)
	{
		t1=t2;
	}

	void
	inline
	assign(VDouble& v, const int size, const double t1, ...)
	{
		va_list ap;
		va_start(ap,t1);

		boost::shared_array<double> ts(new double[size]);
		ts[0]=t1;

		for (int i=1; i < size; i++)
			ts[i]=va_arg(ap,double);

		array_adaptor aa(size, ts.get());
		v = VDouble(size, aa);

		va_end(ap);
	}

	inline
	VDouble
	sqrt(const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);

		vdSqrt(size, &v1[0], &y[0]);
		return y;
	}

	inline
	VDouble
	exp(const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);

		vdExp(size, &v1[0], &y[0]);
		return y;
	}

	inline
	VDouble
	log(const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);

		vdLn(size, &v1[0], &y[0]);
		return y;
	}

	inline
	VDouble
	pow(const VDouble& v1, const VDouble& v2) {
		assert(v1.size() == v2.size());
		int size=v1.size();
		VDouble y(size);

		vdPow(size, &v1[0], &v2[0], &y[0]);
		return y;
	}

	inline
	VDouble
	pow(const VDouble& v1, const double& t1) {
		int size=v1.size();
		VDouble y(size);

		vdPowx(size, &v1[0], t1, &y[0]);
		return y;
	}

	inline
	double
	pow(const double t1, const double t2) {
		return std::pow(t1,t2);
	}

	inline
	VDouble
	sin(const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);

		vdSin(size, &v1[0], &y[0]);
		return y;
	}

	inline
	VDouble
	cos(const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);

		vdCos(size, &v1[0], &y[0]);
		return y;
	}

	inline
	VDouble
	tan(const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);

		vdTan(size, &v1[0], &y[0]);
		return y;
	}

	inline
	VDouble
	phi(const VDouble& v1) {
		int size=v1.size();
		VDouble y(size);

	#ifdef _UBLAS_BINDING
		blas::scal(M_SQRT1_2, y);
	#else
		blas::scal(y, M_SQRT1_2);
	#endif
		vdErf(size, &y[0], &y[0]);
	#ifdef _UBLAS_BINDING
		blas::scal(0.5, y);
	#else
		blas::scal(y, 0.5);
	#endif
		return y;
	}

	inline
	VDouble
	sequence(double start, double end, double incr=1) {
		long size = (long)ceil((start-end)/incr);
		VDouble var(size);
		int j=0;
		for (double i=start; i<=end; i+=incr)
		   var(j++)=i;
		return var;
	}

}