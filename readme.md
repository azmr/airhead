**NOTE: unless specified, the first argument of all macros is the type of the header.**
I have omitted them here to reduce noise.
This is intended for you to redefine with the header type of your choice.
As an example, the included redefinitions are in the format:
```c
// library definition
#define ahd_push(header_type, array, value) ...

// useful redefinition
#define arr_push(array, value) ahd_push(ahd_arr, array, value)
```

## Initialising arrays

Create a null pointer. This will be assigned to heap memory when the array is pushed/added to.

``` c
char **Strings = 0;
int *Nums = NULL;
struct int_float {int i; float f} *Data = 0;
```

## Adding and removing elements
| `arr_push(array, value)`      | append value to the end of the array, growing (or creating) the array if needed                                          |
| `arr_add(array, n)`           | add space for `n` more items on the array, growing (or creating) if needed                                               |
| `arr_insert(array, i, value)` | inserts value into the array at index i, moving elements from position i upwards along 1 to make space                   |
|                               |                                                                                                                          |
| `arr_remove(array, i)`        | removes the element at index i, shifting the higher elements down into its place                                         |
| `arr_pull(array, i)`          | applies arr_remove, but also returns the value. This fails if wrapped in parens: `x = (arr_pull(vals, i))`               |
| `arr_shift(array)`            | removes and returns the first element of the array, shifting the other elements down into its place                      |
| `arr_pop(array)`              | removes and returns the last element of the array                                                                        |
|                               |                                                                                                                          |
| `arr_clear(array)`            | set the length of array to 0                                                                                             |
|                               |                                                                                                                          |
| `arr_free(array)`             | frees the array contents; also sets the array ptr back to NULL, causing access to fail. The array can be pushed to again |
| `arr_free2d(array)`           | frees 2d/jagged arrays - all of the subarrays are freed, then the containing array                                       |
| `arr_free2dt(ht2, array)`     | as above, but for when the inner arrays have a different header type                                                     |


## Array header/metadata

