s/unistd/stdio/g
s/\\$$\(Header[^$$]*\) \\$$/\\1/
s/yy/ace_cc_yy/g
s/YY/ACE_CC_YY/g
s/free( ptr );/free( ACE_MALLOC_T (ptr) );/g
s/realloc( ptr, size );/realloc( ACE_MALLOC_T (ptr), size );/g
