#define _CRT_SECURE_NO_WARNINGS
#include "airhead.h"
#include "airhead.h"
#include <stdio.h>
#define SWEET_NUM_TESTS 512
#include "../sweet/sweet.h"

typedef struct test_t {
	int Int;
	float Float;
	char *String;
} test_t;

test_t vals[] = {
	{ 0xFF, -0.2f, "I, Andrew" },
	{ 0x00, 482.f,  "am the creator" },
	{ -12, 74.f, "of this library" },
	{ 0xFF, -0.2f, "I, Andrew" },
};
test_t new_val = { -64, 12.345f, "New value"};

test_t *InitVals() {
	test_t *Result = 0;
	arr_pusharray(Result, vals);
	return Result;
}

int main()
{
	ahd_int i = 0;

	TestGroup("Add/Remove/Access") {
		TestGroup("Push/Free") {
			test_t *arr = 0;
			test_t val = { 2, -3.4f, "Test String" };

			TestVEq(arr_len(arr), 0, "before push: %d");

			i = arr_push(arr, val);
			TestVEq(arr[i].Int,      2,     "%d");
			TestVEq(arr[i].Float,    -3.4f, "%f");
			TestStrEq(arr[i].String, "Test String");
			TestVEq(arr_len(arr), 1, "after push: %d");

			TestGroup("Add") {
				i = arr_add(arr, 3);
				TestVEq(i, 1, "after add: %d");
				TestVEq(arr_len(arr), 4, "after add: %d");
				arr[i++] = val;
				arr[i++] = val;
				arr[i]   = val;
				TestEq(arr[i], val);
			}

			TestGroup("Array") {
				i = arr_pusharray(arr, vals);
				TestVEq(i, 4, "after array push: %d");
				TestVEq(arr_len(arr), 8, "after array push: %d");
				Test(! memcmp(&arr[i], vals, sizeof(vals)/sizeof(*vals)));

				arr_scoped(test_t, arr2) {
					i = arr_pusharr(arr2, vals, 2);
					TestVEq(i, 0, "after arr push: %d");
					Test(! memcmp(&arr2[i], vals, 2 * sizeof(*vals)));

					i = arr_concat(arr, arr2);
					TestVEq(i, 8, "after concat: %d");
					Test(! memcmp(&arr[i], arr2, arr_len(arr2)));
				}
			}

			arr_free(arr);
			TestVEq("free" && arr, 0, "%ld");
			TestVEq(arr_len(arr), 0, "after free: %d");
		}

		TestGroup("Metadata") arr_scoped(test_t, arr) {
			TestGroup("Before valid array") {
				Test(arr_len(arr)          == 0);
				Test(arr_cap(arr)          == 0);
				Test(arr_size(arr)         == 0);
				Test(arr_totalsize(arr)    == 0);
				Test(arr_capsize(arr)      == 0);
				Test(arr_totalcapsize(arr) == 0);
			}

			arr_pusharray(arr, vals);
			TestVEq(sizeof(*arr_hdr(arr)), sizeof(ahd_arr), "%zd");

			TestVEq(arr_len(arr),  4, "%d");
			TestVEq(arr__len(arr), 4, "%d");
			TestVEq(arr_cap(arr),  4, "%d");
			TestVEq(arr__cap(arr), 4, "%d");


			TestVEq(arr__size(arr),         sizeof(vals),                      "%zd");
			TestVEq(arr__totalsize(arr),    sizeof(vals) + sizeof(ahd_arr),    "%zd");
			TestVEq(arr__capsize(arr),      4*sizeof(*vals),                   "%zd");
			TestVEq(arr__totalcapsize(arr), 4*sizeof(*vals) + sizeof(ahd_arr), "%zd");

			TestNote("Push one");
			arr_push(arr, new_val);
			TestVEq(arr_len(arr),  5, "%d");
			TestVEq(arr__len(arr), 5, "%d");
			TestVEq(arr_cap(arr),  8, "%d");
			TestVEq(arr__cap(arr), 8, "%d");
		}

		TestGroup("Insert/Remove") arr_scoped(test_t, arr) {
			arr_pusharray(arr, vals);

			test_t val1 = arr[1];
			arr_remove(arr, 0);
			TestEq(val1, arr[0]);

			i = arr_insert(arr, 1, new_val);
			TestVEq(i, 1, "%d");
			TestEq(arr[i], new_val);

			i = arr_insert(arr, arr_len(arr)-1, new_val);
			TestEq(arr[i], new_val);

			/* i = arr_insert(arr, arr_len(arr), new_val); */
			SkipTestVEq(i, arr_len(arr)-1, "insert at end: %d");

			ahd_int before_len = arr_len(arr);
			test_t lastval = arr_last(arr);
			arr_removeswap(arr, 2);
			ahd_int after_len = arr_len(arr);
			TestVEq(after_len, before_len-1, "%d");
			TestEq(lastval, arr[2]);
		}

		TestGroup("Clear/Reset Len") arr_scoped(test_t, arr) {
			arr_pusharray(arr, vals);
		
			TestVEq(arr_len(arr), 4, "%d");

			arr_resetlen(arr, 129);
			TestVEq(arr_len(arr), 129, "%d");

			arr_clear(arr);
			Test("Ptr should still be valid" && arr);
			TestVEq(arr_len(arr), 0, "%d");
		}

		TestGroup("Access") arr_scoped(test_t, arr) {
			arr_pusharray(arr, vals);
			TestEq(arr[3], arr_last(arr));
			int end = -1;

			TestNote("Bounds Checking");
			TestVEq(arr_bc(arr, 4),  0, "%d");
			TestVEq(arr_bc(arr, 68), 0, "%d");
			/*TestVEq(arr_bc(arr, -1), 0, "%d"); /* signed-unsigned mismatch */
			TestVEq(arr_bc(arr, 0),  1, "%d");
			TestVEq(arr_bc(arr, 1),  1, "%d");
			TestVEq(arr_bc(arr, 2),  1, "%d");
			TestVEq(arr_bc(arr, 3),  1, "%d");

			TestEq(arr_get(arr, end), arr_last(arr));
			TestEq(arr_get(arr, -3), arr_get(arr, 1));
		}

		TestGroup("Pop/Pull/Shift") arr_scoped(test_t, arr) {
			arr_pusharray(arr, vals);
			TestEq(arr[3], vals[3]);

			test_t val3 = arr[3];
			test_t popval = arr_pop(arr);
			TestEq(val3, popval);
			TestVEq(("after pop", arr_len(arr)), 3, "%d");

#if 0
			test_t val0 = arr[0];
			test_t val1 = arr[1];
			test_t shiftval = arr_pull(arr, 0);
			TestEq(val0, shiftval);
			TestVEq(("after shift", arr_len(arr)), 2, "%d");
#endif
		}
	}

	TestGroup("Array subsets") arr_scoped_init(test_t, base_arr, InitVals()) {
		TestGroup("Dup") {
			arr_scoped(test_t, arr) {
				Test(arr == 0);
				arr = (test_t *)arr_dup(base_arr);
				Test(arr);

				TestVEq(arr_len(arr), arr_len(base_arr), "%d");
			}

			TestNote("Init with dup");
			int scope_exit_successful = 1;
			test_t **arr_ptr = 0;
			arr_scoped_init(test_t, arr, (test_t *)arr_dup(base_arr)) {
				Test(arr);
				TestVEq(arr_len(arr), arr_len(base_arr), "%d");

				TestNote("Scope tests");
				arr_ptr = &arr;
				Test("Initially valid" && *arr_ptr); 

				arr_exitscope;
				/* scope_exit_successful = 0; */
			}
			Test(scope_exit_successful);
			// This is a little sketchy...
			Test("Seemingly destroyed" && *arr_ptr == 0); 
		}

		TestGroup("Sub") arr_scoped_init(test_t, arr, (test_t*)arr_sub(base_arr, 1, 2)) {
			TestVEq(arr_len(arr), 2, "%d");

			TestVEq(arr[0].Int,      vals[1].Int,   "%d");
			TestVEq(arr[0].Float,    vals[1].Float, "%f");
			TestStrEq(arr[0].String, vals[1].String);

			TestVEq(arr[1].Int,      vals[2].Int,   "%d");
			TestVEq(arr[1].Float,    vals[2].Float, "%f");
			TestStrEq(arr[1].String, vals[2].String);
		}
	}

#define TEST_VALX(arr, vals, x) do { \
	TestVEq(arr[x].Int,      vals[x].Int, "%d"); \
	TestVEq(arr[x].Float,    vals[x].Float, "%f"); \
	TestStrEq(arr[x].String, vals[x].String); \
} while(0)

