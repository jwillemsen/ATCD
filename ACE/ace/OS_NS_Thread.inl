// -*- C++ -*-
#include "ace/OS_NS_macros.h"
// for timespec_t, perhaps move it to os_time.h
#include "ace/Time_Value.h"
#include "ace/OS_NS_sys_mman.h"
#include "ace/OS_NS_sys_time.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_errno.h"

#if defined (ACE_USES_FIFO_SEM)
#  include "ace/OS_NS_sys_stat.h"
#  include "ace/OS_NS_sys_select.h"
#  include "ace/OS_NS_fcntl.h"
#  include "ace/Handle_Set.h"
# endif /* ACE_USES_FIFO_SEM */

#if defined (ACE_HAS_ALLOC_HOOKS)
# include "ace/Malloc_Base.h"
#endif /* ACE_HAS_ALLOC_HOOKS */

#if defined (ACE_INTEGRITY) && defined (ACE_HAS_TSS_EMULATION) && !defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
#  include "ace/Object_Manager.h"
#endif

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

/*****************************************************************************/

#if defined (ACE_LACKS_COND_T) && defined (ACE_HAS_THREADS)
ACE_INLINE long
ACE_cond_t::waiters () const
{
  return this->waiters_;
}
#endif /* ACE_LACKS_COND_T && ACE_HAS_THREADS */

/*****************************************************************************/

#if defined (ACE_HAS_TSS_EMULATION)
#  if !defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)

ACE_INLINE
void **&
ACE_TSS_Emulation::tss_base ()
{
#    if defined (ACE_HAS_VXTHREADS)
#      if (!defined (_WRS_CONFIG_SMP) && !defined (INCLUDE_AMP_CPU))
  if (taskVarGet(0, reinterpret_cast<int*>(&ace_tss_keys)) == ERROR)
    taskVarAdd(0, reinterpret_cast<int*>(&ace_tss_keys));
#      endif
  return reinterpret_cast <void **&> (ace_tss_keys);
#    elif defined (ACE_INTEGRITY)
  ACE_INTEGRITY_TSS_Impl *const obj = static_cast<ACE_INTEGRITY_TSS_Impl*> (ACE_Object_Manager::preallocated_object[ACE_Object_Manager::ACE_INTEGRITY_TSS_IMPL]);
  void **&base = obj->get (CurrentTask ());
  return base;
#    else
  // Uh oh.
  ACE_NOTSUP_RETURN (0);
#    endif /* ACE_HAS_VXTHREADS */
}
#  endif /* ! ACE_HAS_THREAD_SPECIFIC_STORAGE */

ACE_INLINE
ACE_TSS_Emulation::ACE_TSS_DESTRUCTOR
ACE_TSS_Emulation::tss_destructor (const ACE_thread_key_t key)
{
  return tss_destructor_ [key];
}

ACE_INLINE
void
ACE_TSS_Emulation::tss_destructor (const ACE_thread_key_t key,
                                   ACE_TSS_DESTRUCTOR destructor)
{
  tss_destructor_ [key] = destructor;
}

ACE_INLINE
void *&
ACE_TSS_Emulation::ts_object (const ACE_thread_key_t key)
{
#    if defined (ACE_HAS_VXTHREADS)
    /* If someone wants tss_base make sure they get one.  This
       gets used if someone spawns a VxWorks task directly, not
       through ACE.  The allocated array will never be deleted! */
    if (ace_tss_keys == 0)
      {
        ace_tss_keys = new void *[ACE_TSS_THREAD_KEYS_MAX];

        // Zero the entire TSS array.  Do it manually instead of using
        // memset, for optimum speed.  Though, memset may be faster :-)
        void **tss_base_p = reinterpret_cast<void **> (ace_tss_keys);
        for (u_int i = 0; i < ACE_TSS_THREAD_KEYS_MAX; ++i, ++tss_base_p)
          {
            *tss_base_p = 0;
          }
      }
#    endif /* ACE_HAS_VXTHREADS */

  return tss_base ()[key];
}

#endif /* ACE_HAS_TSS_EMULATION */

/*****************************************************************************/

ACE_INLINE int
ACE_OS::thr_equal (ACE_thread_t t1, ACE_thread_t t2)
{
#if defined (ACE_HAS_PTHREADS)
# if defined (pthread_equal)
  // If it's a macro we can't say "pthread_equal"...
  return pthread_equal (t1, t2);
# else
  return pthread_equal (t1, t2);
# endif /* pthread_equal */
#else /* For both STHREADS and WTHREADS... */
  // Hum, Do we need to treat WTHREAD differently?
  // levine 13 oct 98 % I don't think so, ACE_thread_t is a DWORD.
  return t1 == t2;
#endif /* ACE_HAS_PTHREADS */
}

ACE_INLINE int
ACE_OS::condattr_destroy (ACE_condattr_t &attributes)
{
#if defined (ACE_HAS_THREADS) && !defined (ACE_LACKS_CONDATTR)
#   if defined (ACE_HAS_PTHREADS)
  pthread_condattr_destroy (&attributes);
#   else
  attributes.type = 0;
#   endif /* ACE_HAS_PTHREADS */
  return 0;
# else
  ACE_UNUSED_ARG (attributes);
  return 0;
# endif /* ACE_HAS_THREADS  */
}

ACE_INLINE int
ACE_OS::condattr_init (ACE_condattr_t &attributes, int type)
{
  ACE_UNUSED_ARG (type);
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = -1;

#     if !defined (ACE_LACKS_CONDATTR)
#       if defined (ACE_PTHREAD_CONDATTR_T_INITIALIZE)
  /* Tests show that VxWorks 6.x pthread lib does not only
    * require zeroing of mutex/condition objects to function correctly
    * but also of the attribute objects.
    */
  ACE_OS::memset (&attributes, 0, sizeof (attributes));
#       endif
  if (
      ACE_ADAPT_RETVAL (pthread_condattr_init (&attributes), result) == 0
#       if defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)
      && ACE_ADAPT_RETVAL (pthread_condattr_setpshared (&attributes, type),
                           result) == 0
#       endif /* _POSIX_THREAD_PROCESS_SHARED && ! ACE_LACKS_CONDATTR_PSHARED */
      )
#     else
  if (type == USYNC_THREAD)
#     endif /* !ACE_LACKS_CONDATTR */
     result = 0;
  else
    {
      ACE_UNUSED_ARG (attributes);
      result = -1;       // ACE_ADAPT_RETVAL used it for intermediate status
    }

  return result;
#   else
  attributes.type = type;
  return 0;
#   endif /* ACE_HAS_PTHREADS */

# else
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (type);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::condattr_synctype (ACE_condattr_t &attributes, int& type)
{
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
#   if !defined (ACE_LACKS_CONDATTR) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)
  int result = -1;

  if (
      ACE_ADAPT_RETVAL (pthread_condattr_getpshared (&attributes, &type),
                           result) == 0
     )
    {
      result = 0;
    }
#   else
  ACE_UNUSED_ARG (attributes);
  int result = 0;
  type = USYNC_THREAD;
#   endif /* !ACE_LACKS_CONDATTR && _POSIX_THREAD_PROCESS_SHARED && ! ACE_LACKS_CONDATTR_PSHARED */

  return result;
#   else
  type = attributes.type;
  return 0;
#   endif /* ACE_HAS_PTHREADS */

# else
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (type);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::condattr_setclock (ACE_condattr_t &attributes, clockid_t clock_id)
{
#if defined (ACE_HAS_CONDATTR_SETCLOCK) && !defined (ACE_LACKS_CONDATTR_SETCLOCK)
  int result = -1;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_condattr_setclock (&attributes, clock_id),
                                       result),
                     int);
  return result;
#else
  ACE_UNUSED_ARG (clock_id);
  ACE_UNUSED_ARG (attributes);
  ACE_NOTSUP_RETURN (-1);
#endif
}

#if !defined (ACE_LACKS_COND_T)
// NOTE: The ACE_OS::cond_* functions for Unix platforms are defined
// here because the ACE_OS::sema_* functions below need them.
// However, ACE_WIN32 and VXWORKS define the ACE_OS::cond_* functions
// using the ACE_OS::sema_* functions.  So, they are defined in OS_NS_Tread.cpp.

ACE_INLINE int
ACE_OS::cond_broadcast (ACE_cond_t *cv)
{
  ACE_OS_TRACE ("ACE_OS::cond_broadcast");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_broadcast (cv),
                                       result),
                     int);
#   elif defined (ACE_HAS_WTHREADS) && defined (ACE_HAS_WTHREADS_CONDITION_VARIABLE)
  ::WakeAllConditionVariable  (cv);
  return 0;
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::cond_destroy (ACE_cond_t *cv)
{
  ACE_OS_TRACE ("ACE_OS::cond_destroy");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_destroy (cv), result), int);
#   elif defined (ACE_HAS_WTHREADS) && defined (ACE_HAS_WTHREADS_CONDITION_VARIABLE)
  // Windows doesn't have a destroy
  return 0;
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::cond_init (ACE_cond_t *cv,
                   ACE_condattr_t &attributes,
                   const char *name,
                   void *arg)
{
  // ACE_OS_TRACE ("ACE_OS::cond_init");
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = -1;

#     if defined (ACE_PTHREAD_COND_T_INITIALIZE)
  /* VxWorks 6.x API reference states:
   *   If the memory for the condition variable object has been allocated
   *   dynamically, it is a good policy to always zero out the
   *   block of memory so as to avoid spurious EBUSY return code
   *   when calling this routine.
   */
  ACE_OS::memset (cv, 0, sizeof (*cv));
#     endif

  if (ACE_ADAPT_RETVAL (pthread_cond_init (cv, &attributes), result) == 0)
     result = 0;
  else
     result = -1;       // ACE_ADAPT_RETVAL used it for intermediate status

  return result;
#   elif defined (ACE_HAS_WTHREADS) && defined (ACE_HAS_WTHREADS_CONDITION_VARIABLE)
    ::InitializeConditionVariable (cv);
    return 0;
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::cond_init (ACE_cond_t *cv,
                   ACE_condattr_t &attributes,
                   const wchar_t *name,
                   void *arg)
{
  return ACE_OS::cond_init (cv, attributes, ACE_Wide_To_Ascii (name).char_rep (), arg);
}
#endif /* ACE_HAS_WCHAR */

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::cond_init (ACE_cond_t *cv, short type, const wchar_t *name, void *arg)
{
  return ACE_OS::cond_init (cv, type, ACE_Wide_To_Ascii (name).char_rep (), arg);
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
ACE_OS::cond_signal (ACE_cond_t *cv)
{
  ACE_OS_TRACE ("ACE_OS::cond_signal");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_signal (cv), result),
                     int);
#   elif defined (ACE_HAS_WTHREADS) && defined (ACE_HAS_WTHREADS_CONDITION_VARIABLE)
  ::WakeConditionVariable (cv);
  return 0;
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::cond_wait (ACE_cond_t *cv,
                   ACE_mutex_t *external_mutex)
{
  ACE_OS_TRACE ("ACE_OS::cond_wait");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_wait (cv, external_mutex), result),
                     int);
#   elif defined (ACE_HAS_WTHREADS) && defined (ACE_HAS_WTHREADS_CONDITION_VARIABLE)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::SleepConditionVariableCS (cv, &external_mutex->thr_mutex_, INFINITE), result),
                     int);
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (external_mutex);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::cond_timedwait (ACE_cond_t *cv,
                        ACE_mutex_t *external_mutex,
                        ACE_Time_Value *timeout)
{
  ACE_OS_TRACE ("ACE_OS::cond_timedwait");
# if defined (ACE_HAS_THREADS)
  int result = 0;
  timespec_t ts;

  if (timeout != 0)
    ts = *timeout; // Calls ACE_Time_Value::operator timespec_t().

#   if defined (ACE_HAS_PTHREADS)

  ACE_OSCALL (ACE_ADAPT_RETVAL (timeout == 0
                                ? pthread_cond_wait (cv, external_mutex)
                                : pthread_cond_timedwait (cv, external_mutex,
                                                            (ACE_TIMESPEC_PTR) &ts),
                                result),
              int, result);
  // We need to adjust this to make the POSIX return
  // values consistent.  EAGAIN is from Pthreads DRAFT4
  if (result == -1 &&
      (errno == ETIMEDOUT || errno == EAGAIN))
    errno = ETIME;

#   elif defined (ACE_HAS_WTHREADS) && defined (ACE_HAS_WTHREADS_CONDITION_VARIABLE)
  int msec_timeout = 0;
  if (timeout != 0)
    {
      ACE_Time_Value relative_time = timeout->to_relative_time ();
      // Watchout for situations where a context switch has caused the
      // current time to be > the timeout.
      if (relative_time > ACE_Time_Value::zero)
        msec_timeout = relative_time.msec ();
    }

  ACE_OSCALL (ACE_ADAPT_RETVAL (::SleepConditionVariableCS (cv, &external_mutex->thr_mutex_, msec_timeout),
                                result),
              int, result);

  return result;
#   endif /* ACE_HAS_PTHREADS */
  if (timeout != 0)
    timeout->set (ts); // Update the time value before returning.

  return result;
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (external_mutex);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}
#endif /* !ACE_LACKS_COND_T */

