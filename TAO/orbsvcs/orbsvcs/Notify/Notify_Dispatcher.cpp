/* -*- C++ -*- $Id$ */

#include "Notify_Dispatcher.h"
#include "ace/CORBA_macros.h"
#include "orbsvcs/orbsvcs/CosNotifyCommC.h" // i get some errors with env otherwise

TAO_Notify_Dispatcher*
TAO_Notify_Dispatcher::create (CORBA::Environment &ACE_TRY_ENV)
{
  // just return the reactive dispatcher for now.
  // TODO: use the service configurator to allow the user to specify
  // which dispatcher to use and where.
  // @@ Should we have a factory instead?
  //
  TAO_Notify_Dispatcher* dispatcher;
  ACE_NEW_THROW_EX (dispatcher,
                    Notify_Reactive_Dispatcher (),
                    CORBA::NO_MEMORY());
  return dispatcher;
}

int
TAO_Notify_Dispatcher::add_dispatcher (TAO_Notify_Dispatcher& dispatcher)
{
  return contained_.insert (&dispatcher);
}

int
TAO_Notify_Dispatcher::remove_dispatcher (TAO_Notify_Dispatcher& dispatcher)
{
  return contained_.remove (&dispatcher);
}

void
Notify_Reactive_Dispatcher::dispatch_event (const CORBA::Any & data,
                                            CORBA::Environment &ACE_TRY_ENV)
{
  // TODO: Filtering comes here..

  DISPATCHER_SET_ITER iter (contained_);
  TAO_Notify_Dispatcher **p_dispatcher;

  for (iter.first ();iter.done () != 1;iter.advance ())
    {
      if (iter.next (p_dispatcher) == 0)
        break;
      (*p_dispatcher)->dispatch_event (data, ACE_TRY_ENV);
      ACE_CHECK;
    }
}