| `arr_hdr(array)`       | 'returns' a pointer to the header, which you can then use like a normal struct             |
| `arr_len(array)`       | number of elements in the array. Safe to pass `NULL`.                                      |
| `arr_cap(array)`       | number of possible elements in the array before it needs to grow. Safe to pass `NULL`.     |
| `arr_count(array)`     | (alias for `ahd_len` (kept for compatibility with `stb_stretchybuffer.h`                   |
| `arr__len(array)`      | number of elements in the array. Not safe to pass `NULL`.                                  |
| `arr__cap(array)`      | number of possible elements in the array before it needs to grow. Not safe to pass `NULL`. |
|                        |                                                                                            |
| `arr_size(array)`      | number of bytes in the array (excluding the header) for `len` elements                     |
| `arr_totalsize(array)` | number of bytes in the array (including the header)                                        |
|                        |                                                                                            |
|                        |                                                                                            |
|                        |

## Extracting subsets
| `arr_last(array)`             | expands to an l-value of the last element in the array                                                                                       |
|

## Ordering

| `arr_reverse`                 | reverse the order of the elements in the array                                                                                               |
| `arr_rotr`                    | move each element to the next index. The last element moves to the first position                                                            |
| `arr_rotl`                    | move each element to the previous index. The first element moves to the last position                                                        |

| `arr_dup(header_type, array)` | duplicate the array (both will need to be `free`d at some point)                                                                              |
| `arr_unique`                  |                                                                                                                                              |

#define ahd_slice()
#define ahd_bc()
#define ahd_sort(header_type, array,t,dir)
// Should this free the original?
#define ahd_append(header_type, array,src,len,size) (AHD_MEMCPY((a)+ahd_add(header_type, array,len), (src), (size)), )
#define ahd_cat(header_type, array,b)           ahd_append(ht, a, b,   ahd_len(ht, b),             ahd_size(b))
#define ahd_pusharr(header_type, array,arr,els) ahd_append(ht, a, arr, els,                        (els)*sizeof(*(a)) )
#define ahd_pusharray(header_type, array,arr)   ahd_append(ht, a, arr, sizeof(arr)/sizeof(*(arr)), sizeof(arr))

#define ahd_foronce ahd_n_ln = 0; ! ahd_n_ln

#ifdef AHD_NO_DECLS
#define ahd_decl(t)
#else
#define ahd_decl(t) t
#endif

#define ahd_resetlen(header_type, array,n) (ahd_clear(header_type, array), ahd_add(header_type, array,n))
// statement
// should this create new array? map to 2nd array? change in place?
#define ahd_mapt(ht,i,ta,a,va,tb,b,vb) if(a) \
			for(ahd_decl(ahd_int) i = ((a) == (b) || ahd_resetlen(ht, b, ht__len(a)), 0), ahd_foronce;) \
			for(ahd_decl(tb) vb = {0}; ! ahd_n_ln++; ) \
			for(ahd_decl(ta) va = (a)[i]; i < ahd__len(a); (b)[i] = vb, ++i, va = (a)[i])
// for when the type remains the same:
#define ahd_map(ht,i,t,a,b,v) if(a) \
			for(ahd_decl(ahd_int) i = ((a) == (b) || ahd_resetlen(ht, b, ht__len(a)), 0), ahd_foronce++;) \
			for(ahd_decl(t) v = (a)[i]; i < ahd__len(a); (b)[i] = v, ++i, v = (a)[i])
/* Usage:
 * int *lengths = 0;
 * arr_mapt(i_str, char *, strings, str, int, lengths, len)
 * { len = strlen(str); }
 *
 * int *lengths = 0;
 * arr_mapt(i_str, strings, lengths, int, len)
 * { len = strlen(strings[i_str]); }

 * int *lengths = 0;
 * arr_map(i_str, strings, lengths)
 * { lengths[i_str] = strlen(strings[i_str]); }
 */

// expression
#define ahd_mapfn(ht,fn,a,b,udata) (ahd_resetlen(ht, b, ht__len(header_type, array)), \
		                            ahd__map(a, sizeof(*(a)), b, sizeof(*(b)), sizeof(ht), ahd_len(a), fn, udata))

typedef struct ahd_any {
	union ahd_base {
		char *bytes;
		void *v;
	} base; // start of array
	ahd_int el_size;
	ahd_int hdr_size;
} ahd_any;

typedef struct ahd_loop_info {
	ahd_int i, n;
	void *udata;
	ahd_any a, b;
} ahd_loop_info;
typedef int ahd_loopfn(ahd_loop_info *info, ahd_any *a, ahd_any *b);

void ahd__map(void *arr_a, ahd_int elsize_a, void *arr_b, ahd_int elsize_b,
		      ahd_int hdrsize, ahd_int len, ahd_loopfn fn, void *udata)
{
	char *a = arr_a, *b = arr_b;
	ahd_loop_info info = { i, n, udata,
						   { arr_a, elsize_a, hdrsize },
						   { arr_b, elsize_b, hdrsize } };
	for(; info.i < len; ++info.i, a += elsize_a, b += elsize_b)
	{ fn(&info, a, b); }
}
/* Usage:
 * struct str{ char *str; int len; }

 * int map_strwlen(ahd_loop_info info, void *a, void *b, void *udata) {
 *     info; // UNUSED
 *     char **str = a;
 *     struct str *val = b;
 *     struct str new_val = { *str, strlen(*str) };
 *     *val = new_val;
 *     return 0;
 * }
 * ...
 * arr_mapfn(map_strwlen, strings, strs, 0);
 */

#define ahd_reduce(header_type, array,ex)
#define ahd_reducefn(ht,fn,a,acc,udata) ahd__reduce(acc, sizeof(acc), a, sizeof(*(a)), sizeof(ht), ahd_len(header_type, array), fn, udata)
#define ahd_find(header_type, array,ex) // first item caught by filter

void ahd__reduce(void *accum, ahd_int elsize_acc,
		         void *value, ahd_int elsize_val, ahd_int hdrsize_val,
		         ahd_int len, ahd_loopfn fn, void *udata)
{
	void *acc = accum,
	     *val = value;
	ahd_loop_info info = { i, n, udata,
						   { accum, elsize_acc, 0 },
						   { value, elsize_val, hdrsize_val } };
	for(; info.i < len; ++info.i, val += size_val)
	{ fn(&info, acc, val); }
}
/* Usage:
 * int reduce_sumstrlen(ahd_loop_info info, void *acc, void *val) {
 *     info; // UNUSED
 *     char **str = val;
 *     int *sum_len = acc;
 *     *sum_len += strlen(*str);
 *     return 0;
 * }
 * ...
 * arr_reducefn(reduce_sumstrlen, strings, sum_len, 0);
 */

#define ahd_filterreject(ht,i,t,v,a,b,tr,reset,test) if(a) \
			for(ahd_int i = (ahd_clear(ht,b), 0), (reset), ahd_foronce++;) \
			for(t v = (a)[i]; \
				i < ahd__len(a); \
				(test) && ahd_push(ht,b,(a)[i]), ++i, (reset))
#define ahd_filter(ht,i,t,v,a,b,tr) ahd_filterreject(ht,i,t,v,a,b,tr,tr=0, tr)
#define ahd_reject(ht,i,t,v,a,b,tr) ahd_filterreject(ht,i,t,v,a,b,tr,tr=1,!tr)
#define ahd_countmatch
#define ahd_countnomatch
/* Usage:
 * obj_t *filtered_objects = 0;
 * arr_filter(objects, i, obj_t, obj, objects, filtered_objects, found)
 * { found = obj.is_valid; }
 * printf("Original length: %d, filtered length: %d", arr_len(objects), arr_len(filtered_objects));
 *
 * arr_filter(objects, i, objects, filtered_objects, found)
 * { found = objects[i].is_valid; }
 * printf("Original length: %d, filtered length: %d", arr_len(objects), arr_len(filtered_objects));
 */

#define ahd_reject(ht,i,t,a,b,tr) if(a) \
			for(ahd_int tr = 0, i = (ahd_clear(ht,b), 0); \
				i < ahd__len(a); \
				!tr && ahd_push(ht,b,(a)[i]), ++i, tr = 1)

#define ahd_filterfn()
void ahd__filter(void *arr_a, void *out, ahd_int len, ahd_int size_a, ahd_int size_out, ahd_loopfn fn, void *udata) {
	char *a = arr_a;
	ahd_int i = 0;
	ahd_loop_info info = { i, n, size_a, size_out, udata };
	for(; i < len; ++i a += size_a) {
		info.i = i;
		fn(&info, a, out);
	}
}

#define ahd_split()
#define ahd_join()
// NOTE: should be able to have the index and found bools as internally or externally scoped
// due to the way that comma op works (?)
#define ahd_find(header_type, array,i,t,v,fnd,tr) if(a) \
			for(ahd_int AHD_LN(ahd_dummy), i = 0, tr = 0, (fnd) = 0); \
				i < ahd__len(header_type, array) && !(fnd); \
				tr ? (ahd_if(fnd, (fnd)=&(a)[i]), i) : ++i)

