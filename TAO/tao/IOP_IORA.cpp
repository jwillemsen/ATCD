// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

#include "IOP_IORC.h"
#include "tao/Null_RefCount_Policy.h"
#include "tao/TypeCode_Constants.h"
#include "tao/Alias_TypeCode.h"
#include "tao/Sequence_TypeCode.h"
#include "tao/String_TypeCode.h"
#include "tao/Struct_TypeCode.h"
#include "tao/CDR.h"
#include "tao/Any.h"
#include "tao/Any_Dual_Impl_T.h"

// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_ProfileId (
    "IDL:omg.org/IOP/ProfileId:1.0",
    "ProfileId",
    &CORBA::_tc_ulong);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_ProfileId =
    &_tao_tc_IOP_ProfileId;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/struct_typecode.cpp:34

static TAO::TypeCode::Struct_Field<char const *> _tao_fields_IOP_TaggedProfile[] =
  {
    { "tag", &IOP::_tc_ProfileId },
    { "profile_data", &CORBA::_tc_OctetSeq }
    
  };
static TAO::TypeCode::Struct<char const *,
                             TAO::TypeCode::Struct_Field<char const *> const *,
                             CORBA::tk_struct,
                             TAO::Null_RefCount_Policy>
  _tao_tc_IOP_TaggedProfile (
    "IDL:omg.org/IOP/TaggedProfile:1.0",
    "TaggedProfile",
    _tao_fields_IOP_TaggedProfile,
    2);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_TaggedProfile =
    &_tao_tc_IOP_TaggedProfile;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31



// TAO_IDL - Generated from
// be/be_visitor_typecode/typecode_defn.cpp:743

namespace TAO
{
  namespace TypeCode
  {
    TAO::TypeCode::Sequence<TAO::Null_RefCount_Policy>
      IOP_TaggedProfileSeq_0 (
        CORBA::tk_sequence,
        &IOP::_tc_TaggedProfile,
        0U);
      
    ::CORBA::TypeCode_ptr const tc_IOP_TaggedProfileSeq_0 =
      &IOP_TaggedProfileSeq_0;
    
  }
}

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_TaggedProfileSeq (
    "IDL:omg.org/IOP/TaggedProfileSeq:1.0",
    "TaggedProfileSeq",
    &TAO::TypeCode::tc_IOP_TaggedProfileSeq_0);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_TaggedProfileSeq =
    &_tao_tc_IOP_TaggedProfileSeq;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/struct_typecode.cpp:34

static TAO::TypeCode::Struct_Field<char const *> _tao_fields_IOP_IOR[] =
  {
    { "type_id", &CORBA::_tc_string },
    { "profiles", &IOP::_tc_TaggedProfileSeq }
    
  };
static TAO::TypeCode::Struct<char const *,
                             TAO::TypeCode::Struct_Field<char const *> const *,
                             CORBA::tk_struct,
                             TAO::Null_RefCount_Policy>
  _tao_tc_IOP_IOR (
    "IDL:omg.org/IOP/IOR:1.0",
    "IOR",
    _tao_fields_IOP_IOR,
    2);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_IOR =
    &_tao_tc_IOP_IOR;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_ComponentId (
    "IDL:omg.org/IOP/ComponentId:1.0",
    "ComponentId",
    &CORBA::_tc_ulong);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_ComponentId =
    &_tao_tc_IOP_ComponentId;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/struct_typecode.cpp:34

static TAO::TypeCode::Struct_Field<char const *> _tao_fields_IOP_TaggedComponent[] =
  {
    { "tag", &IOP::_tc_ComponentId },
    { "component_data", &CORBA::_tc_OctetSeq }
    
  };
static TAO::TypeCode::Struct<char const *,
                             TAO::TypeCode::Struct_Field<char const *> const *,
                             CORBA::tk_struct,
                             TAO::Null_RefCount_Policy>
  _tao_tc_IOP_TaggedComponent (
    "IDL:omg.org/IOP/TaggedComponent:1.0",
    "TaggedComponent",
    _tao_fields_IOP_TaggedComponent,
    2);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_TaggedComponent =
    &_tao_tc_IOP_TaggedComponent;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31



