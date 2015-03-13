#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <ctime>
#include <pthread.h>

#include "utils.h"
#include "lprocess.h"
#include "../src/utils.cpp"
#include "../src/guest.cpp"
#include "../src/lprocess.cpp"

class LProcessTest : public CppUnit::TestFixture {
public:
  void setUp() {
  }

  void tearDown() {
    Utils::systemCmd("killall -9 iperf");
  }

  static void *runServer(void *arg) {
    Utils::systemCmd("iperf -s");
  }

  void testSetNetworkInBW() {
    // doesn't work if filter <match ip dport 5001 0xffff> is added
    LProcess *lp = new LProcess("iperf", "lo", "", 123);

    // setting up incoming bandwidth to be 4KB/s
    lp->setNetworkInBW(4*1024);

    // creating server
    pthread_t server;
    int rc;
    rc = pthread_create(&server, NULL, &(LProcessTest::runServer), 0);
    assert(rc == 0);

    // running client
    string result;
    Utils::systemCmd("iperf -c localhost", result, 0);

    // grep-ing bandwidth and comparing
    stringstream ss;
    ss<<"echo \""<<result<<"\" | "<<"awk '{print $(NF-1)}'";
    string bw_str;
    Utils::systemCmd(ss.str(), bw_str, 0);
    float bw = stof(bw_str);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(bw/8, 4, 0.5);

    // second test
    lp->setNetworkInBW(2*1024);
    result.clear();
    Utils::systemCmd("iperf -c localhost", result, 0);

    // grep-ing bandwidth and comparing
    ss.str("");
    ss<<"echo \""<<result<<"\" | "<<"awk '{print $(NF-1)}'";
    bw_str.clear();
    Utils::systemCmd(ss.str(), bw_str, 0);
    bw = stof(bw_str);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(bw/8, 2, 0.5);

    // clean up
    Utils::systemCmd("killall -9 iperf");
    rc = pthread_join(server, NULL);
    assert(rc == 0);
    delete lp;
  }

  CPPUNIT_TEST_SUITE(LProcessTest);
  CPPUNIT_TEST(testSetNetworkInBW);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(LProcessTest);

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
