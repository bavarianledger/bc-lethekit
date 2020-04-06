// Copyright © 2020 Blockchain Commons, LLC

#include <stdint.h>

#include "seed.h"
#include "util.h"

#include "selftest.h"

namespace selftest_internal {

uint8_t ref_secret[16] =
{
 0x8d, 0x96, 0x9e, 0xef, 0x6e, 0xca, 0xd3, 0xc2,
 0x9a, 0x3a, 0x62, 0x92, 0x80, 0xe6, 0x86, 0xcf
};

uint16_t ref_bip39_words_correct[BIP39Seq::WORD_COUNT] =
{
 0x046c, 0x05a7, 0x05de, 0x06ec,
 0x0569, 0x070a, 0x0347, 0x0262,
 0x0494, 0x0039, 0x050d, 0x04f1
};

uint16_t ref_bip39_words_bad_checksum[BIP39Seq::WORD_COUNT] =
{
 // third word is altered
 0x046c, 0x05a7, 0x0569, 0x06ec,
 0x0569, 0x070a, 0x0347, 0x0262,
 0x0494, 0x0039, 0x050d, 0x04f1
};

char* ref_bip39_mnemonics[BIP39Seq::WORD_COUNT] =
{
 "mirror", "reject", "rookie", "talk",
 "pudding", "throw", "happy", "era",
 "myth", "already", "payment", "owner"
};

size_t const ref_slip39_thresh = 2;
size_t const ref_slip39_nshares = 3;
char* ref_slip39_shares[ref_slip39_nshares] =
{
 "check academic academic acid counter "
 "both course legs visitor squeeze "
 "justice sack havoc elbow crunch "
 "scroll evoke civil exact vexed",
  
 "check academic academic agency custody "
 "purple ceiling walnut garlic hearing "
 "daughter kind critical necklace boundary "
 "dish away obesity glen infant",
  
 "check academic academic always check "
 "enemy fawn glimpse bulb rebound "
 "spelling plunge cover umbrella fused "
 "ocean desktop elephant beam aluminum"
};

uint16_t ref_slip39_words[ref_slip39_nshares][SLIP39ShareSeq::WORDS_PER_SHARE] =
{ 
 { 0x0088, 0x0000, 0x0000, 0x0001, 0x00a2,
   0x005d, 0x00a3, 0x0207, 0x03d8, 0x035a,
   0x01ee, 0x0307, 0x01aa, 0x0108, 0x00b0,
   0x031a, 0x0131, 0x008e, 0x0132, 0x03d1 },
 
 { 0x0088, 0x0000, 0x0000, 0x0011, 0x00b7,
   0x02ca, 0x0082, 0x03e0, 0x0181, 0x01af,
   0x00be, 0x01f2, 0x00ad, 0x0260, 0x005e,
   0x00e3, 0x0040, 0x026a, 0x018d, 0x01d2 },
 
 { 0x0088, 0x0000, 0x0000, 0x0021, 0x0088,
   0x011b, 0x0151, 0x018e, 0x006a, 0x02dc,
   0x034f, 0x02ab, 0x00a4, 0x03b4, 0x017c,
   0x026e, 0x00d3, 0x010d, 0x0043, 0x0020 }
};

uint16_t ref_slip39_share_bad_checksum[SLIP39ShareSeq::WORDS_PER_SHARE] =
{
 // ref_slip39_words[0], 11th word is altered
 0x0088, 0x0000, 0x0000, 0x0001, 0x00a2,
 0x005d, 0x00a3, 0x0207, 0x03d8, 0x035a,
 0x0132, 0x0307, 0x01aa, 0x0108, 0x00b0,
 0x031a, 0x0131, 0x008e, 0x0132, 0x03d1
};
 
// Clearly not random. Only use for tests.
void fake_random(uint8_t *buf, size_t count) {
    uint8_t b = 0;
    for(int i = 0; i < count; i++) {
        buf[i] = b;
        b = b + 17;
    }
}

bool test_failed(char *format, ...) {
  char buff[8192];
  va_list args;
  va_start(args, format);
  vsnprintf(buff, sizeof(buff), format, args);
  va_end(args);
  buff[sizeof(buff)/sizeof(buff[0])-1]='\0';
  Serial.print(buff);
  return false;
}

bool test_seed_generate() {
    serial_printf("test_seed_generate starting\n");
    Seed * seed = Seed::from_rolls("123456");
    Seed * seed0 = new Seed(ref_secret, sizeof(ref_secret));
    if (*seed != *seed0)
        return test_failed("test_seed_generate failed: seed mismatch\n");
    delete seed0;
    delete seed;
    serial_printf("test_seed_generate finished\n");
    return true;
}

bool test_bip39_generate() {
    serial_printf("test_bip39_generate starting\n");
    Seed * seed = Seed::from_rolls("123456");
    BIP39Seq * bip39 = new BIP39Seq(seed);
    for (size_t ii = 0; ii < BIP39Seq::WORD_COUNT; ++ii) {
        if (bip39->get_word(ii) != ref_bip39_words_correct[ii])
            return test_failed("test_bip39_generate failed: word mismatch\n");
        if (strcmp(bip39->get_string(ii), ref_bip39_mnemonics[ii]) != 0)
            return test_failed("test_bip39_generate failed: mnemonic mismatch\n");
    }
    delete bip39;
    delete seed;
    serial_printf("test_bip39_generate finished\n");
    return true;
}

bool test_bip39_restore() {
    serial_printf("test_bip39_restore starting\n");
    BIP39Seq * bip39 = BIP39Seq::from_words(ref_bip39_words_correct);
    Seed * seed = bip39->restore_seed();
    if (!seed)
        return test_failed("test_bip39_restore failed: restore failed\n");
    Seed * seed0 = Seed::from_rolls("123456");
    if (*seed != *seed0)
        return test_failed("test_bip39_restore failed: seed mismatch\n");
    delete seed0;
    delete seed;
    delete bip39;
    serial_printf("test_bip39_restore finished\n");
    return true;
}

bool test_bip39_bad_checksum() {
    serial_printf("test_bip39_bad_checksum starting\n");
    BIP39Seq * bip39 = BIP39Seq::from_words(ref_bip39_words_bad_checksum);
    Seed * seed = bip39->restore_seed();
    if (seed)
        return test_failed(
            "test_bip39_bad_checksum failed: restore verify passed\n");
    delete seed;
    delete bip39;
    serial_printf("test_bip39_bad_checksum finished\n");
    return true;
}

bool test_slip39_generate() {
    serial_printf("test_slip39_generate starting\n");
    Seed * seed = Seed::from_rolls("123456");
    SLIP39ShareSeq * slip39 =
        SLIP39ShareSeq::from_seed(seed, ref_slip39_thresh,
                                  ref_slip39_nshares, fake_random);
    for (size_t ii = 0; ii < ref_slip39_nshares; ++ii) {
        uint16_t const * words = slip39->get_share(ii);
        char * strings = 
            slip39_strings_for_words(words, SLIP39ShareSeq::WORDS_PER_SHARE);
        if (strcmp(strings, ref_slip39_shares[ii]) != 0)
            return test_failed("test_slip39_generate failed: share mismatch\n");
        free(strings);
    }
    delete slip39;
    delete seed;
    serial_printf("test_slip39_generate finished\n");
    return true;
}

bool test_slip39_restore() {
    serial_printf("test_slip39_restore starting\n");
    SLIP39ShareSeq * slip39 = new SLIP39ShareSeq();
    slip39->add_share(ref_slip39_words[2]);
    slip39->add_share(ref_slip39_words[1]);
    Seed * seed = slip39->restore_seed();
    if (!seed)
        return test_failed("test_slip39_restore failed: restore failed\n");
    Seed * seed0 = Seed::from_rolls("123456");
    if (*seed != *seed0)
        return test_failed("test_slip39_restore failed: seed mismatch\n");
    delete seed0;
    delete seed;
    delete slip39;
    serial_printf("test_slip39_restore finished\n");
    return true;
}

bool test_slip39_verify_share_valid() {
    serial_printf("test_slip39_verify_share_valid starting\n");
    bool ok = SLIP39ShareSeq::verify_share_checksum(ref_slip39_words[0]);
    if (!ok)
        return test_failed("test_slip39_verify_share_valid failed: invalid\n");
    serial_printf("test_slip39_verify_share_valid finished\n");
    return true;
}

bool test_slip39_verify_share_invalid() {
    serial_printf("test_slip39_verify_share_invalid starting\n");
    bool ok =
        SLIP39ShareSeq::verify_share_checksum(ref_slip39_share_bad_checksum);
    if (ok)
        return test_failed("test_slip39_verify_share_invalid failed: valid\n");
    serial_printf("test_slip39_verify_share_invalid finished\n");
    return true;
}

bool test_slip39_del_share() {
    serial_printf("test_slip39_del_share starting\n");
    SLIP39ShareSeq * slip39 = new SLIP39ShareSeq();
    slip39->add_share(ref_slip39_words[2]);
    slip39->add_share(ref_slip39_words[0]);	// this will get deleted
    slip39->add_share(ref_slip39_words[1]);

    // delete the middle share
    slip39->del_share(1);
    if (slip39->numshares() != 2)
        return test_failed("test_slip39_del_share failed: bad numshares\n");

    // add and delete the last share
    slip39->add_share(ref_slip39_words[0]);	// this will get deleted
    slip39->del_share(2);
    if (slip39->numshares() != 2)
        return test_failed("test_slip39_del_share failed: bad numshares\n");
    
    Seed * seed = slip39->restore_seed();
    if (!seed)
        return test_failed("test_slip39_del_share failed: restore failed\n");
    Seed * seed0 = Seed::from_rolls("123456");
    if (*seed != *seed0)
        return test_failed("test_slip39_del_share failed: seed mismatch\n");
    delete seed0;
    delete seed;
    delete slip39;
    serial_printf("test_slip39_del_share finished\n");
    return true;
}

struct selftest_t {
    char const * testname;
    bool (*testfun)();
};

selftest_t g_selftests[] =
{
 // Max test name display length is ~16 chars.
 // |--------------|
 { "seed generate", test_seed_generate },
 { "BIP39 generate", test_bip39_generate },
 { "BIP39 restore", test_bip39_restore },
 { "SLIP39 generate", test_slip39_generate },
 { "SLIP39 restore", test_slip39_restore },
 { "BIP39 restore", test_bip39_restore },
 { "BIP39 bad chksum", test_bip39_bad_checksum },
 { "SLIP39 share ok", test_slip39_verify_share_valid },
 { "SLIP39 share bad", test_slip39_verify_share_invalid },
 { "SLIP39 del share", test_slip39_del_share },
 // not enough shares
 // too many shares
 // same share twice
 // foreign share
 // |--------------|
};

size_t const g_numtests = sizeof(g_selftests) / sizeof(*g_selftests);

} // namespace selftest_internal

// deprecated, calling these from userinterface now
#if 0
void selftest() {
    using namespace selftest_internal;
    serial_printf("self_test starting\n");
    test_seed_generate();
    test_bip39_generate();
    test_bip39_restore();
    test_slip39_generate();
    test_slip39_restore();
    serial_printf("self_test finished\n");
}
#endif

const uint16_t * selftest_dummy_bip39() {
    using namespace selftest_internal;
    return ref_bip39_words_correct;
}

const uint16_t * selftest_dummy_slip39(size_t ndx) {
    using namespace selftest_internal;
    if (ndx > ref_slip39_nshares - 1)
        ndx = ref_slip39_nshares - 1;
    return ref_slip39_words[ndx];
}

size_t selftest_numtests() {
    using namespace selftest_internal;
    return g_numtests;
}

String selftest_testname(size_t ndx) {
    using namespace selftest_internal;
    serial_assert(ndx < g_numtests);
    return g_selftests[ndx].testname;
}

bool selftest_testrun(size_t ndx) {
    using namespace selftest_internal;
    serial_assert(ndx < g_numtests);
    g_selftests[ndx].testfun();
}
