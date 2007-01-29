//
// $Id$
//
#include "Oneway_Buffering.h"

ACE_RCSID(Oneway_Buffering, Oneway_Buffering, "$Id$")

Oneway_Buffering::Oneway_Buffering (CORBA::ORB_ptr orb,
                                    Test::Oneway_Buffering_Admin_ptr admin)
  : orb_ (CORBA::ORB::_duplicate (orb))
  , admin_ (Test::Oneway_Buffering_Admin::_duplicate (admin))
{
}

void
Oneway_Buffering::receive_data (const Test::Payload &the_payload)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->admin_->request_received (the_payload.length ());
}

void
Oneway_Buffering::flush (void)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
}

void
Oneway_Buffering::sync (void)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->admin_->flush ();
}

void
Oneway_Buffering::shutdown (void)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0);
}