ACE_INLINE int
ACE_OS::mutex_lock (ACE_mutex_t *m,
                    const ACE_Time_Value *timeout)
{
  return timeout == 0 ? ACE_OS::mutex_lock (m) : ACE_OS::mutex_lock (m, *timeout);
}

ACE_INLINE int
ACE_OS::event_wait (ACE_event_t *event)
{
  return ACE_OS::event_timedwait (event, 0);
}

ACE_INLINE int
ACE_OS::event_init (ACE_event_t *event,
                    int manual_reset,
                    int initial_state,
                    int type,
                    const char *name,
                    void *arg,
                    LPSECURITY_ATTRIBUTES sa)
{
  ACE_condattr_t *pattr = 0;
  return ACE_OS::event_init (event, type, pattr, manual_reset, initial_state, name, arg, sa);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::event_init (ACE_event_t *event,
                    int manual_reset,
                    int initial_state,
                    int type,
                    const wchar_t *name,
                    void *arg,
                    LPSECURITY_ATTRIBUTES sa)
{
#if defined (ACE_WIN32)
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (arg);
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  *event = ::CreateEventW (ACE_OS::default_win32_security_attributes_r
      (sa, &sa_buffer, &sd_buffer),
  manual_reset,
  initial_state,
  name);
  if (*event == 0)
    ACE_FAIL_RETURN (-1);

  // Make sure to set errno to ERROR_ALREADY_EXISTS if necessary.
  ACE_OS::set_errno_to_last_error ();
  return 0;
#else  /* ACE_WIN32 */
  return ACE_OS::event_init (event,
                             manual_reset,
                             initial_state,
                             type,
                             ACE_Wide_To_Ascii (name).char_rep (),
                             arg,
                             sa);
#endif /* ACE_WIN32 */
}

ACE_INLINE int
ACE_OS::event_init (ACE_event_t *event,
                    int type,
                    ACE_condattr_t *attributes,
                    int manual_reset,
                    int initial_state,
                    const wchar_t *name,
                    void *arg,
                    LPSECURITY_ATTRIBUTES sa)
{
#if defined (ACE_WIN32)
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (arg);
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  *event = ::CreateEventW (ACE_OS::default_win32_security_attributes_r
      (sa, &sa_buffer, &sd_buffer),
  manual_reset,
  initial_state,
  name);
  if (*event == 0)
    ACE_FAIL_RETURN (-1);

  // Make sure to set errno to ERROR_ALREADY_EXISTS if necessary.
  ACE_OS::set_errno_to_last_error ();
  return 0;
#else  /* ACE_WIN32 */
  return ACE_OS::event_init (event,
                             type,
                             attributes,
                             manual_reset,
                             initial_state,
                             ACE_Wide_To_Ascii (name).char_rep (),
                             arg,
                             sa);
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE long
ACE_OS::priority_control (ACE_idtype_t /*idtype*/, ACE_id_t /*identifier*/, int /*cmd*/, void * /*arg*/)
{
  ACE_OS_TRACE ("ACE_OS::priority_control");
  ACE_NOTSUP_RETURN (-1);
}

// This method is used to prepare the recursive mutex for releasing
// when waiting on a condition variable. If the platform doesn't have
// native recursive mutex and condition variable support, then ACE needs
// to save the recursion state around the wait and also ensure that the
// wait and lock release are atomic. recursive_mutex_cond_relock()
// is the inverse of this method.
ACE_INLINE int
ACE_OS::recursive_mutex_cond_unlock (ACE_recursive_thread_mutex_t *m,
                                     ACE_recursive_mutex_state &state)
{
#if defined (ACE_HAS_THREADS)
  ACE_OS_TRACE ("ACE_OS::recursive_mutex_cond_unlock");
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
  // Windows need special handling since it has recursive mutexes, but
  // does not integrate them into a condition variable.
#    if defined (ACE_WIN32)
  // For Windows, the OS takes care of the mutex and its recursion. We just
  // need to release the lock one fewer times than this thread has acquired
  // it. Remember how many times, and reacquire it that many more times when
  // the condition is signaled.

  // We're using undocumented fields in the CRITICAL_SECTION structure
  // and they've been known to change across Windows variants and versions./
  // So be careful if you need to change these - there may be other
  // Windows variants that depend on existing values and limits.

  state.relock_count_ = 0;
  while (m->LockCount > 0 && m->RecursionCount > 1)
    {
      // This may fail if the current thread doesn't own the mutex. If it
      // does fail, it'll be on the first try, so don't worry about resetting
      // the state.
      if (ACE_OS::recursive_mutex_unlock (m) == -1)
        return -1;
      ++state.relock_count_;
    }
#    else /* not ACE_WIN32 */
    // prevent warnings for unused variables
    ACE_UNUSED_ARG (state);
    ACE_UNUSED_ARG (m);
#    endif /* ACE_WIN32 */
  return 0;
#  else /* ACE_HAS_RECURSIVE_MUTEXES */
  // For platforms without recursive mutexes, we obtain the nesting mutex
  // to gain control over the mutex internals. Then set the internals to say
  // the mutex is available. If there are waiters, signal the condition
  // to notify them (this is mostly like the recursive_mutex_unlock() method).
  // Then, return with the nesting mutex still held. The condition wait
  // will release it atomically, allowing mutex waiters to continue.
  // Note that this arrangement relies on the fact that on return from
  // the condition wait, this thread will again own the nesting mutex
  // and can either set the mutex internals directly or get in line for
  // the mutex... this part is handled in recursive_mutex_cond_relock().
  if (ACE_OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    return -1;

#    if !defined (ACE_NDEBUG)
  if (m->nesting_level_ == 0
      || ACE_OS::thr_equal (ACE_OS::thr_self (), m->owner_id_) == 0)
    {
      ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);
      errno = EINVAL;
      return -1;
    }
#    endif /* ACE_NDEBUG */

  // To make error recovery a bit easier, signal the condition now. Any
  // waiter won't regain control until the mutex is released, which won't
  // be until the caller returns and does the wait on the condition.
  if (ACE_OS::cond_signal (&m->lock_available_) == -1)
    {
      // Save/restore errno.
      ACE_Errno_Guard error (errno);
      ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);
      return -1;
    }

  // Ok, the nesting_mutex_ lock is still held, the condition has been
  // signaled... reset the nesting info and return _WITH_ the lock
  // held. The lock will be released when the condition waits, in the
  // caller.
  state.nesting_level_ = m->nesting_level_;
  state.owner_id_ = m->owner_id_;
  m->nesting_level_ = 0;
  m->owner_id_ = ACE_OS::NULL_thread;
  return 0;
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (state);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}


// This method is called after waiting on a condition variable when a
// recursive mutex must be reacquired. If the platform doesn't natively
// integrate recursive mutexes and condition variables, it's taken care
// of here (inverse of ACE_OS::recursive_mutex_cond_unlock).
ACE_INLINE void
ACE_OS::recursive_mutex_cond_relock (ACE_recursive_thread_mutex_t *m,
                                     ACE_recursive_mutex_state &state)
{
#if defined (ACE_HAS_THREADS)
  ACE_OS_TRACE ("ACE_OS::recursive_mutex_cond_relock");
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
  // Windows need special handling since it has recursive mutexes, but
  // does not integrate them into a condition variable.
  // On entry, the OS has already reacquired the lock for us. Just
  // reacquire it the proper number of times so the recursion is the same as
  // before waiting on the condition.
#    if defined (ACE_WIN32)
  while (state.relock_count_ > 0)
    {
      ACE_OS::recursive_mutex_lock (m);
      --state.relock_count_;
    }
  return;
#    else /* not ACE_WIN32 */
    // prevent warnings for unused variables
    ACE_UNUSED_ARG (state);
    ACE_UNUSED_ARG (m);

#    endif /* ACE_WIN32 */
#  else
  // Without recursive mutex support, it's somewhat trickier. On entry,
  // the current thread holds the nesting_mutex_, but another thread may
  // still be holding the ACE_recursive_mutex_t. If so, mimic the code
  // in ACE_OS::recursive_mutex_lock that waits to acquire the mutex.
  // After acquiring it, restore the nesting counts and release the
  // nesting mutex. This will restore the conditions to what they were
  // before calling ACE_OS::recursive_mutex_cond_unlock().
  while (m->nesting_level_ > 0)
    ACE_OS::cond_wait (&m->lock_available_, &m->nesting_mutex_);

  // At this point, we still have nesting_mutex_ and the mutex is free.
  m->nesting_level_ = state.nesting_level_;
  m->owner_id_ = state.owner_id_;
  ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);
  return;
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (state);
  return;
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::recursive_mutex_destroy (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return ACE_OS::thread_mutex_destroy (m);
#else
  if (ACE_OS::cond_destroy (&m->lock_available_) == -1
      || ACE_OS::thread_mutex_destroy (&m->nesting_mutex_) == -1)
    return -1;
  return 0;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::recursive_mutex_init (ACE_recursive_thread_mutex_t *m,
                              const ACE_TCHAR *name,
                              ACE_mutexattr_t *arg,
                              LPSECURITY_ATTRIBUTES sa)
{
  ACE_UNUSED_ARG (sa);
#if defined (ACE_HAS_THREADS)
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
#    if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  return ACE_OS::thread_mutex_init (m, PTHREAD_MUTEX_RECURSIVE, name, arg);
#    else
  return ACE_OS::thread_mutex_init (m, 0, name, arg);
#    endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
#  else
  if (ACE_OS::thread_mutex_init (&m->nesting_mutex_, 0, name, arg) == -1)
    return -1;
  else if (ACE_OS::cond_init (&m->lock_available_,
                              (short) USYNC_THREAD,
                              name,
                              0) == -1)
    return -1;
  else
    {
      m->nesting_level_ = 0;
      m->owner_id_ = ACE_OS::NULL_thread;
      return 0;
    }
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::recursive_mutex_lock (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return ACE_OS::thread_mutex_lock (m);
#else
  ACE_thread_t const t_id = ACE_OS::thr_self ();
  int result = 0;

  // Acquire the guard.
  if (ACE_OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    result = -1;
  else
  {
    // If there's no contention, just grab the lock immediately
    // (since this is the common case we'll optimize for it).
    if (m->nesting_level_ == 0)
      m->owner_id_ = t_id;
      // If we already own the lock, then increment the nesting level
      // and return.
    else if (ACE_OS::thr_equal (t_id, m->owner_id_) == 0)
    {
          // Wait until the nesting level has dropped to zero, at
          // which point we can acquire the lock.
      while (m->nesting_level_ > 0)
        ACE_OS::cond_wait (&m->lock_available_,
                            &m->nesting_mutex_);

          // At this point the nesting_mutex_ is held...
      m->owner_id_ = t_id;
    }

    // At this point, we can safely increment the nesting_level_ no
    // matter how we got here!
    ++m->nesting_level_;
  }

  {
    // Save/restore errno.
    ACE_Errno_Guard error (errno);
    ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);
  }
  return result;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::recursive_mutex_lock (ACE_recursive_thread_mutex_t *m,
                              const ACE_Time_Value &timeout)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return ACE_OS::thread_mutex_lock (m, timeout);
#else
  ACE_thread_t t_id = ACE_OS::thr_self ();
  int result = 0;

  // Try to acquire the guard.
  if (ACE_OS::thread_mutex_lock (&m->nesting_mutex_, timeout) == -1)
    result = -1;
  else
    {
      // If there's no contention, just grab the lock immediately
      // (since this is the common case we'll optimize for it).
      if (m->nesting_level_ == 0)
        m->owner_id_ = t_id;
      // If we already own the lock, then increment the nesting level
      // and return.
      else if (ACE_OS::thr_equal (t_id, m->owner_id_) == 0)
        {
          // Wait until the nesting level has dropped to zero, at
          // which point we can acquire the lock.
          while (m->nesting_level_ > 0)
            {
              result = ACE_OS::cond_timedwait (&m->lock_available_,
                                               &m->nesting_mutex_,
                                               const_cast <ACE_Time_Value *> (&timeout));

              // The mutex is reacquired even in the case of a timeout
              // release the mutex to prevent a deadlock
              if (result == -1)
                {
                  // Save/restore errno.
                  ACE_Errno_Guard error (errno);
                  ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);

                  return result;
                }
            }

          // At this point the nesting_mutex_ is held...
          m->owner_id_ = t_id;
        }

      // At this point, we can safely increment the nesting_level_ no
      // matter how we got here!
      m->nesting_level_++;

      // Save/restore errno.
      ACE_Errno_Guard error (errno);
      ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);
    }
  return result;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::recursive_mutex_lock (ACE_recursive_thread_mutex_t *m,
                              const ACE_Time_Value *timeout)
{
  return timeout == 0
    ? ACE_OS::recursive_mutex_lock (m)
    : ACE_OS::recursive_mutex_lock (m, *timeout);
}

ACE_INLINE int
ACE_OS::recursive_mutex_trylock (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return ACE_OS::thread_mutex_trylock (m);
#else
  ACE_thread_t t_id = ACE_OS::thr_self ();
  int result = 0;

  // Acquire the guard.
  if (ACE_OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    result = -1;
  else
  {
      // If there's no contention, just grab the lock immediately.
    if (m->nesting_level_ == 0)
    {
      m->owner_id_ = t_id;
      m->nesting_level_ = 1;
    }
      // If we already own the lock, then increment the nesting level
      // and proceed.
    else if (ACE_OS::thr_equal (t_id, m->owner_id_))
      m->nesting_level_++;
    else
    {
      errno = EBUSY;
      result = -1;
    }
  }

  {
    // Save/restore errno.
    ACE_Errno_Guard error (errno);
    ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);
  }
  return result;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::recursive_mutex_unlock (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return ACE_OS::thread_mutex_unlock (m);
#  else
  ACE_OS_TRACE ("ACE_OS::recursive_mutex_unlock");
#    if !defined (ACE_NDEBUG)
  ACE_thread_t t_id = ACE_OS::thr_self ();
#    endif /* ACE_NDEBUG */
  int result = 0;

  if (ACE_OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    result = -1;
  else
  {
#    if !defined (ACE_NDEBUG)
      if (m->nesting_level_ == 0
          || ACE_OS::thr_equal (t_id, m->owner_id_) == 0)
{
  errno = EINVAL;
  result = -1;
}
      else
#    endif /* ACE_NDEBUG */
{
  m->nesting_level_--;
  if (m->nesting_level_ == 0)
  {
              // This may not be strictly necessary, but it does put
              // the mutex into a known state...
    m->owner_id_ = ACE_OS::NULL_thread;

              // Inform a waiter that the lock is free.
    if (ACE_OS::cond_signal (&m->lock_available_) == -1)
      result = -1;
  }
}
  }

{
    // Save/restore errno.
  ACE_Errno_Guard error (errno);
  ACE_OS::thread_mutex_unlock (&m->nesting_mutex_);
}
  return result;
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::rw_rdlock (ACE_rwlock_t *rw)
{
  ACE_OS_TRACE ("ACE_OS::rw_rdlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_rdlock (rw),
                                       result),
                     int);
#  else /* ACE_HAS_PTHREADS_UNIX98_EXT */
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_rdlock (rw), result), int);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_PUSH (&rw->lock_);
#   endif /* ACE_HAS_PTHREADS */
  int result = 0;
  if (ACE_OS::mutex_lock (&rw->lock_) == -1)
    result = -1; // -1 means didn't get the mutex.
  else
    {
      // Give preference to writers who are waiting.
      while (rw->ref_count_ < 0 || rw->num_waiting_writers_ > 0)
        {
          rw->num_waiting_readers_++;
          if (ACE_OS::cond_wait (&rw->waiting_readers_, &rw->lock_) == -1)
            {
              result = -2; // -2 means that we need to release the mutex.
              break;
            }
          rw->num_waiting_readers_--;
        }
    }
  if (result == 0)
    rw->ref_count_++;
  if (result != -1)
    ACE_OS::mutex_unlock (&rw->lock_);
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_POP (0);
#   endif /* defined (ACE_HAS_PTHREADS) */
  return 0;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::rw_tryrdlock (ACE_rwlock_t *rw)
{
  ACE_OS_TRACE ("ACE_OS::rw_tryrdlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_tryrdlock (rw),
                                       result),
                     int);
#  else /* ACE_HAS_PTHREADS_UNIX98_EXT */
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_tryrdlock (rw), result), int);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  int result = -1;

  if (ACE_OS::mutex_lock (&rw->lock_) != -1)
    {
      ACE_Errno_Guard error (errno);

      if (rw->ref_count_ == -1 || rw->num_waiting_writers_ > 0)
        {
          error = EBUSY;
          result = -1;
        }
      else
        {
          rw->ref_count_++;
          result = 0;
        }

      ACE_OS::mutex_unlock (&rw->lock_);
    }
  return result;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::rw_trywrlock (ACE_rwlock_t *rw)
{
  ACE_OS_TRACE ("ACE_OS::rw_trywrlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_trywrlock (rw),
                                       result),
                     int);
#  else /* ACE_HAS_PTHREADS_UNIX98_EXT */
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_trywrlock (rw), result), int);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  int result = -1;

  if (ACE_OS::mutex_lock (&rw->lock_) != -1)
    {
      ACE_Errno_Guard error (errno);

      if (rw->ref_count_ != 0)
        {
          error = EBUSY;
          result = -1;
        }
      else
        {
          rw->ref_count_ = -1;
          result = 0;
        }

      ACE_OS::mutex_unlock (&rw->lock_);
    }
  return result;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

