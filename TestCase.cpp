#include "Foundation.h"
#include "TestCase.h"
#include "String/StringTestCase.h"
#include "Math/StringMatrix/StringMatrix.h"
#include "Algorithm/Queue/CircularQueue.h"
using namespace	PEIPEISPACE;

int main()
{
	auto stringTestCase = CStringTestCase();

	auto strMatrix = StringMatrix<4>{};

	strMatrix.Print();

	auto f = [](int a) {std::cout << a << std::endl; };
	CircularQueue<int, 10> q;
	for (int i = 0; i < 20; i++)
	{
		q.ForEach(f);
		std::cout << std::endl;
		q.Push(i);
	}
	const auto& cref_q = q;
	auto cH = cref_q.Head();
	auto cT = cref_q.Tail();

	stringTestCase();
	
	return 0;
}