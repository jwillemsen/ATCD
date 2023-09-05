#ifndef STATISTICS_HELPER_H
#define STATISTICS_HELPER_H

#include "CSD_TP_Test_Export.h"
#include "tao/Basic_Types.h"
#include "ace/Containers_T.h"
#include "ace/SString.h"
#include <vector>

typedef std::vector< CORBA::Long, 1000 > LongVector;
typedef std::vector< ACE_CString, 1000 > StringVector;
typedef ACE_Array< CORBA::Long > LongArray;

extern CSD_TP_Test_Export void swap (CORBA::Long& x, CORBA::Long& y);

extern CSD_TP_Test_Export void sort (LongVector & vector);

#endif