// Note that the caller of this method *must* already possess this
// lock as a read lock.
// return {-1 and no errno set means: error,
//         -1 and errno==EBUSY set means: could not upgrade,
//         0 means: upgraded successfully}

ACE_INLINE int
ACE_OS::rw_trywrlock_upgrade (ACE_rwlock_t *rw)
{
  ACE_OS_TRACE ("ACE_OS::rw_trywrlock_upgrade");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS_UNIX98_EXT) && !defined (ACE_LACKS_RWLOCK_T)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_trywrlock (rw),
                                       result),
                     int);
# elif !defined (ACE_LACKS_RWLOCK_T)
  // Some native rwlocks
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
# else /* NT, POSIX, and VxWorks don't support this natively. */
  // The ACE rwlock emulation does support upgrade . . .
  int result = 0;

#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_PUSH (&rw->lock_);
#   endif /* defined (ACE_HAS_PTHREADS) */

  if (ACE_OS::mutex_lock (&rw->lock_) == -1)
    return -1;
    // -1 means didn't get the mutex, error
  else if (rw->important_writer_)
    // an other reader upgrades already
    {
      result = -1;
      errno = EBUSY;
    }
  else
    {
      while (rw->ref_count_ > 1) // wait until only I am left
        {
          rw->num_waiting_writers_++; // prohibit any more readers
          rw->important_writer_ = true;

          if (ACE_OS::cond_wait (&rw->waiting_important_writer_, &rw->lock_) == -1)
            {
              result = -1;
              // we know that we have the lock again, we have this guarantee,
              // but something went wrong
            }
          rw->important_writer_ = false;
          rw->num_waiting_writers_--;
        }
      if (result == 0)
        {
          // nothing bad happend
          rw->ref_count_ = -1;
          // now I am a writer
          // everything is O.K.
        }
    }

  ACE_OS::mutex_unlock (&rw->lock_);

#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_POP (0);
#   endif /* defined (ACE_HAS_PTHREADS) */

  return result;

# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::rw_unlock (ACE_rwlock_t *rw)
{
  ACE_OS_TRACE ("ACE_OS::rw_unlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_unlock (rw),
                                       result),
                     int);
#  else /* ACE_HAS_PTHREADS_UNIX98_EXT */
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_unlock (rw), result), int);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  if (ACE_OS::mutex_lock (&rw->lock_) == -1)
    return -1;

  if (rw->ref_count_ > 0) // Releasing a reader.
    rw->ref_count_--;
  else if (rw->ref_count_ == -1) // Releasing a writer.
    rw->ref_count_ = 0;
  else
    {
      (void) ACE_OS::mutex_unlock (&rw->lock_);
      return -1; // @@ ACE_ASSERT (!"count should not be 0!\n");
    }

  int result = 0;
  ACE_Errno_Guard error (errno);

  if (rw->important_writer_ && rw->ref_count_ == 1)
    // only the reader requesting to upgrade its lock is left over.
    {
      result = ACE_OS::cond_signal (&rw->waiting_important_writer_);
      error = errno;
    }
  else if (rw->num_waiting_writers_ > 0 && rw->ref_count_ == 0)
    // give preference to writers over readers...
    {
      result = ACE_OS::cond_signal (&rw->waiting_writers_);
      error =  errno;
    }
  else if (rw->num_waiting_readers_ > 0 && rw->num_waiting_writers_ == 0)
    {
      result = ACE_OS::cond_broadcast (&rw->waiting_readers_);
      error = errno;
    }

  (void) ACE_OS::mutex_unlock (&rw->lock_);
  return result;
# endif /* ! ace_lacks_rwlock_t */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ace_has_threads */
}

ACE_INLINE int
ACE_OS::rw_wrlock (ACE_rwlock_t *rw)
{
  ACE_OS_TRACE ("ACE_OS::rw_wrlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_wrlock (rw),
                                       result),
                     int);
#  else /* ACE_HAS_PTHREADS_UNIX98_EXT */
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_wrlock (rw), result), int);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_PUSH (&rw->lock_);
#   endif /* defined (ACE_HAS_PTHREADS) */
  int result = 0;

  if (ACE_OS::mutex_lock (&rw->lock_) == -1)
    result = -1; // -1 means didn't get the mutex.
  else
    {
      while (rw->ref_count_ != 0)
        {
          rw->num_waiting_writers_++;

          if (ACE_OS::cond_wait (&rw->waiting_writers_, &rw->lock_) == -1)
            {
              result = -2; // -2 means we need to release the mutex.
              break;
            }

          rw->num_waiting_writers_--;
        }
    }
  if (result == 0)
    rw->ref_count_ = -1;
  if (result != -1)
    ACE_OS::mutex_unlock (&rw->lock_);
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_POP (0);
#   endif /* defined (ACE_HAS_PTHREADS) */
  return 0;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::rwlock_destroy (ACE_rwlock_t *rw)
{
  ACE_OS_TRACE ("ACE_OS::rwlock_destroy");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_destroy (rw),
                                       result),
                     int);
#  else /* ACE_HAS_PTHREADS_UNIX98_EXT */
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::rwlock_destroy (rw), result), int);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  ACE_OS::mutex_destroy (&rw->lock_);
  ACE_OS::cond_destroy (&rw->waiting_readers_);
  ACE_OS::cond_destroy (&rw->waiting_important_writer_);
  return ACE_OS::cond_destroy (&rw->waiting_writers_);
# endif /* !defined (ACE_LACKS_RWLOCK_T) */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_THREADS) && !defined (ACE_LACKS_RWLOCK_T)
ACE_INLINE int
ACE_OS::rwlock_init (ACE_rwlock_t *rw,
                     int type,
                     const ACE_TCHAR *name,
                     void *arg)
{
  // ACE_OS_TRACE ("ACE_OS::rwlock_init");
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);

  int status;
  pthread_rwlockattr_t attr;
  pthread_rwlockattr_init (&attr);
#    if !defined (ACE_LACKS_RWLOCKATTR_PSHARED)
  pthread_rwlockattr_setpshared (&attr, (type == USYNC_THREAD ?
                                         PTHREAD_PROCESS_PRIVATE :
                                         PTHREAD_PROCESS_SHARED));
#    else
  ACE_UNUSED_ARG (type);
#    endif /* !ACE_LACKS_RWLOCKATTR_PSHARED */
  status = ACE_ADAPT_RETVAL (pthread_rwlock_init (rw, &attr), status);
  pthread_rwlockattr_destroy (&attr);

  return status;

#  else
  type = type;
  name = name;
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::rwlock_init (rw, type, arg), result), int);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
}
#endif /* ACE_HAS_THREADS && !defined (ACE_LACKS_RWLOCK_T) */

