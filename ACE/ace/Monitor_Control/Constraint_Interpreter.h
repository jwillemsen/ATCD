// -*- C++ -*-

//=============================================================================
/**
 *  @file   Constraint_Interpreter.h
 *
 *  @author Jeff Parsons <j.parsons@vanderbilt.edu>
 */
//=============================================================================

#ifndef CONSTRAINT_INTERPRETER_H
#define CONSTRAINT_INTERPRETER_H

#include /**/ "ace/pre.h"

#include "ace/CDR_Base.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined (ACE_HAS_MONITOR_FRAMEWORK) && (ACE_HAS_MONITOR_FRAMEWORK == 1)

#include "ace/ETCL/ETCL_Interpreter.h"

#include "ace/Monitor_Control/Monitor_Control_export.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace ACE
{
  namespace Monitor_Control
  {
    class Constraint_Visitor;

    /**
     * @class Constraint_Interpreter
     *
     * @brief "ETCL" Interpreter for MonitorControl constraints,
              specialized to encapsulate MonitorControl-specific
              knowledge of types and visitors.
     */
    class MONITOR_CONTROL_Export Constraint_Interpreter
      : public ETCL_Interpreter
    {
    public:
      Constraint_Interpreter () = default;

      virtual ~Constraint_Interpreter () = default;

      /**
       * This method builds an expression tree representing the
       * constraint specified in <constraints>, and returns -1 with
       * an error message if the constraint given has syntax errors or
       * semantic errors, such as mismatched types.
       */
      int build_tree (const char* constraints);

      /// Returns true if the constraint is evaluated successfully by
      /// the evaluator.
      ACE_CDR::Boolean evaluate (Constraint_Visitor &evaluator);
    };
  }
}

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_MONITOR_FRAMEWORK==1 */

#include /**/ "ace/post.h"

#endif /* CONSTRAINT_INTERPRETER_H */
