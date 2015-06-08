#include "test.h"
#include "restitutionTest.h"
#include "HierarchieTest.h"
#include "StressTest.h"
#include "RayMarcherTest.h"
#include "FilterTest.h"
#include "ContinuosTest.h"
#include "myMath.h"

#undef main

int main()
{

	Test* test = new ContinuousTest();
	test->init();

	test->run();


	return 0;
}