ACE_INLINE int
ACE_OS::sema_destroy (ACE_sema_t *s)
{
  ACE_OS_TRACE ("ACE_OS::sema_destroy");
#if defined (ACE_HAS_POSIX_SEM)
  int result = 0;
# if !defined (ACE_HAS_POSIX_SEM_TIMEOUT) && !defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  ACE_OS::mutex_destroy (&s->lock_);
  ACE_OS::cond_destroy (&s->count_nonzero_);
# endif /* !ACE_HAS_POSIX_SEM_TIMEOUT && !ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION */
# if defined (ACE_LACKS_NAMED_POSIX_SEM)
  if (s->name_)
    {
      // Only destroy the semaphore if we're the ones who
      // initialized it.
#  if !defined (ACE_LACKS_SEM_DESTROY)
      ACE_OSCALL (::sem_destroy (s->sema_),int, result);
#  endif /* ACE_LACKS_SEM_DESTROY */
      ACE_OS::shm_unlink (s->name_);
      delete s->name_;
      return result;
    }
# else
  if (s->name_)
    {
      if (!s->avoid_unlink_)
        ACE_OS::sema_unlink (s->name_);
#if defined (ACE_HAS_ALLOC_HOOKS)
      ACE_Allocator::instance()->free ((void *) s->name_);
#else
      ACE_OS::free ((void *) s->name_);
#endif /* ACE_HAS_ALLOC_HOOKS */
      return ::sem_close (s->sema_);
    }
# endif /*  ACE_LACKS_NAMED_POSIX_SEM */
  else
    {
# if !defined (ACE_LACKS_UNNAMED_SEMAPHORE) && !defined (ACE_LACKS_SEM_DESTROY)
      ACE_OSCALL (::sem_destroy (s->sema_), int, result);
# endif /* !ACE_LACKS_UNNAMED_SEMAPHORE  && !ACE_LACKS_SEM_DESTROY */
# if defined (ACE_LACKS_NAMED_POSIX_SEM)
      if (s->new_sema_)
# endif /* ACE_LACKS_NAMED_POSIX_SEM */
#if defined (ACE_HAS_ALLOC_HOOKS)
        ACE_Allocator::instance()->free(s->sema_);
#else
        delete s->sema_;
#endif /* ACE_HAS_ALLOC_HOOKS */
      s->sema_ = 0;
      return result;
    }
#elif defined (ACE_USES_FIFO_SEM)
  int r0 = 0;
  if (s->name_)
    {
      r0 = ACE_OS::unlink (s->name_);
#if defined (ACE_HAS_ALLOC_HOOKS)
      ACE_Allocator::instance()->free ((void *) s->name_);
#else
      ACE_OS::free ((void *) s->name_);
#endif /* ACE_HAS_ALLOC_HOOKS */
      s->name_ = 0;
    }
  int r1 = ACE_OS::close (s->fd_[0]);      /* ignore error */
  int r2 = ACE_OS::close (s->fd_[1]);      /* ignore error */
  return r0 != 0 || r1 != 0 || r2 != 0 ? -1 : 0;
#elif defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  int r1 = ACE_OS::mutex_destroy (&s->lock_);
  int r2 = ACE_OS::cond_destroy (&s->count_nonzero_);
  return r1 != 0 || r2 != 0 ? -1 : 0;
# elif defined (ACE_HAS_WTHREADS)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CloseHandle (*s), ace_result_), int, -1);
# elif defined (ACE_VXWORKS)
  int result;
  ACE_OSCALL (::semDelete (s->sema_), int, result);
  s->sema_ = 0;
  return result;
# elif defined (ACE_INTEGRITY) && !defined (ACE_INTEGRITY178B)
  return ::CloseSemaphore (*s) == Success ? 0 : -1;
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_POSIX_SEM */
}

// NOTE: The previous four function definitions must appear before
// ACE_OS::sema_init ().
ACE_INLINE int
ACE_OS::sema_init (ACE_sema_t *s,
                   u_int count,
                   int type,
                   const char *name,
                   void *arg,
                   int max,
                   LPSECURITY_ATTRIBUTES sa)
{
  ACE_condattr_t *pattr = 0;
  return ACE_OS::sema_init (s, count, type, pattr, name, arg, max, sa);
}

ACE_INLINE int
ACE_OS::sema_init (ACE_sema_t *s,
                   u_int count,
                   int type,
                   ACE_condattr_t *attributes,
                   const char *name,
                   void *arg,
                   int max,
                   LPSECURITY_ATTRIBUTES sa)
{
  ACE_OS_TRACE ("ACE_OS::sema_init");
#if defined (ACE_HAS_POSIX_SEM)
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);

  s->name_ = 0;
  s->avoid_unlink_ = false;
#  if defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (attributes);
#  else
  int result = -1;

  if (ACE_OS::mutex_init (&s->lock_, type, name,
                          (ACE_mutexattr_t *) arg) == 0
      && (attributes == 0 ?
          ACE_OS::cond_init (&s->count_nonzero_, (short)type, name, arg) :
            ACE_OS::cond_init (&s->count_nonzero_, *attributes, name, arg)) == 0
      && ACE_OS::mutex_lock (&s->lock_) == 0)
    {
      if (ACE_OS::mutex_unlock (&s->lock_) == 0)
        result = 0;
    }

  if (result == -1)
    {
      ACE_OS::mutex_destroy (&s->lock_);
      ACE_OS::cond_destroy (&s->count_nonzero_);
      return result;
    }
#  endif /* ACE_HAS_POSIX_SEM_TIMEOUT || ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION */

#  if defined (ACE_LACKS_NAMED_POSIX_SEM)
  s->new_sema_ = false;
  if (type == USYNC_PROCESS)
    {
      // Let's see if it already exists.
      ACE_HANDLE fd = ACE_OS::shm_open (ACE_TEXT_CHAR_TO_TCHAR (name),
                                        O_RDWR | O_CREAT | O_EXCL,
                                        ACE_DEFAULT_FILE_PERMS);
      if (fd == ACE_INVALID_HANDLE)
        {
          if (errno == EEXIST)
            fd = ACE_OS::shm_open (ACE_TEXT_CHAR_TO_TCHAR (name),
                                   O_RDWR | O_CREAT,
                                   ACE_DEFAULT_FILE_PERMS);
          else
            return -1;
        }
      else
        {
          // We own this shared memory object!  Let's set its
          // size.
          if (ACE_OS::ftruncate (fd,
                                 sizeof (ACE_sema_t)) == -1)
            return -1;
          s->name_ = ACE_OS::strdup (name);
          if (s->name_ == 0)
            return -1;
        }
      if (fd == -1)
        return -1;

      s->sema_ = (sem_t *)
        ACE_OS::mmap (0,
                      sizeof (ACE_sema_t),
                      PROT_RDWR,
                      MAP_SHARED,
                      fd,
                      0);
      ACE_OS::close (fd);
      if (s->sema_ == (sem_t *) MAP_FAILED)
        return -1;
      if (s->name_
          // @@ According UNIX Network Programming V2 by Stevens,
          //    sem_init() is currently not required to return zero on
          //    success, but it *does* return -1 upon failure.  For
          //    this reason, check for failure by comparing to -1,
          //    instead of checking for success by comparing to zero.
          //        -Ossama
          // Only initialize it if we're the one who created it.
          && ::sem_init (s->sema_, type == USYNC_PROCESS, count) == -1)
        return -1;
      return 0;
    }
#  else
  if (name)
    {
      ACE_ALLOCATOR_RETURN (s->name_,
                            ACE_OS::strdup (name),
                            -1);
      s->sema_ = ::sem_open (s->name_,
                             O_CREAT,
                             ACE_DEFAULT_FILE_PERMS,
                             count);
      if (s->sema_ == (sem_t *) SEM_FAILED)
        return -1;
      else
        return 0;
    }
#  endif /* ACE_LACKS_NAMED_POSIX_SEM */
  else
    {
#  if defined (ACE_LACKS_UNNAMED_SEMAPHORE)
      ACE_NOTSUP_RETURN (-1);
#  else
#if defined (ACE_HAS_ALLOC_HOOKS)
      ACE_ALLOCATOR_RETURN (s->sema_,
                            static_cast<sem_t*>(ACE_Allocator::instance()->malloc(sizeof(sem_t))),
                            -1);
#else
      ACE_NEW_RETURN (s->sema_,
                      sem_t,
                      -1);
#endif /* ACE_HAS_ALLOC_HOOKS */

#   if defined (ACE_LACKS_NAMED_POSIX_SEM)
      s->new_sema_ = true;
#   endif /* ACE_LACKS_NAMED_POSIX_SEM */
      ACE_OS::memset(s->sema_, 0, sizeof(*s->sema_));
      return ::sem_init (s->sema_, type != USYNC_THREAD, count);
#  endif /* ACE_LACKS_UNNAMED_SEMAPHORE */
    }

#elif defined (ACE_USES_FIFO_SEM)
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  int             flags = 0;
  mode_t          mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP;

  if (type == USYNC_THREAD)
    {
      // Create systemwide unique name for semaphore
      char   uname[ACE_UNIQUE_NAME_LEN];
      ACE_OS::unique_name ((const void *) s,
                            uname,
                            ACE_UNIQUE_NAME_LEN);
      name = &uname[0];
    }

  s->name_ = 0;
  s->fd_[0] = s->fd_[1] = ACE_INVALID_HANDLE;
  bool creator = false;

  if (ACE_OS::mkfifo (ACE_TEXT_CHAR_TO_TCHAR (name), mode) < 0)
    {
      if (errno != EEXIST)    /* already exists OK else ERR */
        return -1;
      // check if this is a real FIFO, not just some other existing file
      ACE_stat fs;
      if (ACE_OS::stat (name, &fs))
        return -1;
      if (!S_ISFIFO (fs.st_mode))
        {
          // existing file is not a FIFO
          errno = EEXIST;
          return -1;
        }
    }
    else
      creator = true; // remember we created it for initialization at end

  // for processshared semaphores remember who we are to be able to remove
  // the FIFO when we're done with it
  if (type == USYNC_PROCESS)
    {
      s->name_ = ACE_OS::strdup (name);
      if (s->name_ == 0)
        {
          if (creator)
            ACE_OS::unlink (name);
          return -1;
        }
    }

  if ((s->fd_[0] = ACE_OS::open (name, O_RDONLY | O_NONBLOCK)) == ACE_INVALID_HANDLE
      || (s->fd_[1] = ACE_OS::open (name, O_WRONLY | O_NONBLOCK)) == ACE_INVALID_HANDLE)
    return -1;

  /* turn off nonblocking for fd_[0] */
  if ((flags = ACE_OS::fcntl (s->fd_[0], F_GETFL, 0)) < 0)
    return -1;

  flags &= ~O_NONBLOCK;
  if (ACE_OS::fcntl (s->fd_[0], F_SETFL, flags) < 0)
    return -1;

  //if (s->name_ && count)
  if (creator && count)
    {
      char    c = 1;
      for (u_int i=0; i<count ;++i)
        if (ACE_OS::write (s->fd_[1], &c, sizeof (char)) != 1)
          return -1;
    }

  // In the case of process scope semaphores we can already unlink the FIFO now that
  // we completely set it up (the opened handles will keep it active until we close
  // thos down). This way we're protected against unexpected crashes as far as removal
  // is concerned.
  // Unfortunately this does not work for processshared FIFOs since as soon as we
  // have unlinked the semaphore no other process will be able to open it anymore.
  if (type == USYNC_THREAD)
    {
      ACE_OS::unlink (name);
    }

  return 0;
#elif defined (ACE_HAS_THREADS)
#  if defined (ACE_HAS_PTHREADS)
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  int result = -1;

  if (ACE_OS::mutex_init (&s->lock_, type, name,
                          (ACE_mutexattr_t *) arg) == 0
      && (attributes == 0 ?
          ACE_OS::cond_init (&s->count_nonzero_, (short)type, name, arg) :
            ACE_OS::cond_init (&s->count_nonzero_, *attributes, name, arg)) == 0
      && ACE_OS::mutex_lock (&s->lock_) == 0)
    {
      s->count_ = count;
      s->waiters_ = 0;

      if (ACE_OS::mutex_unlock (&s->lock_) == 0)
        result = 0;
    }

  if (result == -1)
    {
      ACE_OS::mutex_destroy (&s->lock_);
      ACE_OS::cond_destroy (&s->count_nonzero_);
    }
  return result;
