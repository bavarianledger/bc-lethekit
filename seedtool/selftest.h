// Copyright © 2020 Blockchain Commons, LLC

#ifndef SELFTEST_H
#define SELFTEST_H

void selftest();

// Used to run selftests from the UI.
size_t selftest_numtests();
String selftest_testname(size_t ndx);
bool selftest_testrun(size_t ndx);

// Used to populate dummy data in UI testing.
const uint16_t * selftest_dummy_bip39();
const uint16_t * selftest_dummy_slip39(size_t ndx);
const uint16_t * selftest_dummy_slip39_alt(size_t ndx);

#endif // SELFTEST_H
