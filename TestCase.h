#pragma once
class CTestCase
{
public:
	CTestCase() = default;

	virtual ~CTestCase() = default;

	virtual void operator()() = 0;
};