#  elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (arg);
  // Create the semaphore with its value initialized to <count> and
  // its maximum value initialized to <max>.
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  *s = ACE_TEXT_CreateSemaphore
    (ACE_OS::default_win32_security_attributes_r (sa, &sa_buffer, &sd_buffer),
     count,
     max,
     ACE_TEXT_CHAR_TO_TCHAR (name));

  if (*s == 0)
    ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
  else
    {
      // Make sure to set errno to ERROR_ALREADY_EXISTS if necessary.
      ACE_OS::set_errno_to_last_error ();
      return 0;
    }
#  elif defined (ACE_VXWORKS)
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  s->name_ = 0;
  s->sema_ = ::semCCreate (type, count);
  return s->sema_ ? 0 : -1;
#  elif defined (ACE_INTEGRITY)
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  return ::CreateSemaphore (count, s) == Success ? 0 : -1;
#  endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (count);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_POSIX_SEM */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::sema_init (ACE_sema_t *s,
                   u_int count,
                   int type,
                   const wchar_t *name,
                   void *arg,
                   int max,
                   LPSECURITY_ATTRIBUTES sa)
{
  ACE_condattr_t *pattr = 0;
  return ACE_OS::sema_init (s, count, type, pattr, name, arg, max, sa);
}

ACE_INLINE int
ACE_OS::sema_init (ACE_sema_t *s,
                   u_int count,
                   int type,
                   ACE_condattr_t *attributes,
                   const wchar_t *name,
                   void *arg,
                   int max,
                   LPSECURITY_ATTRIBUTES sa)
{
# if defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (arg);
  // Create the semaphore with its value initialized to <count> and
  // its maximum value initialized to <max>.
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  *s = ::CreateSemaphoreW
    (ACE_OS::default_win32_security_attributes_r (sa, &sa_buffer, &sd_buffer),
     count,
     max,
     name);

  if (*s == 0)
    ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
  else
    {
      // Make sure to set errno to ERROR_ALREADY_EXISTS if necessary.
      ACE_OS::set_errno_to_last_error ();
      return 0;
    }
# else /* ACE_HAS_WTHREADS */
  // Just call the normal char version.
  return ACE_OS::sema_init (s, count, type, attributes, ACE_Wide_To_Ascii (name).char_rep (), arg, max, sa);
# endif /* ACE_HAS_WTHREADS */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE void
ACE_OS::sema_avoid_unlink (ACE_sema_t *s, bool avoid_unlink)
{
#if defined (ACE_HAS_POSIX_SEM)
  s->avoid_unlink_ = avoid_unlink;
#else
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (avoid_unlink);
#endif
}

ACE_INLINE int
ACE_OS::sema_unlink (const char *name)
{
#if defined (ACE_HAS_POSIX_SEM) && !defined (ACE_LACKS_SEM_UNLINK)
  return ::sem_unlink (name);
#else
  ACE_UNUSED_ARG (name);
  ACE_NOTSUP_RETURN (-1);
#endif
}

ACE_INLINE int
ACE_OS::sema_post (ACE_sema_t *s)
{
  ACE_OS_TRACE ("ACE_OS::sema_post");
# if defined (ACE_HAS_POSIX_SEM)
#   if defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  return ::sem_post (s->sema_);
#   else
  int result = -1;

  if (ACE_OS::mutex_lock (&s->lock_) == 0)
    {
      if (::sem_post (s->sema_) == 0)
        result = ACE_OS::cond_signal (&s->count_nonzero_);

      ACE_OS::mutex_unlock (&s->lock_);
    }
  return result;
#   endif /* ACE_HAS_POSIX_SEM_TIMEOUT || ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION */
# elif defined (ACE_USES_FIFO_SEM)
  char    c = 1;
  if (ACE_OS::write (s->fd_[1], &c, sizeof (char)) == sizeof (char))
    return 0;
  return -1;
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = -1;

  if (ACE_OS::mutex_lock (&s->lock_) == 0)
    {
      // Always allow a waiter to continue if there is one.
      if (s->waiters_ > 0)
        result = ACE_OS::cond_signal (&s->count_nonzero_);
      else
        result = 0;

      s->count_++;
      ACE_OS::mutex_unlock (&s->lock_);
    }
  return result;
#   elif defined (ACE_HAS_WTHREADS)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::ReleaseSemaphore (*s, 1, 0),
                                          ace_result_),
                        int, -1);
#   elif defined (ACE_VXWORKS)
  return ::semGive (s->sema_);
#   elif defined (ACE_INTEGRITY)
  return ::ReleaseSemaphore (*s) == Success ? 0 : -1;
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
ACE_OS::sema_post (ACE_sema_t *s, u_int release_count)
{
#if defined (ACE_WIN32)
  // Win32 supports this natively.
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::ReleaseSemaphore (*s, release_count, 0),
                                          ace_result_), int, -1);
#else
  // On POSIX platforms we need to emulate this ourselves.
  // @@ We can optimize on this implementation.  However,
  // the semaphore primitive on Win32 doesn't allow one
  // to increase a semaphore to more than the count it was
  // first initialized.  Posix doesn't seem to have
  // this restriction.  Should we impose the restriction in
  // our semaphore simulation?
  for (size_t i = 0; i < release_count; i++)
    if (ACE_OS::sema_post (s) == -1)
      return -1;

  return 0;
#endif /* ACE_WIN32 */
}

