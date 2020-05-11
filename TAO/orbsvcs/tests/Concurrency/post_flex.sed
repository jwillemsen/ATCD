s/unistd/stdio/g
s/\\$$\(Header[^$$]*\) \\$$/\\1/
s/free( ptr );/free( ACE_MALLOC_T (ptr) );/g
s/realloc( ptr, size );/realloc( ACE_MALLOC_T (ptr), size );/g
s/YY_BREAK break;/YY_BREAK ACE_NOTREACHED (break;)/g
s@#include <errno.h>@#include "ace/OS_NS_errno.h"@
s@#include <stdio.h>@#include "ace/OS_NS_stdio.h"@
s@#include <unistd.h>@#include "ace/os_include/os_ctype.h"@
s@c = getc@c = ACE_OS::getc@
s@199901L@199901L || defined (__HP_aCC)@
/#include <[seu]/d
