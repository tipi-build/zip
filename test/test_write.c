#include <stdio.h>
#include <stdlib.h>

#include <zip.h>

#include "minunit.h"

static char ZIPNAME[L_tmpnam + 1] = {0};
static char WFILE[L_tmpnam + 1] = {0};

void test_setup(void) {
  strncpy(ZIPNAME, "z-XXXXXX\0", L_tmpnam);
  strncpy(WFILE, "w-XXXXXX\0", L_tmpnam);

  mktemp(ZIPNAME);
  mktemp(WFILE);
}

void test_teardown(void) {
  remove(WFILE);
  remove(ZIPNAME);
}

#define CRC32DATA1 2220805626
#define TESTDATA1 "Some test data 1...\0"

// Sat Sep 25 2021 10:26:20 GMT+0000
#define TESTDATA_TIME 1632565580

MU_TEST(test_write) {
  struct zip_t *zip = zip_open(ZIPNAME, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
  mu_check(zip != NULL);

  mu_assert_int_eq(0, zip_entry_open(zip, "test/test-1.txt"));
  mu_assert_int_eq(0, zip_entry_write(zip, TESTDATA1, strlen(TESTDATA1)));
  mu_assert_int_eq(0, strcmp(zip_entry_name(zip), "test/test-1.txt"));
  mu_assert_int_eq(0, zip_entry_index(zip));
  mu_assert_int_eq(strlen(TESTDATA1), zip_entry_size(zip));
  mu_check(CRC32DATA1 == zip_entry_crc32(zip));
  mu_assert_int_eq(0, zip_entry_close(zip));

  mu_assert_int_eq(0, zip_is64(zip));

  zip_close(zip);
}

MU_TEST(test_write_set_date) {
  struct zip_t *zip = zip_open(ZIPNAME, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
  mu_check(zip != NULL);

  mu_assert_int_eq(0, zip_entry_open(zip, "test/test-1.txt"));

  time_t zipentry_time = 1632565580;

  mu_assert_int_eq(0, zip_entry_write_set_time(zip, TESTDATA1, strlen(TESTDATA1), &zipentry_time));
  mu_assert_int_eq(0, strcmp(zip_entry_name(zip), "test/test-1.txt"));
  mu_assert_int_eq(0, zip_entry_index(zip));
  mu_assert_int_eq(strlen(TESTDATA1), zip_entry_size(zip));
  mu_check(CRC32DATA1 == zip_entry_crc32(zip));
  mu_assert_int_eq(0, zip_entry_close(zip));

  mu_assert_int_eq(0, zip_is64(zip));

  // TODO add api to get zip file info as well and test for that to be consistent

  zip_close(zip);
}

MU_TEST(test_fwrite) {
  const char *filename = WFILE;
  FILE *stream = NULL;
  struct zip_t *zip = NULL;
#if defined(_MSC_VER)
  if (0 != fopen_s(&stream, filename, "w+"))
#else
  if (!(stream = fopen(filename, "w+")))
#endif
  {
    // Cannot open filename
    mu_fail("Cannot open filename\n");
  }
  fwrite(TESTDATA1, sizeof(char), strlen(TESTDATA1), stream);
  mu_assert_int_eq(0, fclose(stream));

  zip = zip_open(ZIPNAME, 9, 'w');
  mu_check(zip != NULL);
  mu_assert_int_eq(0, zip_entry_open(zip, WFILE));
  mu_assert_int_eq(0, zip_entry_fwrite(zip, WFILE));
  mu_assert_int_eq(0, zip_entry_close(zip));
  mu_assert_int_eq(0, zip_is64(zip));

  zip_close(zip);
}

MU_TEST_SUITE(test_write_suite) {
  MU_SUITE_CONFIGURE(&test_setup, &test_teardown);


  MU_RUN_TEST(test_write_set_date);
  MU_RUN_TEST(test_write);
  MU_RUN_TEST(test_fwrite);
}

#define UNUSED(x) (void)x

int main(int argc, char *argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  MU_RUN_SUITE(test_write_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}