#define ahd_findi(header_type, array,i,t,v,tr)   ahd_find(ht, a, i,             t, v, 0,   tr)
#define ahd_findv(header_type, array,t,v,fnd,tr) ahd_find(ht, a, ahd_i_ln, t, v, fnd, tr)
/* Usage:
 * f32 *greater_than3; 
 * ahd_find(ahd_arr, arr, _, f32, val, greater_than3, found)
 * { found = val > 3.f; }
 * if(greater_than3) { assert(*greater_than3 > 3.f); }
 *
 * int found, found_index;
 * ahd_find(ahd_arr, arr, found_index, f32, val, 0, found)
 * { found = val > 3.f && found_index % 2; }
 * if(found) { assert(arr[found_index] > 3.f); }

 * int found, i_obj;
 * my_findi(objects, i_obj, obj_t, obj, found)
 * { found = obj.is_valid; }
 * if(found) { assert(objects[i_obj].is_valid); }
 */

#define ahd_exitscope continue
#define ahd_scope(ht,t,a) for(t *a = 0, *ahd_n_ln = 0; ! ahd_n_ln++; ahd_free(a))
#define ahd_scope2d(ht,t,a) for(t **a = 0, *ahd_n_ln = 0; ! ahd_n_ln++; ahd_free2d(header_type, array))
/* Usage:
 * arr_scoped(char *, strings)
 * {
 *     arr_push(strings, "Hello");
 *     arr_push(strings, "World");
 *     printf(strings[0]);
 *     if(strings[1][0] == 'W')
 *     { ahd_exitscope; }
 *
 *     puts(strings[1]);
 * } // no leaks
 *
 * puts(Strings[1]); // fails at compile time - Strings has not been declared
 *
 * // stackable (don't semicolon)
 * ahd_scoped(ahd_arr, char *, Strings)
 * ahd_scoped(ahd_arr, int, Ints)
 *
 */

