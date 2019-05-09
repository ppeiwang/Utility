
#pragma  once
namespace PEIPEISPACE
{
	template<typename T>
	T Mean(const T E_alpha, const T n, const T Xn)
	{
		T M = Xn;
		if (n > 1)
		{
			M = E_alpha + ((Xn - E_alpha) / n);
		}
		return M;
	}

	// 方差递推公式： 
	// 令F(n) = ∑（Xi-En)^2
	// F(n) = Fn-1 + (Xn -En-1)(Xn - En)
	// ==> F(n) = Var(n-1)*(n-1) + (Xn - En-1)(Xn - En)
	// ==> Var(n) = F(n) / n
	template<typename T>
	T Variance(const T V_alpha, const T E_alpha, const T E, const T n, const T Xn)
	{
		T Var = 0;
		if (n > 1)
		{
			const T Fn = (V_alpha * (n - 1)) + (Xn - E_alpha) * (Xn - E);
			Var = Fn / n;
		}

		return Var;
	}

}