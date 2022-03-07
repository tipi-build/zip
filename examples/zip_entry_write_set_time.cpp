
#include <iostream>
#include "zip.h"

#define TESTDATA1 "Some test data 1...\0"

int main() {
    struct zip_t *zip = zip_open("test.zip", ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    zip_entry_open(zip, "test/test-1.txt");

    time_t zipentry_time = 1622026718;
    zip_entry_write_set_time(zip, TESTDATA1, strlen(TESTDATA1), &zipentry_time);
    zip_entry_close(zip);
    // TODO add api to get zip file info as well and test for that to be consistent

    zip_close(zip);
    return 0;
}