#define TEST_VALS(arr, vals) do { \
	TEST_VALX(arr, vals, 0); \
	TEST_VALX(arr, vals, 1); \
	TEST_VALX(arr, vals, 2); \
	TEST_VALX(arr, vals, 3); \
} while (0)

	TestGroup("Array reordering") {
		TestGroup("Rotate") {
			TestGroup("rot1") arr_scoped_init(test_t, arr, InitVals()) {
				test_t rot1_vals[] = {
					{ 0xFF,  -0.2f, "I, Andrew" },
					{ 0xFF,  -0.2f, "I, Andrew" },
					{ 0x00, 482.f,  "am the creator" },
					{ -12,   74.f,  "of this library" },
				};
				arr_rotr(arr, 1);
				TEST_VALS(arr, rot1_vals);
			}

			TestGroup("rot2") arr_scoped_init(test_t, arr, InitVals()) {
				test_t rot2_vals[] = {
					{ -12,   74.f,  "of this library" },
					{ 0xFF,  -0.2f, "I, Andrew" },
					{ 0xFF,  -0.2f, "I, Andrew" },
					{ 0x00, 482.f,  "am the creator" },
				};
				arr_rotr(arr, 2);
				TEST_VALS(arr, rot2_vals);
			}
		}

		TestGroup("Sort")    arr_scoped_init(test_t, arr, InitVals()) {
			test_t sort_int_vals[] = {
				{ -12,   74.f,  "of this library" },
				{ 0x00, 482.f,  "am the creator" },
				{ 0xFF,  -0.2f, "I, Andrew" },
				{ 0xFF,  -0.2f, "I, Andrew" },
			};
			test_t sort_int_vals_desc[] = {
				{ 0xFF,  -0.2f, "I, Andrew" },
				{ 0xFF,  -0.2f, "I, Andrew" },
				{ 0x00, 482.f,  "am the creator" },
				{ -12,   74.f,  "of this library" },
			};

			test_t sort_float_vals[] = {
				{ 0xFF,  -0.2f, "I, Andrew" },
				{ 0xFF,  -0.2f, "I, Andrew" },
				{ -12,   74.f,  "of this library" },
				{ 0x00, 482.f,  "am the creator" },
			};

			test_t sort_uint_vals[] = {
				{ 0x00, 482.f,  "am the creator" },
				{ 0xFF,  -0.2f, "I, Andrew" },
				{ 0xFF,  -0.2f, "I, Andrew" },
				{ -12,   74.f,  "of this library" },
			};

			/* char buf[8*sizeof(sort_int_vals)]; */
			/* char buf2[8*sizeof(sort_int_vals)]; */
			/* for(i = 0; i < sizeof(sort_int_vals); ++i) { */
			/* 	sprintf(buf+i*3,  "%.2x   ", ((unsigned char *)arr)[i]); */
			/* 	sprintf(buf2+i*3, "%.2x   ", ((unsigned char *)sort_int_vals)[i]); */
			/* } */
			/* TestNote(buf); */
			/* TestNote(buf2); */

			arr_sorti(arr, &arr->Int, ahd_ASC);
			TestGroup("Sort by Int ascending") TEST_VALS(arr, sort_int_vals);

			arr_sortint(arr, &arr->Int, ahd_DESC);
			TestGroup("Sort by Int descending") TEST_VALS(arr, sort_int_vals_desc);

			arr_sortf(arr, &arr->Float, ahd_ASC);
			TestGroup("Sort by Float") TEST_VALS(arr, sort_float_vals);

			arr_sortu(arr, &arr->Int, ahd_ASC);
			TestGroup("Sort by (unsigned) Int ascending") TEST_VALS(arr, sort_uint_vals);
		}

		TestGroup("Reverse") {
			TestGroup("4") arr_scoped_init(test_t, arr, InitVals()) {
				test_t reversed_vals[] = {
					{ 0xFF, -0.2f, "I, Andrew" },
					{ -12, 74.f, "of this library" },
					{ 0x00, 482.f,  "am the creator" },
					{ 0xFF, -0.2f, "I, Andrew" },
				};

				arr_reverse(arr);
				TEST_VALS(arr, reversed_vals);
			}

			TestGroup("3") arr_scoped_init(test_t, arr, InitVals()) {
				test_t reversed_vals[] = {
					{ -12, 74.f, "of this library" },
					{ 0x00, 482.f,  "am the creator" },
					{ 0xFF, -0.2f, "I, Andrew" },
				};

				arr__len(arr) = 3;
				arr_reverse(arr);

				TEST_VALX(arr, reversed_vals, 0);
				TEST_VALX(arr, reversed_vals, 1);
				TEST_VALX(arr, reversed_vals, 2);
			}
		}
	}

	PrintTestResults(sweetCONTINUE);
}

