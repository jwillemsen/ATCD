//=============================================================================
/**
 *  @file    Exceptions.cpp
 *
 *  This test application tests the state of WFMO_Reactor when
 *  exceptions occurs when executing user callbacks.
 *
 *  The thread count in WFMO_Reactor is used to ensure that state of
 *  WFMO_Reactor is not fouled up when exceptions occur in user code.
 *  This example also shows how to write event loops that survive
 *  user exceptions
 *
 *  @author Irfan Pyarali
 */
//=============================================================================


#include "ace/OS_main.h"

#if defined (ACE_WIN32)

#include "ace/WFMO_Reactor.h"

class Event_Handler : public ACE_Event_Handler
{
public:
  Event_Handler ()
    : event_ (1)
  {
    ACE_DEBUG ((LM_DEBUG,
                "Event_Handler created\n"));
  }

  ~Event_Handler ()
  {
    ACE_DEBUG ((LM_DEBUG,
                "Event_Handler destroyed\n"));
  }

  int handle_signal (int, siginfo_t * = 0, ucontext_t * = 0)
  {
    ACE_DEBUG ((LM_DEBUG,
                "Event_Handler::handle_signal called\n"));
    volatile int* pInt = 0x0000000;
    *pInt = 20;
    return 0;
  }

  ACE_HANDLE get_handle () const
  {
    return this->event_.handle ();
  }
private:
  ACE_Manual_Event event_;
};

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

class ACE_WFMO_Reactor_Test
{
public:
  static void doit (ACE_WFMO_Reactor &wfmo_reactor)
  {
    for (int i = 1; i <= 10; i++)
      {
        ACE_DEBUG ((LM_DEBUG,
                    "Active threads in WFMO_Reactor (before handle_events) = %d\n",
                    wfmo_reactor.active_threads_));
        ACE_SEH_TRY
          {
            wfmo_reactor.handle_events ();
          }
        ACE_SEH_EXCEPT (EXCEPTION_EXECUTE_HANDLER)
          {
            ACE_DEBUG ((LM_DEBUG,
                        "Exception occurred\n"));
          }
        ACE_DEBUG ((LM_DEBUG,
                    "Active threads in WFMO_Reactor (after handle_events) = %d\n",
                    wfmo_reactor.active_threads_));
      }
  }
};

ACE_END_VERSIONED_NAMESPACE_DECL

int
ACE_TMAIN (int, ACE_TCHAR *[])
{
  Event_Handler handler;
  ACE_WFMO_Reactor wfmo_reactor;
  wfmo_reactor.register_handler (&handler);

  ACE_WFMO_Reactor_Test::doit (wfmo_reactor);

  return 0;
}
#else /* !ACE_WIN32 */
int
ACE_TMAIN (int, ACE_TCHAR *[])
{
  return 0;
}
#endif /* ACE_WIN32 */