ACE_INLINE int
ACE_OS::sema_trywait (ACE_sema_t *s)
{
  ACE_OS_TRACE ("ACE_OS::sema_trywait");
# if defined (ACE_HAS_POSIX_SEM)
  // POSIX semaphores set errno to EAGAIN if trywait fails
  return ::sem_trywait (s->sema_);
# elif defined (ACE_USES_FIFO_SEM)
  char  c;
  int     rc, flags;

  /* turn on nonblocking for s->fd_[0] */
  if ((flags = ACE_OS::fcntl (s->fd_[0], F_GETFL, 0)) < 0)
    return -1;
  flags |= O_NONBLOCK;
  if (ACE_OS::fcntl (s->fd_[0], F_SETFL, flags) < 0)
    return -1;

  // read sets errno to EAGAIN if no input
  rc = ACE_OS::read (s->fd_[0], &c, sizeof (char));

  /* turn off nonblocking for fd_[0] */
  if ((flags = ACE_OS::fcntl (s->fd_[0], F_GETFL, 0)) >= 0)
  {
    flags &= ~O_NONBLOCK;
    ACE_OS::fcntl (s->fd_[0], F_SETFL, flags);
  }

  return rc == 1 ? 0 : (-1);
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = -1;

  if (ACE_OS::mutex_lock (&s->lock_) == 0)
    {
      if (s->count_ > 0)
        {
          --s->count_;
          result = 0;
        }
      else
        errno = EBUSY;

      ACE_OS::mutex_unlock (&s->lock_);
    }
  return result;
#   elif defined (ACE_HAS_WTHREADS)
  DWORD result = ::WaitForSingleObject (*s, 0);

  if (result == WAIT_OBJECT_0)
    return 0;
  else
    {
      if (result == WAIT_TIMEOUT)
        errno = EBUSY;
      else
        ACE_OS::set_errno_to_last_error ();
      // This is a hack, we need to find an appropriate mapping...
      return -1;
    }
#   elif defined (ACE_VXWORKS)
  if (::semTake (s->sema_, NO_WAIT) == ERROR)
    if (errno == S_objLib_OBJ_UNAVAILABLE)
      {
        // couldn't get the semaphore
        errno = EBUSY;
        return -1;
      }
    else
      // error
      return -1;
  else
    // got the semaphore
    return 0;
#   elif defined (ACE_INTEGRITY)
  return ::TryToObtainSemaphore (*s) == Success ? 0 : -1;
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
ACE_OS::sema_wait (ACE_sema_t *s)
{
  ACE_OS_TRACE ("ACE_OS::sema_wait");
# if defined (ACE_HAS_POSIX_SEM)
  return ::sem_wait (s->sema_);
# elif defined (ACE_USES_FIFO_SEM)
  char c;
  if (ACE_OS::read (s->fd_[0], &c, sizeof (char)) == 1)
    return 0;
  return -1;
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = 0;

  ACE_PTHREAD_CLEANUP_PUSH (&s->lock_);

  if (ACE_OS::mutex_lock (&s->lock_) != 0)
    result = -1;
  else
    {
      // Keep track of the number of waiters so that we can signal
      // them properly in <ACE_OS::sema_post>.
      s->waiters_++;

      // Wait until the semaphore count is > 0.
      while (s->count_ == 0)
        if (ACE_OS::cond_wait (&s->count_nonzero_,
                               &s->lock_) == -1)
          {
            result = -2; // -2 means that we need to release the mutex.
            break;
          }

      --s->waiters_;
    }

  if (result == 0)
    --s->count_;

  if (result != -1)
    ACE_OS::mutex_unlock (&s->lock_);
  ACE_PTHREAD_CLEANUP_POP (0);
  return result < 0 ? -1 : result;

#   elif defined (ACE_HAS_WTHREADS)
  switch (::WaitForSingleObject (*s, INFINITE))
    {
    case WAIT_OBJECT_0:
      return 0;
    default:
      // This is a hack, we need to find an appropriate mapping...
      ACE_OS::set_errno_to_last_error ();
      return -1;
    }
  /* NOTREACHED */
#   elif defined (ACE_VXWORKS)
  return ::semTake (s->sema_, WAIT_FOREVER);
#   elif defined (ACE_INTEGRITY)
  return ::WaitForSemaphore (*s) == Success ? 0 : -1;
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
ACE_OS::sema_wait (ACE_sema_t *s, ACE_Time_Value &tv)
{
  ACE_OS_TRACE ("ACE_OS::sema_wait");
# if defined (ACE_HAS_POSIX_SEM)
#   if defined (ACE_HAS_POSIX_SEM_TIMEOUT)
  int rc;
  timespec_t ts;
  ts = tv; // Calls ACE_Time_Value::operator timespec_t().
  ACE_OSCALL (::sem_timedwait (s->sema_, &ts), int, rc);
  if (rc == -1 && errno == ETIMEDOUT)
    errno = ETIME;  /* POSIX returns ETIMEDOUT but we need ETIME */
  return rc;
#   elif !defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  int result = 0;
  bool expired = false;
  ACE_Errno_Guard error (errno);

  ACE_PTHREAD_CLEANUP_PUSH (&s->lock_);

  if (ACE_OS::mutex_lock (&s->lock_) != 0)
    result = -2;
  else
    {
      bool finished = true;
      do
      {
        result = ACE_OS::sema_trywait (s);
        if (result == -1 && errno == EAGAIN)
          expired = (tv.to_relative_time () <= ACE_Time_Value::zero);
        else
          expired = false;

        finished = result != -1 || expired ||
                   (result == -1 && errno != EAGAIN);
        if (!finished)
          {
            if (ACE_OS::cond_timedwait (&s->count_nonzero_,
                                        &s->lock_,
                                        &tv) == -1)
              {
                error = errno;
                result = -1;
                break;
              }
          }
      } while (!finished);

      if (expired)
        error = ETIME;
    }

  if (result != -2)
    ACE_OS::mutex_unlock (&s->lock_);
  ACE_PTHREAD_CLEANUP_POP (0);
  return result < 0 ? -1 : result;
#   else /* No native sem_timedwait(), and emulation disabled */
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_POSIX_SEM_TIMEOUT */
# elif defined (ACE_USES_FIFO_SEM)
  int rc;
  ACE_Time_Value timeout = tv.to_relative_time ();

  while (timeout > ACE_Time_Value::zero)
    {
      ACE_Handle_Set  fds_;

      fds_.set_bit (s->fd_[0]);
      if ((rc = ACE_OS::select (ACE_Handle_Set::MAXSIZE, fds_, 0, 0, timeout)) != 1)
        {
          if (rc == 0 || errno != EAGAIN)
          {
            if (rc == 0)
              errno = ETIME;
            return -1;
          }
        }

      // try to read the signal *but* do *not* block
      if (rc == 1 && ACE_OS::sema_trywait (s) == 0)
        return 0;

      // we were woken for input but someone beat us to it
      // so we wait again if there is still time
      timeout = tv.to_relative_time ();
    }

  // make sure errno is set right
  errno = ETIME;

  return -1;
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = 0;
  ACE_Errno_Guard error (errno);

  ACE_PTHREAD_CLEANUP_PUSH (&s->lock_);

  if (ACE_OS::mutex_lock (&s->lock_) != 0)
    result = -1;
  else
    {
      // Keep track of the number of waiters so that we can signal
      // them properly in <ACE_OS::sema_post>.
      s->waiters_++;

      // Wait until the semaphore count is > 0 or until we time out.
      while (s->count_ == 0)
        if (ACE_OS::cond_timedwait (&s->count_nonzero_,
                                    &s->lock_,
                                    &tv) == -1)
          {
            error = errno;
            result = -2; // -2 means that we need to release the mutex.
            break;
          }

      --s->waiters_;
    }

  if (result == 0)
    {
      --s->count_;
    }

  if (result != -1)
    ACE_OS::mutex_unlock (&s->lock_);
  ACE_PTHREAD_CLEANUP_POP (0);
  return result < 0 ? -1 : result;
#   elif defined (ACE_HAS_WTHREADS)
  int msec_timeout;

  if (tv == ACE_Time_Value::zero)
    msec_timeout = 0; // Do a "poll."
  else
    {
      // Note that we must convert between absolute time (which is
      // passed as a parameter) and relative time (which is what
      // <WaitForSingleObjects> expects).
      ACE_Time_Value relative_time = tv.to_relative_time ();

      // Watchout for situations where a context switch has caused the
      // current time to be > the timeout.
      if (relative_time < ACE_Time_Value::zero)
        msec_timeout = 0;
      else
        msec_timeout = relative_time.msec ();
    }

  switch (::WaitForSingleObject (*s, msec_timeout))
    {
    case WAIT_OBJECT_0:
      tv = tv.now ();     // Update time to when acquired
      return 0;
    case WAIT_TIMEOUT:
      errno = ETIME;
      return -1;
    default:
      // This is a hack, we need to find an appropriate mapping...
      ACE_OS::set_errno_to_last_error ();
      return -1;
    }
  /* NOTREACHED */
#   elif defined (ACE_VXWORKS)
  // Note that we must convert between absolute time (which is
  // passed as a parameter) and relative time (which is what
  // the system call expects).
  ACE_Time_Value relative_time = tv.to_relative_time ();

  _Vx_freq_t const ticks_per_sec = ::sysClkRateGet ();

  int ticks = relative_time.sec () * ticks_per_sec +
              relative_time.usec () * ticks_per_sec / ACE_ONE_SECOND_IN_USECS;
  if (::semTake (s->sema_, ticks) == ERROR)
    {
      if (errno == S_objLib_OBJ_TIMEOUT)
        // Convert the VxWorks errno to one that's common for to ACE
        // platforms.
        errno = ETIME;
      else if (errno == S_objLib_OBJ_UNAVAILABLE)
        errno = EBUSY;
      return -1;
    }
  else
    {
      tv = tv.now ();  // Update to time acquired
      return 0;
    }
#   elif defined (ACE_INTEGRITY)
  // INTEGRITY has kernel calls to wait for semaphore with timeout.
  // Since we are imitating INTEGRITY-178 behavior, we're not using them now.
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
ACE_OS::sema_wait (ACE_sema_t *s, ACE_Time_Value *tv)
{
  return tv == 0 ? ACE_OS::sema_wait (s) : ACE_OS::sema_wait (s, *tv);
}

ACE_INLINE int
ACE_OS::semctl (int int_id, int semnum, int cmd, semun value)
{
  ACE_OS_TRACE ("ACE_OS::semctl");
#if defined (ACE_HAS_SYSV_IPC)
  return ::semctl (int_id, semnum, cmd, value);
#else
  ACE_UNUSED_ARG (int_id);
  ACE_UNUSED_ARG (semnum);
  ACE_UNUSED_ARG (cmd);
  ACE_UNUSED_ARG (value);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
ACE_OS::semget (key_t key, int nsems, int flags)
{
  ACE_OS_TRACE ("ACE_OS::semget");
#if defined (ACE_HAS_SYSV_IPC)
  return ::semget (key, nsems, flags);
#else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (nsems);
  ACE_UNUSED_ARG (flags);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
ACE_OS::semop (int int_id, struct sembuf *sops, size_t nsops)
{
  ACE_OS_TRACE ("ACE_OS::semop");
#if defined (ACE_HAS_SYSV_IPC)
  return ::semop (int_id, sops, nsops);
#else
  ACE_UNUSED_ARG (int_id);
  ACE_UNUSED_ARG (sops);
  ACE_UNUSED_ARG (nsops);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
ACE_OS::sigtimedwait (const sigset_t *sset,
                      siginfo_t *info,
                      const ACE_Time_Value *timeout)
{
  ACE_OS_TRACE ("ACE_OS::sigtimedwait");
#if defined (ACE_HAS_SIGTIMEDWAIT)
  timespec_t ts;
  timespec_t *tsp = 0;

  if (timeout != 0)
    {
      ts = *timeout; // Calls ACE_Time_Value::operator timespec_t().
      tsp = &ts;
    }

  return ::sigtimedwait (sset, info, tsp);
#else
    ACE_UNUSED_ARG (sset);
    ACE_UNUSED_ARG (info);
    ACE_UNUSED_ARG (timeout);
    ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SIGTIMEDWAIT */
}

ACE_INLINE int
ACE_OS::sigwait (sigset_t *sset, int *sig)
{
  ACE_OS_TRACE ("ACE_OS::sigwait");
  int local_sig;
  if (sig == 0)
    sig = &local_sig;
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS) || defined (ACE_LACKS_SIGWAIT)
    ACE_UNUSED_ARG (sset);
    ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_HAS_PTHREADS)
#   if defined (CYGWIN32)
      // Cygwin has sigwait definition, but it is not implemented
      ACE_UNUSED_ARG (sset);
      ACE_NOTSUP_RETURN (-1);
#   else   /* this is std */
      errno = ::sigwait (sset, sig);
      return errno == 0  ?  *sig  :  -1;
#   endif /* CYGWIN32 */
# elif defined (ACE_VXWORKS)
    // Second arg is a struct siginfo *, which we don't need (the
    // selected signal number is returned).  Third arg is timeout:  0
    // means forever.
    *sig = ::sigtimedwait (sset, 0, 0);
    return *sig;
# else
    ACE_UNUSED_ARG (sset);
    ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_WTHREADS || ACE_LACKS_SIGWAIT */
#else
    ACE_UNUSED_ARG (sset);
    ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::sigwaitinfo (const sigset_t *sset,
                     siginfo_t *info)
{
  ACE_OS_TRACE ("ACE_OS::sigwaitinfo");
  // If this platform has sigtimedwait, it should have sigwaitinfo as well.
  // If this isn't true somewhere, let me know and I'll fix this.
  // -Steve Huston <shuston@riverace.com>.
#if defined (ACE_HAS_SIGTIMEDWAIT)
  return ::sigwaitinfo (sset, info);
#else
  ACE_UNUSED_ARG (sset);
  ACE_UNUSED_ARG (info);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SIGTIMEDWAIT */
}

ACE_INLINE int
ACE_OS::thr_cancel (ACE_thread_t thr_id)
{
  ACE_OS_TRACE ("ACE_OS::thr_cancel");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cancel (thr_id),
                                        result),
                      int);
# elif defined (ACE_HAS_VXTHREADS)
  return ::taskDelete (thr_id);
# else /* Could be ACE_HAS_PTHREADS && ACE_LACKS_PTHREAD_CANCEL */
  ACE_UNUSED_ARG (thr_id);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (thr_id);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_cmp (ACE_hthread_t t1, ACE_hthread_t t2)
{
#if defined (ACE_HAS_PTHREADS)
# if defined (pthread_equal)
  // If it's a macro we can't say "pthread_equal"...
  return pthread_equal (t1, t2);
# else
  return pthread_equal (t1, t2);
# endif /* pthread_equal */
#else /* For STHREADS, WTHREADS, and VXWORKS ... */
  // Hum, Do we need to treat WTHREAD differently?
  // levine 13 oct 98 % Probably, ACE_hthread_t is a HANDLE.
  return t1 == t2;
#endif /* ACE_HAS_PTHREADS */
}

ACE_INLINE int
ACE_OS::thr_continue (ACE_hthread_t target_thread)
{
  ACE_OS_TRACE ("ACE_OS::thr_continue");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#  if defined (ACE_HAS_PTHREAD_CONTINUE)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_continue (target_thread),
                                       result),
                     int);
#  elif defined (ACE_HAS_PTHREAD_CONTINUE_NP)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_continue_np (target_thread),
                                       result),
                     int);
#  elif defined (ACE_HAS_PTHREAD_RESUME_NP)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_resume_np (target_thread),
                                       result),
                     int);
#  else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#  endif /* ACE_HAS_PTHREAD_CONTINUE */
# elif defined (ACE_HAS_WTHREADS)
  DWORD result = ::ResumeThread (target_thread);
  if (result == ACE_SYSCALL_FAILED)
    ACE_FAIL_RETURN (-1);
  else
    return 0;
# elif defined (ACE_HAS_VXTHREADS)
  return ::taskResume (target_thread);
# elif defined (ACE_INTEGRITY)
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_getconcurrency ()
{
  ACE_OS_TRACE ("ACE_OS::thr_getconcurrency");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && defined (ACE_HAS_PTHREAD_GETCONCURRENCY)
  return pthread_getconcurrency ();
# else
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_getprio (ACE_hthread_t ht_id, int &priority, int &policy)
{
  ACE_OS_TRACE ("ACE_OS::thr_getprio");
  ACE_UNUSED_ARG (policy);
#if defined (ACE_HAS_THREADS)
# if (defined (ACE_HAS_PTHREADS) && \
     (!defined (ACE_LACKS_SETSCHED) || defined (ACE_HAS_PTHREAD_SCHEDPARAM)))

  struct sched_param param;
  int result;

  ACE_OSCALL (ACE_ADAPT_RETVAL (pthread_getschedparam (ht_id, &policy, &param),
                                result), int,
              result);
  priority = param.sched_priority;
  return result;
# elif defined (ACE_HAS_WTHREADS)
  ACE_Errno_Guard error (errno);
  priority = ::GetThreadPriority (ht_id);
  DWORD const priority_class = ::GetPriorityClass (::GetCurrentProcess ());
  if (priority_class == 0 && (error = ::GetLastError ()) != NO_ERROR)
    ACE_FAIL_RETURN (-1);

  policy = (priority_class == REALTIME_PRIORITY_CLASS) ? ACE_SCHED_FIFO : ACE_SCHED_OTHER;
  return 0;
# elif defined (ACE_HAS_VXTHREADS)
  return ::taskPriorityGet (ht_id, &priority);
# elif defined (ACE_INTEGRITY)
  Value active_prio;
  const Error err = ::GetActivePriority (ht_id, &active_prio);
  if (err != Success)
    return -1;
  priority = static_cast<int> (active_prio);
  return 0;
# else
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_getprio (ACE_hthread_t ht_id, int &priority)
{
  ACE_OS_TRACE ("ACE_OS::thr_getprio");
  int policy = 0;
  return ACE_OS::thr_getprio (ht_id, priority, policy);
}

#if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
ACE_INLINE int
ACE_OS::thr_getspecific_native (ACE_OS_thread_key_t key, void **data)
{
//  ACE_OS_TRACE ("ACE_OS::thr_getspecific_native");
# if defined (ACE_HAS_PTHREADS)
    *data = pthread_getspecific (key);
    return 0;
# elif defined (ACE_HAS_WTHREADS)
  *data = ::TlsGetValue (key);
  if (*data == 0 && ::GetLastError () != NO_ERROR)
    {
      ACE_OS::set_errno_to_last_error ();
      return -1;
    }
  else
    return 0;
# else /* ACE_HAS_PTHREADS etc.*/
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (data);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS etc.*/
}
#endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */

ACE_INLINE int
ACE_OS::thr_getspecific (ACE_thread_key_t key, void **data)
{
//   ACE_OS_TRACE ("ACE_OS::thr_getspecific");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_TSS_EMULATION)
    if (ACE_TSS_Emulation::is_key (key) == 0)
      {
        errno = EINVAL;
        data = 0;
        return -1;
      }
    else
      {
        *data = ACE_TSS_Emulation::ts_object (key);
        return 0;
      }
# elif defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
  return ACE_OS::thr_getspecific_native (key, data);
#else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (data);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_TSS_EMULATION */
#else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (data);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

#if !(defined (ACE_HAS_VXTHREADS) || (defined (ACE_INTEGRITY) && !defined (ACE_HAS_PTHREADS)))
ACE_INLINE int
ACE_OS::thr_join (ACE_hthread_t thr_handle,
                  ACE_THR_FUNC_RETURN *status)
{
  ACE_OS_TRACE ("ACE_OS::thr_join");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#  if defined (ACE_LACKS_PTHREAD_JOIN)
  ACE_UNUSED_ARG (thr_handle);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
#  else
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_join (thr_handle, status), result),
                     int);
#  endif /* ACE_LACKS_PTHREAD_JOIN */
# elif defined (ACE_HAS_WTHREADS)
  // Waiting on the calling thread will deadlock, so try to avoid that. The
  // direct access to the needed info (GetThreadId) was added at Vista.
  // Win Server 2003 is 5.2; Vista is 6.0
#   if defined (_WIN32_WINNT) && (_WIN32_WINNT >= 0x0502)
  const ACE_TEXT_OSVERSIONINFO &info = ACE_OS::get_win32_versioninfo ();
  if (info.dwMajorVersion >= 6 ||
      (info.dwMajorVersion == 5 && info.dwMinorVersion == 2))
    {
      if (::GetThreadId (thr_handle) == ::GetCurrentThreadId ())
        {
          errno = ERROR_POSSIBLE_DEADLOCK;
          return -1;
        }
    }
#   endif /* _WIN32_WINNT */

  ACE_THR_FUNC_RETURN local_status = 0;

  // Make sure that status is non-NULL.
  if (status == 0)
    status = &local_status;

  if (::WaitForSingleObject (thr_handle, INFINITE) == WAIT_OBJECT_0
      && ::GetExitCodeThread (thr_handle, status) != FALSE)
    {
      ::CloseHandle (thr_handle);
      return 0;
    }
  ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
# else
  ACE_UNUSED_ARG (thr_handle);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (thr_handle);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_join (ACE_thread_t waiter_id,
                  ACE_thread_t *thr_id,
                  ACE_THR_FUNC_RETURN *status)
{
  ACE_OS_TRACE ("ACE_OS::thr_join");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#  if defined (ACE_LACKS_PTHREAD_JOIN)
  ACE_UNUSED_ARG (waiter_id);
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
#  else
  ACE_UNUSED_ARG (thr_id);
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_join (waiter_id, status), result),
                     int);
