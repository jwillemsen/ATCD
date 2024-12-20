
//=============================================================================
/**
 *  @file    interface_is.cpp
 *
 *  Visitor generating code for Interfaces in the implementation skeletons file.
 *
 *  @author Yamuna Krishnamurthy (yamuna@cs.wustl.edu)
 */
//=============================================================================

#include "interface.h"

be_visitor_interface_is::be_visitor_interface_is (be_visitor_context *ctx)
  : be_visitor_interface (ctx)
{
}

be_visitor_interface_is::~be_visitor_interface_is ()
{
}

int
be_visitor_interface_is::visit_interface (be_interface *node)
{
  if (node->impl_skel_gen () || node->imported () || node->is_abstract ())
    {
      return 0;
    }

  this->ctx_->interface (node);
  TAO_OutStream *os = this->ctx_->stream ();

  // Generate the skeleton class name.

  if (be_global->gen_impl_debug_info ())
    {
      TAO_INSERT_COMMENT (os);
    }

  *os << "// Implementation skeleton constructor" << be_nl;

  // Find if we are at the top scope or inside some module.
  *os << be_global->impl_class_prefix () << node->flat_name ()
      << be_global->impl_class_suffix () <<"::"
      << be_global->impl_class_prefix () << node->flat_name ()
      << be_global->impl_class_suffix ()
      << " ()" << be_nl;

  *os << "{" << be_nl
      << "}" << be_nl_2;

  os->indent ();
  *os << "// Implementation skeleton destructor" << be_nl;

  *os << be_global->impl_class_prefix () << node->flat_name ()
      << be_global->impl_class_suffix () <<"::~"
      << be_global->impl_class_prefix () << node->flat_name ()
      << be_global->impl_class_suffix ()
      << " ()" << be_nl;

  *os << "{" <<be_nl;
  *os << "}" << be_nl_2;

  if (be_global->gen_assign_op ())
    {
      *os << "//Implementation Skeleton Copy Assignment" << be_nl;

      *os << be_global->impl_class_prefix () << node->flat_name ()
          << be_global->impl_class_suffix () << "& "
          << be_global->impl_class_prefix () << node->flat_name ()
          << be_global->impl_class_suffix () << "::operator=(const "
          << be_global->impl_class_prefix () << node->flat_name ()
          << be_global->impl_class_suffix () << "& t)" <<be_idt_nl
          << "{" << be_idt_nl
          << "return *this;" << be_uidt_nl
          << "}" << be_nl << be_uidt_nl;
    }

  // Generate code for elements in the scope (e.g., operations).

  if (this->visit_scope (node) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "be_visitor_interface_is::"
                         "visit_interface - "
                         "codegen for scope failed\n"),
                        -1);
    }

  int status =
    node->traverse_inheritance_graph (
              be_visitor_interface_is::method_helper,
              os
            );

  if (status == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "be_visitor_interface_is::"
                         "visit_interface - "
                         "traversal of inhertance graph failed\n"),
                        -1);
    }

  return 0;
}


// Helper method to generate the members in the scope of the base classes.
int
be_visitor_interface_is::method_helper (be_interface *derived,
                                        be_interface *node,
                                        TAO_OutStream *os)
{
  if (ACE_OS::strcmp (derived->flat_name (), node->flat_name ()) != 0)
    {
      be_visitor_context ctx;
      ctx.state (TAO_CodeGen::TAO_ROOT_IS);
      ctx.interface (derived);
      ctx.stream (os);
      be_visitor_interface_is visitor (&ctx);

      if (visitor.visit_scope (node) == -1)

        {
          ACE_ERROR_RETURN ((LM_ERROR,
                             "be_visitor_interface_is::"
                             "method_helper\n"),
                            -1);
        }

    }

  return 0;
}
