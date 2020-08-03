#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	_CrtSetBreakAlloc(154); // 154 is the memory block number.

	char* buffer = new char[1024];

	_CrtDumpMemoryLeaks();//退出前检查一下内存泄露情况  

	return 0;
}