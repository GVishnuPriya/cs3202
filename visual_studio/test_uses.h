#pragma once
#ifndef test_uses_h
#define test_uses_h
//#include <cppunit/extensions/HelperMacros.h>

class TestUses : public CPPUNIT_NS::TestFixture // Note 2 
{ 
	CPPUNIT_TEST_SUITE( TestUses ); // Note 3 
	CPPUNIT_TEST ( testinsertUses );	//Sample: to delete later
	CPPUNIT_TEST (single_varible_no_variables_use);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	// method to test the constructor
	//void testConstructor();

	// method to test the assigning and retrieval of grades
	void testinsertUses();	//Sample: to change later
};
#endif
