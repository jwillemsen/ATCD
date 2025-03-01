#include "ace/Monitor_Control/Constraint_Interpreter.h"

#if defined (ACE_HAS_MONITOR_FRAMEWORK) && (ACE_HAS_MONITOR_FRAMEWORK == 1)

#include "ace/Log_Category.h"

#include "ace/ETCL/ETCL_Constraint.h"

#include "ace/Monitor_Control/Constraint_Visitor.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace ACE
{
  namespace Monitor_Control
  {
    int
    Constraint_Interpreter::build_tree (const char *constraints)
    {
      if (ETCL_Interpreter::is_empty_string (constraints))
        {
          /// Root is deleted in the interpreter's destructor.
          ACE_NEW_RETURN (this->root_,
                          ETCL_Literal_Constraint (true),
                          -1);
        }
      else
        {
          /// root_ is set in this base class call.
          if (ETCL_Interpreter::build_tree (constraints) != 0)
            {
              ACELIB_ERROR_RETURN ((LM_ERROR,
                                 "Constraint_Interpreter::build_tree() - "
                                 "ETCL_Interpreter::build_tree() failed\n"),
                                -1);
            }
        }

      return 0;
    }

    ACE_CDR::Boolean
    Constraint_Interpreter::evaluate (Constraint_Visitor &evaluator)
    {
      return evaluator.evaluate_constraint (this->root_);
    }
  }
}

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_HAS_MONITOR_FRAMEWORK==1 */
