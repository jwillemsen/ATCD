/* -*- C++ -*- */
/**
 *  @file  DSRT_CV_Dispatcher_Impl_T.h
 *
 *  @author Venkita Subramonian (venkita@cs.wustl.edu)
 */

#ifndef DSRT_CV_DISPATCHER_IMPL_T_H
#define DSRT_CV_DISPATCHER_IMPL_T_H
#include /**/ "ace/pre.h"
#include "ace/Task.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "Kokyu_dsrt.h"
#include "DSRT_Sched_Queue_T.h"
#include "DSRT_Dispatcher_Impl_T.h"

namespace Kokyu
{
  template<class DSRT_Scheduler_Traits>
  class DSRT_CV_Dispatcher_Impl :
    public DSRT_Dispatcher_Impl<DSRT_Scheduler_Traits>
  {
  public:
    typedef typename
    DSRT_Scheduler_Traits::Guid_t Guid_t;

    typedef typename
    DSRT_Scheduler_Traits::QoSDescriptor_t DSRT_QoSDescriptor;

    DSRT_CV_Dispatcher_Impl (ACE_Sched_Params::Policy sched_policy,
                             int sched_scope);

    DSRT_CV_Dispatcher_Impl (const DSRT_CV_Dispatcher_Impl &) = delete;
    DSRT_CV_Dispatcher_Impl (DSRT_CV_Dispatcher_Impl &&) = delete;
    DSRT_CV_Dispatcher_Impl &operator= (const DSRT_CV_Dispatcher_Impl &) = delete;
    DSRT_CV_Dispatcher_Impl &operator= (DSRT_CV_Dispatcher_Impl &&) = delete;

    int init_i (const DSRT_ConfigInfo&);

    /// Schedule a thread dynamically based on the qos info supplied.
    int schedule_i (Guid_t, const DSRT_QoSDescriptor&);

    /// Update the schedule for a thread. This could alter the current
    /// schedule.
    int update_schedule_i (Guid_t, const DSRT_QoSDescriptor&);

    /// Inform the scheduler that the caller thread is about to
    /// block. This could alter the current schedule.
    int update_schedule_i (Guid_t, Block_Flag_t);

    /// Cancel the schedule for a thread. This could alter the current
    /// schedule.
    int cancel_schedule_i (Guid_t);

    /// Shut down the dispatcher. The dispatcher will stop processing
    /// requests.
    int shutdown_i ();

  private:
    typedef ACE_SYNCH_MUTEX cond_lock_t;
    typedef ACE_SYNCH_CONDITION cond_t;

    cond_lock_t run_cond_lock_;
    cond_t run_cond_;
  };
}

#include "DSRT_CV_Dispatcher_Impl_T.cpp"

#include /**/ "ace/post.h"
#endif /* DSRT_DIRECT_DISPATCHER_IMPL_T_H */