#  endif /* ACE_LACKS_PTHREAD_JOIN */
# elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (waiter_id);
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (status);

  // This could be implemented if the DLL-Main function or the
  // task exit base class some log the threads which have exited
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (waiter_id);
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}
#endif /* ! ((VXWORKS || ACE_INTEGRITY) && !ACE_HAS_PTHREADS)  */

ACE_INLINE int
ACE_OS::thr_kill (ACE_thread_t thr_id, int signum)
{
  ACE_OS_TRACE ("ACE_OS::thr_kill");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#   if defined (ACE_LACKS_PTHREAD_KILL)
  ACE_UNUSED_ARG (signum);
  ACE_UNUSED_ARG (thr_id);
  ACE_NOTSUP_RETURN (-1);
#   else
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_kill (thr_id, signum),
                                       result),
                     int);
#   endif /* ACE_LACKS_PTHREAD_KILL */
# elif defined (ACE_HAS_VXTHREADS)
  //FUZZ: disable check_for_lack_ACE_OS
  return ::kill (thr_id, signum);
  //FUZZ: enable check_for_lack_ACE_OS
# elif defined (ACE_INTEGRITY)
  Error err;
  ACE_OSCALL (::ExitTask (thr_id, (Value)signum), Error, err);
  return err == Success ? 0 : -1;
# else
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (signum);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (signum);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE size_t
ACE_OS::thr_min_stack ()
{
  ACE_OS_TRACE ("ACE_OS::thr_min_stack");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#   if defined (_SC_THREAD_STACK_MIN)
  return (size_t) ACE_OS::sysconf (_SC_THREAD_STACK_MIN);
#   elif defined (PTHREAD_STACK_MIN)
  return PTHREAD_STACK_MIN;
#   else
  ACE_NOTSUP_RETURN (0);
#   endif /* _SC_THREAD_STACK_MIN */
# elif defined (ACE_HAS_WTHREADS)
  ACE_NOTSUP_RETURN (0);
# elif defined (ACE_HAS_VXTHREADS)
  TASK_DESC taskDesc;
  STATUS status;

  ACE_thread_t tid = ACE_OS::thr_self ();

  ACE_OSCALL (ACE_ADAPT_RETVAL (::taskInfoGet (tid, &taskDesc),
                                status),
              STATUS, status);
  return status == OK ? taskDesc.td_stackSize : 0;
# elif defined (ACE_INTEGRITY)
  // There seems no API to get the minimum stack size for a Task.
  // A related call, GetTaskStackLimits, returns the actual bottom and top of the current Task's stack.
  ACE_NOTSUP_RETURN (0);
# else /* Should not happen... */
  ACE_NOTSUP_RETURN (0);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE ssize_t
ACE_OS::thr_id (char buffer[], size_t buffer_length)
{
#if defined (ACE_WIN32)
  return ACE_OS::snprintf (buffer,
                           buffer_length,
                           "%u",
                           static_cast <unsigned> (ACE_OS::thr_self ()));
#else /* ACE_WIN32 */
  ACE_hthread_t t_id;
  ACE_OS::thr_self (t_id);
#if defined(ACE_HAS_OPAQUE_PTHREAD_T)
  return ACE_OS::snprintf (buffer,
                           buffer_length,
                           "%s",
                           "<unknown>");
#else /* ACE_HAS_OPAQUE_PTHREAD_T */
  return ACE_OS::snprintf (buffer,
                           buffer_length,
                           "%lu",
                           (unsigned long) t_id);
#endif /* ACE_HAS_OPAQUE_PTHREAD_T */
#endif /* WIN32 */
}

ACE_INLINE ssize_t
ACE_OS::thr_gettid (char buffer[], size_t buffer_length)
{
  return ACE_OS::snprintf (buffer, buffer_length, "%d",
    static_cast<int> (ACE_OS::thr_gettid ()));
}

ACE_INLINE pid_t
ACE_OS::thr_gettid ()
{
#ifdef ACE_HAS_GETTID
  return syscall (SYS_gettid);
#else
  ACE_NOTSUP_RETURN (-1);
#endif
}

ACE_INLINE ACE_thread_t
ACE_OS::thr_self ()
{
  // ACE_OS_TRACE ("ACE_OS::thr_self");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  // Note, don't use "::" here since the following call is often a macro.
  return pthread_self ();
# elif defined (ACE_HAS_WTHREADS)
  return ::GetCurrentThreadId ();
# elif defined (ACE_HAS_VXTHREADS)
  return ::taskIdSelf ();
# elif defined (ACE_INTEGRITY)
  return ::CurrentTask ();
# endif /* ACE_HAS_PTHREADS */
#else
  return 1; // Might as well make it the first thread ;-)
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE const char *
ACE_OS::thr_name ()
{
#if defined (ACE_HAS_THREADS) && defined (ACE_HAS_VXTHREADS)
  return ::taskName (ACE_OS::thr_self ());
#else
  ACE_NOTSUP_RETURN (0);
#endif
}

ACE_INLINE void
ACE_OS::thr_self (ACE_hthread_t &self)
{
  ACE_OS_TRACE ("ACE_OS::thr_self");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  // Note, don't use "::" here since the following call is often a macro.
  self = pthread_self ();
# elif defined (ACE_HAS_THREAD_SELF)
  self = ::thread_self ();
# elif defined (ACE_HAS_WTHREADS)
  self = ::GetCurrentThread ();
# elif defined (ACE_HAS_VXTHREADS)
  self = ::taskIdSelf ();
# elif defined (ACE_INTEGRITY)
  self = ::CurrentTask ();
# endif /* ACE_HAS_PTHREADS */
#else
  self = 1; // Might as well make it the main thread ;-)
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_setcancelstate (int new_state, int *old_state)
{
  ACE_OS_TRACE ("ACE_OS::thr_setcancelstate");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
  int result;
  int local_new, local_old;
  switch (new_state)
    {
    case THR_CANCEL_ENABLE:
      local_new = PTHREAD_CANCEL_ENABLE;
      break;
    case THR_CANCEL_DISABLE:
      local_new = PTHREAD_CANCEL_DISABLE;
      break;
    default:
      errno = EINVAL;
      return -1;
    }
  ACE_OSCALL (ACE_ADAPT_RETVAL (pthread_setcancelstate (local_new,
                                                        &local_old),
                                result),
              int, result);
  if (result == -1)
    return -1;
  switch (local_old)
    {
    case PTHREAD_CANCEL_ENABLE:
      *old_state = THR_CANCEL_ENABLE;
      break;
    case PTHREAD_CANCEL_DISABLE:
      *old_state = THR_CANCEL_DISABLE;
      break;
    }
  return result;
# elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (new_state);
  ACE_UNUSED_ARG (old_state);
  ACE_NOTSUP_RETURN (-1);
# else /* Could be ACE_HAS_PTHREADS && ACE_LACKS_PTHREAD_CANCEL */
  ACE_UNUSED_ARG (new_state);
  ACE_UNUSED_ARG (old_state);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (new_state);
  ACE_UNUSED_ARG (old_state);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_setcanceltype (int new_type, int *old_type)
{
  ACE_OS_TRACE ("ACE_OS::thr_setcanceltype");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
  int result;
  int local_new, local_old;
  switch (new_type)
    {
    case THR_CANCEL_DEFERRED:
      local_new = PTHREAD_CANCEL_DEFERRED;
      break;
    case THR_CANCEL_ASYNCHRONOUS:
      local_new = PTHREAD_CANCEL_ASYNCHRONOUS;
      break;
    default:
      errno = EINVAL;
      return -1;
    }
  ACE_OSCALL (ACE_ADAPT_RETVAL (pthread_setcanceltype (local_new,
                                                       &local_old),
                                result),
              int, result);
  if (result == -1)
    return -1;
  switch (local_old)
    {
    case PTHREAD_CANCEL_DEFERRED:
      *old_type = THR_CANCEL_DEFERRED;
      break;
    case PTHREAD_CANCEL_ASYNCHRONOUS:
      *old_type = THR_CANCEL_ASYNCHRONOUS;
      break;
    }
  return result;
# else /* Could be ACE_HAS_PTHREADS && ACE_LACKS_PTHREAD_CANCEL */
  ACE_UNUSED_ARG (new_type);
  ACE_UNUSED_ARG (old_type);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (new_type);
  ACE_UNUSED_ARG (old_type);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_setconcurrency (int hint)
{
  ACE_OS_TRACE ("ACE_OS::thr_setconcurrency");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && defined (ACE_HAS_PTHREAD_SETCONCURRENCY)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_setconcurrency (hint), result), int);
# else
  ACE_UNUSED_ARG (hint);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (hint);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_setprio (ACE_hthread_t ht_id, int priority, int policy)
{
  ACE_OS_TRACE ("ACE_OS::thr_setprio");
  ACE_UNUSED_ARG (policy);
#if defined (ACE_HAS_THREADS)
# if (defined (ACE_HAS_PTHREADS) && \
      (!defined (ACE_LACKS_SETSCHED) || defined (ACE_HAS_PTHREAD_SCHEDPARAM)))

  int result;
  struct sched_param param;
  ACE_OS::memset ((void *) &param, 0, sizeof param);

  // If <policy> is -1, we don't want to use it for
  // pthread_setschedparam().  Instead, obtain policy from
  // pthread_getschedparam().
  if (policy == -1)
    {
      ACE_OSCALL (ACE_ADAPT_RETVAL (pthread_getschedparam (ht_id, &policy, &param), result), int, result);
      if (result == -1)
        return result;
    }

  param.sched_priority = priority;

  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_setschedparam (ht_id,
                                                              policy,
                                                              &param),
                                       result),
                     int);
# elif defined (ACE_HAS_WTHREADS)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::SetThreadPriority (ht_id, priority),
                                          ace_result_),
                        int, -1);
# elif defined (ACE_HAS_VXTHREADS)
  ACE_OSCALL_RETURN (::taskPrioritySet (ht_id, priority), int);
