TRIBITS_INCLUDE_DIRECTORIES(REQUIRED_DURING_INSTALLATION_TESTING ${CMAKE_CURRENT_SOURCE_DIR}/../MVOPTester)

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  Factory
  SOURCES
  Factory.cpp
  ${TEUCHOS_STD_UNIT_TEST_MAIN}
  )
