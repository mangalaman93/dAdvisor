#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <ctime>
#include <pthread.h>
#include <cstring>

#include "utils.h"
#include "../src/utils.cpp"

class UtilsTest : public CppUnit::TestFixture {
public:
  void setUp() {
  }

  void tearDown() {
  }

  void test_system_cmd() {
    string out;
    CPPUNIT_ASSERT(Utils::systemCmd("echo aman") == 0);
    CPPUNIT_ASSERT(Utils::systemCmd("echo aman", out) == 0);
    CPPUNIT_ASSERT(strcmp(out.c_str(), "aman") == 0);
    CPPUNIT_ASSERT(Utils::systemCmd("hello", out) == 127);
  }

  CPPUNIT_TEST_SUITE(UtilsTest);
  CPPUNIT_TEST(test_system_cmd);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(UtilsTest);

int main() {
   // Get the top level suite from the registry
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);

  // Run the tests.
  runner.run();
  return 0;
}