# else
  // For example, platforms that support Pthreads but LACK_SETSCHED.
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_sigsetmask (int how,
                        const sigset_t *nsm,
                        sigset_t *osm)
{
  ACE_OS_TRACE ("ACE_OS::thr_sigsetmask");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_LACKS_PTHREAD_THR_SIGSETMASK)
  // DCE threads
  ACE_UNUSED_ARG (osm);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (how);

  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_HAS_SIGTHREADMASK)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::sigthreadmask (how, nsm, osm), result), int);
# elif defined (ACE_HAS_PTHREADS)
#   if !defined (ACE_LACKS_PTHREAD_SIGMASK)
  int result;
  //FUZZ: disable check_for_lack_ACE_OS
#    if defined (ACE_HAS_NONCONST_PTHREAD_SIGMASK)
  sigset_t *ncnsm = const_cast<sigset_t *>(nsm);
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::pthread_sigmask (how, ncnsm, osm), result), int);
#    else
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (::pthread_sigmask (how, nsm, osm), result), int);
#    endif /* ACE_HAS_NONCONST__PTHREAD_SIGMASK */
  //FUZZ: enable check_for_lack_ACE_OS
#   endif /* !ACE_LACKS_PTHREAD_SIGMASK */

# elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (osm);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (how);

  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_VXWORKS)
  int old_mask = 0;
  switch (how)
    {
    case SIG_BLOCK:
    case SIG_UNBLOCK:
      {
        // get the old mask
        old_mask = ::sigsetmask (*nsm);
        // create a new mask:  the following assumes that sigset_t is 4 bytes,
        // which it is on VxWorks 5.2, so bit operations are done simply . . .
        ::sigsetmask (how == SIG_BLOCK ? (old_mask |= *nsm) : (old_mask &= ~*nsm));
        if (osm)
          *osm = old_mask;
        break;
      }
    case SIG_SETMASK:
      old_mask = ::sigsetmask (*nsm);
      if (osm)
        *osm = old_mask;
      break;
    default:
      return -1;
    }

  return 0;
# else /* Should not happen. */
  ACE_UNUSED_ARG (how);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (osm);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_LACKS_PTHREAD_THR_SIGSETMASK */
#else
  ACE_UNUSED_ARG (how);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (osm);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thr_suspend (ACE_hthread_t target_thread)
{
  ACE_OS_TRACE ("ACE_OS::thr_suspend");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#  if defined (ACE_HAS_PTHREAD_SUSPEND)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_suspend (target_thread), result), int);
#  elif defined (ACE_HAS_PTHREAD_SUSPEND_NP)
  int result;
  ACE_OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_suspend_np (target_thread), result), int);
#  else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#  endif /* ACE_HAS_PTHREAD_SUSPEND */
# elif defined (ACE_HAS_WTHREADS)
  if (::SuspendThread (target_thread) != ACE_SYSCALL_FAILED)
    return 0;
  else
    ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
# elif defined (ACE_HAS_VXTHREADS)
  return ::taskSuspend (target_thread);
# elif defined (ACE_INTEGRITY)
  return ::HaltTask (target_thread) == Success ? 0 : -1;
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE void
ACE_OS::thr_testcancel ()
{
  ACE_OS_TRACE ("ACE_OS::thr_testcancel");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
  pthread_testcancel ();
# else
  // no-op:  can't use ACE_NOTSUP_RETURN because there is no return value
# endif /* ACE_HAS_PTHREADS */
#else
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE void
ACE_OS::thr_yield ()
{
  ACE_OS_TRACE ("ACE_OS::thr_yield");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  ::sched_yield ();
# elif defined (ACE_HAS_WTHREADS)
  ::Sleep (0);
# elif defined (ACE_HAS_VXTHREADS)
  // An argument of 0 to ::taskDelay doesn't appear to yield the
  // current thread.
  // Now, it does seem to work.  The context_switch_time test
  // works fine with task_delay set to 0.
  ::taskDelay (0);
# elif defined (ACE_INTEGRITY)
  ::Yield ();
# endif /* ACE_HAS_PTHREADS */
#else
  ;
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thread_mutex_destroy (ACE_thread_mutex_t *m)
{
  ACE_OS_TRACE ("ACE_OS::thread_mutex_destroy");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ::DeleteCriticalSection (m);
  return 0;
# else
  return ACE_OS::mutex_destroy (m);
# endif /* ACE_HAS_WTHREADS */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);

#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thread_mutex_init (ACE_thread_mutex_t *m,
                           int lock_type,
                           const char *name,
                           ACE_mutexattr_t *arg)
{
  // ACE_OS_TRACE ("ACE_OS::thread_mutex_init");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);

  ACE_SEH_TRY
    {
      ::InitializeCriticalSection (m);
    }
  ACE_SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
      errno = ENOMEM;
      return -1;
    }
  return 0;

# elif defined (ACE_HAS_PTHREADS)
  // Force the use of USYNC_THREAD!
  return ACE_OS::mutex_init (m, USYNC_THREAD, name, arg, 0, lock_type);
# elif defined (ACE_HAS_VXTHREADS)
  return mutex_init (m, lock_type, name, arg);
# elif defined (ACE_INTEGRITY)
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  return ACE_OS::mutex_init (m);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);

#endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::thread_mutex_init (ACE_thread_mutex_t *m,
                           int lock_type,
                           const wchar_t *name,
                           ACE_mutexattr_t *arg)
{
  // ACE_OS_TRACE ("ACE_OS::thread_mutex_init");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);

  ACE_SEH_TRY
    {
      ::InitializeCriticalSection (m);
    }
  ACE_SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
      errno = ENOMEM;
      return -1;
    }
  return 0;

# elif defined (ACE_HAS_PTHREADS)
  // Force the use of USYNC_THREAD!
  return ACE_OS::mutex_init (m, USYNC_THREAD, name, arg, 0, lock_type);
# elif defined (ACE_HAS_VXTHREADS)
  return mutex_init (m, lock_type, name, arg);
# elif defined (ACE_INTEGRITY)
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  return ACE_OS::mutex_init (m);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);

#endif /* ACE_HAS_THREADS */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
ACE_OS::thread_mutex_lock (ACE_thread_mutex_t *m)
{
  // ACE_OS_TRACE ("ACE_OS::thread_mutex_lock");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ::EnterCriticalSection (m);
  return 0;
# else
  return ACE_OS::mutex_lock (m);
# endif /* ACE_HAS_WTHREADS */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thread_mutex_lock (ACE_thread_mutex_t *m,
                           const ACE_Time_Value &timeout)
{
  // ACE_OS_TRACE ("ACE_OS::thread_mutex_lock");

  // For all platforms, except MS Windows, this method is equivalent
  // to calling ACE_OS::mutex_lock() since ACE_thread_mutex_t and
  // ACE_mutex_t are the same type.  However, those typedefs evaluate
  // to different types on MS Windows.  The "thread mutex"
  // implementation in ACE for MS Windows cannot readily support
  // timeouts due to a lack of timeout features for this type of MS
  // Windows synchronization mechanism.

#if defined (ACE_HAS_THREADS) && !defined (ACE_HAS_WTHREADS)
  return ACE_OS::mutex_lock (m, timeout);
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thread_mutex_lock (ACE_thread_mutex_t *m,
                           const ACE_Time_Value *timeout)
{
  return timeout == 0
    ? ACE_OS::thread_mutex_lock (m)
    : ACE_OS::thread_mutex_lock (m, *timeout);
}

ACE_INLINE int
ACE_OS::thread_mutex_trylock (ACE_thread_mutex_t *m)
{
  ACE_OS_TRACE ("ACE_OS::thread_mutex_trylock");

#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
#   if defined (ACE_HAS_WIN32_TRYLOCK)
  BOOL result = ::TryEnterCriticalSection (m);
  if (result)
    {
      return 0;
    }
  else
    {
      errno = EBUSY;
      return -1;
    }
#   else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_WIN32_TRYLOCK */
# elif defined (ACE_HAS_PTHREADS) || defined (ACE_VXWORKS) || defined (ACE_INTEGRITY)
  return ACE_OS::mutex_trylock (m);
#endif /* Threads variety case */

#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
ACE_OS::thread_mutex_unlock (ACE_thread_mutex_t *m)
{
  ACE_OS_TRACE ("ACE_OS::thread_mutex_unlock");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ::LeaveCriticalSection (m);
  return 0;
# else
  return ACE_OS::mutex_unlock (m);
# endif /* ACE_HAS_WTHREADS */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

/*****************************************************************************/

#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)

ACE_INLINE
int
ACE_OS_Thread_Mutex_Guard::acquire ()
{
  return owner_ = ACE_OS::thread_mutex_lock (&lock_);
}

ACE_INLINE
int
ACE_OS_Thread_Mutex_Guard::release ()
{
  if (owner_ == -1)
    return 0;
  else
    {
      owner_ = -1;
      return ACE_OS::thread_mutex_unlock (&lock_);
    }
}

ACE_INLINE
ACE_OS_Thread_Mutex_Guard::ACE_OS_Thread_Mutex_Guard (ACE_thread_mutex_t &m)
  : lock_ (m), owner_ (-1)
{
  if (!ACE_OS_Object_Manager::starting_up ())
    acquire ();
}

ACE_INLINE
ACE_OS_Thread_Mutex_Guard::~ACE_OS_Thread_Mutex_Guard ()
{
  release ();
}

/*****************************************************************************/

ACE_INLINE
int
ACE_OS_Recursive_Thread_Mutex_Guard::acquire ()
{
  return owner_ = ACE_OS::recursive_mutex_lock (&lock_);
}

ACE_INLINE
int
ACE_OS_Recursive_Thread_Mutex_Guard::release ()
{
  if (owner_ == -1)
    return 0;
  else
    {
      owner_ = -1;
      return ACE_OS::recursive_mutex_unlock (&lock_);
    }
}

ACE_INLINE
ACE_OS_Recursive_Thread_Mutex_Guard::ACE_OS_Recursive_Thread_Mutex_Guard (
  ACE_recursive_thread_mutex_t &m)
   : lock_ (m),
     owner_ (-1)
{
  if (!ACE_OS_Object_Manager::starting_up ())
    acquire ();
}

ACE_INLINE
ACE_OS_Recursive_Thread_Mutex_Guard::~ACE_OS_Recursive_Thread_Mutex_Guard ()
{
  release ();
}

#endif /* ACE_MT_SAFE && ACE_MT_SAFE != 0 */


/*****************************************************************************/

ACE_INLINE
ACE_Thread_ID::ACE_Thread_ID (ACE_thread_t thread_id,
                              ACE_hthread_t thread_handle)
  : thread_id_ (thread_id),
    thread_handle_ (thread_handle)
{
}

ACE_INLINE
ACE_Thread_ID::ACE_Thread_ID (const ACE_Thread_ID &id)
  : thread_id_ (id.thread_id_),
    thread_handle_ (id.thread_handle_)
{
}

ACE_INLINE
ACE_Thread_ID&
ACE_Thread_ID::operator= (const ACE_Thread_ID &id)
{
  if (this != &id)
    {
      this->thread_id_ = id.thread_id_;
      this->thread_handle_ = id.thread_handle_;
    }
  return *this;
}

ACE_INLINE
ACE_Thread_ID::ACE_Thread_ID ()
  : thread_id_ (ACE_OS::thr_self ())
{
  ACE_OS::thr_self (thread_handle_);
}

ACE_INLINE
ACE_thread_t
ACE_Thread_ID::id () const
{
  return this->thread_id_;
}

ACE_INLINE void
ACE_Thread_ID::id (ACE_thread_t thread_id)
{
  this->thread_id_ = thread_id;
}

ACE_INLINE ACE_hthread_t
ACE_Thread_ID::handle () const
{
  return this->thread_handle_;
}

ACE_INLINE void
ACE_Thread_ID::handle (ACE_hthread_t thread_handle)
{
  this->thread_handle_ = thread_handle;
}

ACE_INLINE bool
ACE_Thread_ID::operator== (const ACE_Thread_ID &rhs) const
{
  return
    ACE_OS::thr_cmp (this->thread_handle_, rhs.thread_handle_)
    && ACE_OS::thr_equal (this->thread_id_, rhs.thread_id_);
}

ACE_INLINE bool
ACE_Thread_ID::operator!= (const ACE_Thread_ID &rhs) const
{
  return !(*this == rhs);
}

#if !defined (ACE_WIN32)

ACE_INLINE
ACE_event_t::ACE_event_t () :
  name_ (0),
  eventdata_ (0)
{
}

#endif /* !ACE_WIN32 */

ACE_END_VERSIONED_NAMESPACE_DECL
