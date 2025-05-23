// -*- C++ -*-
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_unistd.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_INLINE
u_long
ACE_Log_Msg::priority_mask (MASK_TYPE mask_type)
{
  return mask_type == THREAD
    ? this->priority_mask_
    :  ACE_Log_Msg::process_priority_mask_;
}

ACE_INLINE
int
ACE_Log_Msg::log_priority_enabled (ACE_Log_Priority log_priority)
{
  return ACE_BIT_ENABLED (this->priority_mask_ |
                          ACE_Log_Msg::process_priority_mask_,
                          static_cast<u_long> (log_priority));
}

ACE_INLINE
void
ACE_Log_Msg::op_status (int status)
{
  this->status_ = status;
}

ACE_INLINE
int
ACE_Log_Msg::op_status () const
{
  return this->status_;
}

ACE_INLINE
void
ACE_Log_Msg::restart (bool r)
{
  this->restart_ = r;
}

ACE_INLINE
bool
ACE_Log_Msg::restart () const
{
  return this->restart_;
}

ACE_INLINE
int
ACE_Log_Msg::errnum () const
{
  return this->errnum_;
}

ACE_INLINE
void
ACE_Log_Msg::errnum (int e)
{
  this->errnum_ = e;
}

ACE_INLINE
int
ACE_Log_Msg::linenum () const
{
  return this->linenum_;
}

ACE_INLINE
void
ACE_Log_Msg::linenum (int l)
{
  this->linenum_ = l;
}

ACE_INLINE
int
ACE_Log_Msg::inc ()
{
  return this->trace_depth_++;
}

ACE_INLINE
int
ACE_Log_Msg::dec ()
{
  return this->trace_depth_ == 0 ? 0 : --this->trace_depth_;
}

ACE_INLINE
int
ACE_Log_Msg::trace_depth () const
{
  return this->trace_depth_;
}

ACE_INLINE
void
ACE_Log_Msg::trace_depth (int depth)
{
  this->trace_depth_ = depth;
}

ACE_INLINE
bool
ACE_Log_Msg::trace_active () const
{
  return this->trace_active_;
}

ACE_INLINE
void
ACE_Log_Msg::trace_active (bool value)
{
  this->trace_active_ = value;
}

ACE_INLINE
ACE_Thread_Descriptor *
ACE_Log_Msg::thr_desc () const
{
  return this->thr_desc_;
}

/// Enable the tracing facility on a per-thread basis.
ACE_INLINE
void
ACE_Log_Msg::start_tracing ()
{
  this->tracing_enabled_ = true;
}

/// Disable the tracing facility on a per-thread basis.
ACE_INLINE
void
ACE_Log_Msg::stop_tracing ()
{
  this->tracing_enabled_ = false;
}

ACE_INLINE
bool
ACE_Log_Msg::tracing_enabled () const
{
  return this->tracing_enabled_;
}

ACE_INLINE
const char *
ACE_Log_Msg::file ()
{
  return this->file_;
}

ACE_INLINE
void
ACE_Log_Msg::file (const char *s)
{
  ACE_OS::strsncpy (this->file_, s, sizeof this->file_);
}

ACE_INLINE
const ACE_TCHAR *
ACE_Log_Msg::msg ()
{
  return this->msg_ + ACE_Log_Msg::msg_off_;
}

ACE_INLINE
void
ACE_Log_Msg::msg (const ACE_TCHAR *m)
{
  ACE_OS::strsncpy (this->msg_, m,
                    ((ACE_MAXLOGMSGLEN+1) / sizeof (ACE_TCHAR)));
}

ACE_INLINE
ACE_Log_Msg_Callback *
ACE_Log_Msg::msg_callback () const
{
  return this->msg_callback_;
}

ACE_INLINE
ACE_Log_Msg_Callback *
ACE_Log_Msg::msg_callback (ACE_Log_Msg_Callback *c)
{
  ACE_Log_Msg_Callback *old = this->msg_callback_;
  this->msg_callback_ = c;
  return old;
}

ACE_INLINE
ACE_OSTREAM_TYPE *
ACE_Log_Msg::msg_ostream () const
{
  return this->ostream_;
}

ACE_INLINE
void
ACE_Log_Msg::msg_ostream (ACE_OSTREAM_TYPE *m)
{
  this->ostream_ = m;
}

ACE_INLINE
const ACE_TCHAR *
ACE_Log_Msg::local_host () const
{
  return ACE_Log_Msg::local_host_;
}

ACE_INLINE
pid_t
ACE_Log_Msg::getpid () const
{
  return ACE_OS::getpid ();
}

ACE_END_VERSIONED_NAMESPACE_DECL
