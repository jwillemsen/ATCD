// $Id$

#include "tao/Transport_Mux_Strategy.h"
#include "tao/Reply_Dispatcher.h"
#include "tao/debug.h"

// @@ Alex: there is another aspect that is controlled by this
//    strategy: the demuxed version must idle() the transport
//    right after the request is sent, otherwise nobody else will be
//    able to use it.
//    The exclusive version must idle it after the reply is received,
//    to guarantee that nobody else is using it.
//    We may need to add a couple of methods to implement that.

TAO_Transport_Mux_Strategy::TAO_Transport_Mux_Strategy (void)
{
}

TAO_Transport_Mux_Strategy::~TAO_Transport_Mux_Strategy (void)
{
}

// *********************************************************************

TAO_Exclusive_TMS::TAO_Exclusive_TMS (void)
  : request_id_generator_ (0),
    request_id_ (0),
    rd_ (0)
{
}

TAO_Exclusive_TMS::~TAO_Exclusive_TMS (void)
{
}

// Generate and return an unique request id for the current
// invocation. We can actually return a predecided ULong, since we
// allow only one invocation over this connection at a time.
CORBA::ULong
TAO_Exclusive_TMS::request_id (void)
{
  return this->request_id_generator_++;
}

// Bind the handler with the request id.
int
TAO_Exclusive_TMS::bind_dispatcher (CORBA::ULong request_id,
                                    TAO_Reply_Dispatcher *rd)
{
  this->request_id_ = request_id;
  this->rd_ = rd;
  return 0;
}

int
TAO_Exclusive_TMS::dispatch_reply (CORBA::ULong request_id,
                                   CORBA::ULong reply_status,
                                   const TAO_GIOP_Version& version,
                                   TAO_GIOP_ServiceContextList& reply_ctx,
                                   TAO_GIOP_Message_State* message_state)
{
  if (this->request_id_ != request_id)
    {
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    "TAO_Exclusive_TMS::dispatch_reply - <%d != %d>\n",
                    this->request_id_, request_id));
      return -1;
    }

  TAO_Reply_Dispatcher *rd = this->rd_;
  this->request_id_ = 0xdeadbeef; // @@ What is a good value???
  this->rd_ = 0;

  return rd->dispatch_reply (reply_status,
                             version,
                             reply_ctx,
                             message_state);
}

TAO_GIOP_Message_State *
TAO_Exclusive_TMS::get_message_state (void)
{
  if (this->rd_ == 0)
    return 0;

  return this->rd_->message_state ();
}

// NOOP function.
void
TAO_Exclusive_TMS::destroy_message_state (TAO_GIOP_Message_State *)
{
}

// *********************************************************************

TAO_Muxed_TMS::TAO_Muxed_TMS (void)
  : request_id_generator_ (0)
{
}

TAO_Muxed_TMS::~TAO_Muxed_TMS (void)
{
  // @@ delete ???
}

// Generate and return an unique request id for the current
// invocation.
CORBA::ULong
TAO_Muxed_TMS::request_id (void)
{
  return this->request_id_generator_++;
}

// Bind the dispatcher with the request id.
int
TAO_Muxed_TMS::bind_dispatcher (CORBA::ULong request_id,
                                TAO_Reply_Dispatcher *rd)
{
  int result = this->dispatcher_table_.bind (request_id, rd);
  
  if (result != 0)
    {
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    "%N:%L:TAO_Muxed_TMS::bind_dispatcher: "
                    "bind dispatcher failed: result = %d\n",
                    result));

      return -1;
    }
  return 0;
}

int
TAO_Muxed_TMS::dispatch_reply (CORBA::ULong request_id,
                               CORBA::ULong reply_status,
                               const TAO_GIOP_Version &version,
                               TAO_GIOP_ServiceContextList &reply_ctx,
                               TAO_GIOP_Message_State *message_state)
{

  int result = 0;
  TAO_Reply_Dispatcher *rd = 0;

  // Find the reply dispatcher for this id.
  result = this->dispatcher_table_.unbind (request_id, rd);
  
  if (result != 0)
    {
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    "%N:%L:TAO_Muxed_TMS::dispatch_reply: "
                    "unbind dispatcher failed: result = %d\n",
                    result));
      
      return -1;
    }
     
  // Dispatch the reply.
  result =  rd->dispatch_reply (reply_status,
                                version,
                                reply_ctx,
                                message_state);

  // Delete the reply dispatcher.
  delete rd;
  rd = 0;
  
  return result;
}

TAO_GIOP_Message_State *
TAO_Muxed_TMS::get_message_state (void)
{
  return 0;
}

void
TAO_Muxed_TMS::destroy_message_state (TAO_GIOP_Message_State *)
{
  // @@ Implement.
  //    delete message_state;
  //    message_state = 0;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Hash_Map_Manager_Ex <CORBA::ULong,
                                        TAO_Reply_Dispatcher *,
                                        ACE_Hash <CORBA::ULong>,
                                        ACE_Equal_To <CORBA::ULong>,
                                        ACE_Null_Mutex>;

template class ACE_Hash_Map_Entry<CORBA::ULong,
                                  TAO_Reply_Dispatcher *>;

template class ACE_Hash_Map_Iterator_Base_Ex<CORBA::ULong,
                                             TAO_Reply_Dispatcher *,
                                             ACE_Hash<unsigned int>, 
                                             ACE_Equal_To<unsigned int>, 
                                             ACE_Null_Mutex>;

template class ACE_Hash_Map_Iterator_Ex<CORBA::ULong,
                                        TAO_Reply_Dispatcher*,
                                        ACE_Hash<CORBA::ULong>,
                                        ACE_Equal_To<CORBA::ULong>,
                                        ACE_Null_Mutex>;

template class ACE_Hash_Map_Reverse_Iterator_Ex<CORBA::ULong,
                                                TAO_Reply_Dispatcher*,
                                                ACE_Hash<CORBA::ULong>,
                                                ACE_Equal_To<CORBA::ULong>,
                                                ACE_Null_Mutex>;
template class ACE_Equal_To <CORBA::ULong>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Hash_Map_Manager_Ex <CORBA::ULong,
                                             TAO_Reply_Dispatcher *,
                                             ACE_Hash <CORBA::ULong>,
                                             ACE_Equal_To <CORBA::ULong>,
                                             ACE_Null_Mutex>

#pragma instantiate ACE_Hash_Map_Entry<CORBA::ULong,
                                       TAO_Reply_Dispatcher *>

#pragma instantiate ACE_Hash_Map_Iterator_Base_Ex<unsigned int,
                                                  TAO_Reply_Dispatcher *,
                                                  ACE_Hash<unsigned int>, 
                                                  ACE_Equal_To<unsigned int>, 
                                                  ACE_Null_Mutex>

#pragma instantiate ACE_Hash_Map_Iterator_Ex<CORBA::ULong,
                                             TAO_Reply_Dispatcher*,
                                             ACE_Hash<CORBA::ULong>,
                                             ACE_Equal_To<CORBA::ULong>,
                                             ACE_Null_Mutex>

#pragma instantiate ACE_Hash_Map_Reverse_Iterator_Ex<CORBA::ULong,
                                                     TAO_Reply_Dispatcher*,
                                                     ACE_Hash<CORBA::ULong>,
                                                     ACE_Equal_To<CORBA::ULong>,
                                                     ACE_Null_Mutex>

#pragma instantiate ACE_Equal_To <CORBA::ULong>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
