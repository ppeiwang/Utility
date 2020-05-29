#include "Foundation.h"
#include "TestCase.h"
#include "String/StringTestCase.h"
#include "Math/StringMatrix/StringMatrix.h"

using namespace	PEIPEISPACE;

int main()
{
	auto stringTestCase = CStringTestCase();

	auto strMatrix = StringMatrix<4>{};

	strMatrix.Print();

	stringTestCase();
	
	return 0;
}