// TAO_IDL - Generated from
// be/be_visitor_typecode/typecode_defn.cpp:743

namespace TAO
{
  namespace TypeCode
  {
    TAO::TypeCode::Sequence<TAO::Null_RefCount_Policy>
      IOP_MultipleComponentProfile_0 (
        CORBA::tk_sequence,
        &IOP::_tc_TaggedComponent,
        0U);
      
    ::CORBA::TypeCode_ptr const tc_IOP_MultipleComponentProfile_0 =
      &IOP_MultipleComponentProfile_0;
    
  }
}

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_MultipleComponentProfile (
    "IDL:omg.org/IOP/MultipleComponentProfile:1.0",
    "MultipleComponentProfile",
    &TAO::TypeCode::tc_IOP_MultipleComponentProfile_0);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_MultipleComponentProfile =
    &_tao_tc_IOP_MultipleComponentProfile;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31



// TAO_IDL - Generated from
// be/be_visitor_typecode/typecode_defn.cpp:743

namespace TAO
{
  namespace TypeCode
  {
    TAO::TypeCode::Sequence<TAO::Null_RefCount_Policy>
      IOP_TaggedComponentList_0 (
        CORBA::tk_sequence,
        &IOP::_tc_TaggedComponent,
        0U);
      
    ::CORBA::TypeCode_ptr const tc_IOP_TaggedComponentList_0 =
      &IOP_TaggedComponentList_0;
    
  }
}

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_TaggedComponentList (
    "IDL:omg.org/IOP/TaggedComponentList:1.0",
    "TaggedComponentList",
    &TAO::TypeCode::tc_IOP_TaggedComponentList_0);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_TaggedComponentList =
    &_tao_tc_IOP_TaggedComponentList;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31



// TAO_IDL - Generated from
// be/be_visitor_typecode/typecode_defn.cpp:743

namespace TAO
{
  namespace TypeCode
  {
    TAO::TypeCode::Sequence<TAO::Null_RefCount_Policy>
      IOP_TaggedComponentSeq_0 (
        CORBA::tk_sequence,
        &IOP::_tc_TaggedComponent,
        0U);
      
    ::CORBA::TypeCode_ptr const tc_IOP_TaggedComponentSeq_0 =
      &IOP_TaggedComponentSeq_0;
    
  }
}

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_TaggedComponentSeq (
    "IDL:omg.org/IOP/TaggedComponentSeq:1.0",
    "TaggedComponentSeq",
    &TAO::TypeCode::tc_IOP_TaggedComponentSeq_0);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_TaggedComponentSeq =
    &_tao_tc_IOP_TaggedComponentSeq;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_ServiceId (
    "IDL:omg.org/IOP/ServiceId:1.0",
    "ServiceId",
    &CORBA::_tc_ulong);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_ServiceId =
    &_tao_tc_IOP_ServiceId;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/struct_typecode.cpp:34

static TAO::TypeCode::Struct_Field<char const *> _tao_fields_IOP_ServiceContext[] =
  {
    { "context_id", &IOP::_tc_ServiceId },
    { "context_data", &CORBA::_tc_OctetSeq }
    
  };
static TAO::TypeCode::Struct<char const *,
                             TAO::TypeCode::Struct_Field<char const *> const *,
                             CORBA::tk_struct,
                             TAO::Null_RefCount_Policy>
  _tao_tc_IOP_ServiceContext (
    "IDL:omg.org/IOP/ServiceContext:1.0",
    "ServiceContext",
    _tao_fields_IOP_ServiceContext,
    2);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_ServiceContext =
    &_tao_tc_IOP_ServiceContext;
}



// TAO_IDL - Generated from
// be/be_visitor_typecode/alias_typecode.cpp:31



