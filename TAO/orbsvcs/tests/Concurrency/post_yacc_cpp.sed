s/char \*getenv/char *ace_foo/g
s/= getenv/= ACE_OS::getenv/g
s/yynewerror://g
s/goto\ yynewerror.*//g
s/^\#pragma.*//g