// TAO_IDL - Generated from
// be/be_visitor_typecode/typecode_defn.cpp:743

namespace TAO
{
  namespace TypeCode
  {
    TAO::TypeCode::Sequence<TAO::Null_RefCount_Policy>
      IOP_ServiceContextList_0 (
        CORBA::tk_sequence,
        &IOP::_tc_ServiceContext,
        0U);
      
    ::CORBA::TypeCode_ptr const tc_IOP_ServiceContextList_0 =
      &IOP_ServiceContextList_0;
    
  }
}

static TAO::TypeCode::Alias<char const *,
                            TAO::Null_RefCount_Policy>
  _tao_tc_IOP_ServiceContextList (
    "IDL:omg.org/IOP/ServiceContextList:1.0",
    "ServiceContextList",
    &TAO::TypeCode::tc_IOP_ServiceContextList_0);
  
namespace IOP
{
  ::CORBA::TypeCode_ptr const _tc_ServiceContextList =
    &_tao_tc_IOP_ServiceContextList;
}



// TAO_IDL - Generated from 
// be/be_visitor_structure/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::TaggedProfile &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedProfile>::insert_copy (
      _tao_any,
      IOP::TaggedProfile::_tao_any_destructor,
      IOP::_tc_TaggedProfile,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::TaggedProfile *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedProfile>::insert (
      _tao_any,
      IOP::TaggedProfile::_tao_any_destructor,
      IOP::_tc_TaggedProfile,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::TaggedProfile *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::TaggedProfile *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::TaggedProfile *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::TaggedProfile>::extract (
        _tao_any,
        IOP::TaggedProfile::_tao_any_destructor,
        IOP::_tc_TaggedProfile,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_sequence/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::TaggedProfileSeq &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedProfileSeq>::insert_copy (
      _tao_any,
      IOP::TaggedProfileSeq::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_TaggedProfileSeq_0,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::TaggedProfileSeq *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedProfileSeq>::insert (
      _tao_any,
      IOP::TaggedProfileSeq::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_TaggedProfileSeq_0,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::TaggedProfileSeq *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::TaggedProfileSeq *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::TaggedProfileSeq *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::TaggedProfileSeq>::extract (
        _tao_any,
        IOP::TaggedProfileSeq::_tao_any_destructor,
        TAO::TypeCode::tc_IOP_TaggedProfileSeq_0,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_structure/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::IOR &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::IOR>::insert_copy (
      _tao_any,
      IOP::IOR::_tao_any_destructor,
      IOP::_tc_IOR,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::IOR *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::IOR>::insert (
      _tao_any,
      IOP::IOR::_tao_any_destructor,
      IOP::_tc_IOR,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::IOR *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::IOR *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::IOR *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::IOR>::extract (
        _tao_any,
        IOP::IOR::_tao_any_destructor,
        IOP::_tc_IOR,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_structure/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::TaggedComponent &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedComponent>::insert_copy (
      _tao_any,
      IOP::TaggedComponent::_tao_any_destructor,
      IOP::_tc_TaggedComponent,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::TaggedComponent *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedComponent>::insert (
      _tao_any,
      IOP::TaggedComponent::_tao_any_destructor,
      IOP::_tc_TaggedComponent,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::TaggedComponent *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::TaggedComponent *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::TaggedComponent *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::TaggedComponent>::extract (
        _tao_any,
        IOP::TaggedComponent::_tao_any_destructor,
        IOP::_tc_TaggedComponent,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_sequence/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::MultipleComponentProfile &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::MultipleComponentProfile>::insert_copy (
      _tao_any,
      IOP::MultipleComponentProfile::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_MultipleComponentProfile_0,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::MultipleComponentProfile *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::MultipleComponentProfile>::insert (
      _tao_any,
      IOP::MultipleComponentProfile::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_MultipleComponentProfile_0,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::MultipleComponentProfile *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::MultipleComponentProfile *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::MultipleComponentProfile *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::MultipleComponentProfile>::extract (
        _tao_any,
        IOP::MultipleComponentProfile::_tao_any_destructor,
        TAO::TypeCode::tc_IOP_MultipleComponentProfile_0,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_sequence/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::TaggedComponentList &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedComponentList>::insert_copy (
      _tao_any,
      IOP::TaggedComponentList::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_TaggedComponentList_0,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::TaggedComponentList *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedComponentList>::insert (
      _tao_any,
      IOP::TaggedComponentList::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_TaggedComponentList_0,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::TaggedComponentList *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::TaggedComponentList *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::TaggedComponentList *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::TaggedComponentList>::extract (
        _tao_any,
        IOP::TaggedComponentList::_tao_any_destructor,
        TAO::TypeCode::tc_IOP_TaggedComponentList_0,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_sequence/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::TaggedComponentSeq &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedComponentSeq>::insert_copy (
      _tao_any,
      IOP::TaggedComponentSeq::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_TaggedComponentSeq_0,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::TaggedComponentSeq *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::TaggedComponentSeq>::insert (
      _tao_any,
      IOP::TaggedComponentSeq::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_TaggedComponentSeq_0,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::TaggedComponentSeq *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::TaggedComponentSeq *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::TaggedComponentSeq *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::TaggedComponentSeq>::extract (
        _tao_any,
        IOP::TaggedComponentSeq::_tao_any_destructor,
        TAO::TypeCode::tc_IOP_TaggedComponentSeq_0,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_structure/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::ServiceContext &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::ServiceContext>::insert_copy (
      _tao_any,
      IOP::ServiceContext::_tao_any_destructor,
      IOP::_tc_ServiceContext,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::ServiceContext *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::ServiceContext>::insert (
      _tao_any,
      IOP::ServiceContext::_tao_any_destructor,
      IOP::_tc_ServiceContext,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::ServiceContext *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::ServiceContext *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::ServiceContext *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::ServiceContext>::extract (
        _tao_any,
        IOP::ServiceContext::_tao_any_destructor,
        IOP::_tc_ServiceContext,
        _tao_elem
      );
}

// TAO_IDL - Generated from 
// be/be_visitor_sequence/any_op_cs.cpp:54

// Copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    const IOP::ServiceContextList &_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::ServiceContextList>::insert_copy (
      _tao_any,
      IOP::ServiceContextList::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_ServiceContextList_0,
      _tao_elem
    );
}

// Non-copying insertion.
void operator<<= (
    CORBA::Any &_tao_any,
    IOP::ServiceContextList *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<IOP::ServiceContextList>::insert (
      _tao_any,
      IOP::ServiceContextList::_tao_any_destructor,
      TAO::TypeCode::tc_IOP_ServiceContextList_0,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    IOP::ServiceContextList *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const IOP::ServiceContextList *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
CORBA::Boolean operator>>= (
    const CORBA::Any &_tao_any,
    const IOP::ServiceContextList *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<IOP::ServiceContextList>::extract (
        _tao_any,
        IOP::ServiceContextList::_tao_any_destructor,
        TAO::TypeCode::tc_IOP_ServiceContextList_0,
        _tao_elem
      );
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

  template class
    TAO::Any_Dual_Impl_T<
        IOP::TaggedProfile
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::TaggedProfileSeq
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::IOR
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::TaggedComponent
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::MultipleComponentProfile
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::TaggedComponentList
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::TaggedComponentSeq
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::ServiceContext
      >;

  template class
    TAO::Any_Dual_Impl_T<
        IOP::ServiceContextList
      >;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::TaggedProfile \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::TaggedProfileSeq \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::IOR \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::TaggedComponent \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::MultipleComponentProfile \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::TaggedComponentList \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::TaggedComponentSeq \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::ServiceContext \
      >

# pragma instantiate \
    TAO::Any_Dual_Impl_T< \
        IOP::ServiceContextList \
      >

#endif /* !ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */ 
