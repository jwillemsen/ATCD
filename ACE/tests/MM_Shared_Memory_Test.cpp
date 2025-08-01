
//=============================================================================
/**
 *  @file    MM_Shared_Memory_Test.cpp
 *
 *   This is a simple test of <ACE_Shared_Memory_MM>.  The test
 *   forks two processes or spawns two threads (depending upon the
 *   platform) and then executes child and parent allowing them to
 *   exchange data using shared memory. No user input is required as
 *   far as command line arguments are concerned.
 *
 *  @author Prashant Jain <pjain@cs.wustl.edu> and Douglas C. Schmidt <d.schmidt@vanderbilt.edu>
 */
//=============================================================================

#include "test_config.h"
#include "ace/Shared_Memory_MM.h"
#include "ace/SV_Semaphore_Simple.h"
#include "ace/Process_Semaphore.h"
#include "ace/Thread_Manager.h"
#include "ace/Lib_Find.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_unistd.h"
#include "ace/ACE.h"

#if !defined (ACE_LACKS_MMAP)

#if defined (ACE_LACKS_FORK)
#include "ace/Thread_Semaphore.h"
typedef ACE_Thread_Semaphore SYNCHRONIZER;
#define SYNC_EXTRA_ARGS
#elif defined (ACE_HAS_POSIX_SEM) && defined(ACE_HAS_SYSV_IPC)
/**
 * @class SYNCHRONIZER
 *
 * @brief If the platform has native cross-process POSIX semaphores, we
 * must *force* this test to use the System V Semaphores in order
 * to get the right semantics.
 */
class SYNCHRONIZER : public ACE_SV_Semaphore_Simple
{
public:
  SYNCHRONIZER (int initial_value)
    : ACE_SV_Semaphore_Simple ((const char *) 0,
                               ACE_SV_Semaphore_Simple::ACE_CREATE,
                               initial_value)
  {}
};
#define SYNC_EXTRA_ARGS
#else

typedef ACE_Process_Semaphore SYNCHRONIZER;

#define SYNC_EXTRA_ARGS , sem_name ()
ACE_TCHAR sem_name_[ACE_UNIQUE_NAME_LEN] = ACE_TEXT ("/");

const ACE_TCHAR *sem_name ()
{
  ACE::unique_name (sem_name_, sem_name_ + 1, ACE_UNIQUE_NAME_LEN - 1);
  return sem_name_;
}
#endif /* !defined (ACE_LACKS_FORK) */

#ifndef ACE_DISABLE_MKTEMP
static const char ACE_ALPHABET[] = "abcdefghijklmnopqrstuvwxyz";
const int SHMSZ = 27;
static ACE_TCHAR *shm_key;

// Synchronize the start of the parent and the child.
static SYNCHRONIZER *synchronizer = 0;

static void *
child (void * = 0)
{
  int result;

  // Wait for the parent to be initialized.
  result = synchronizer->acquire ();
  ACE_TEST_ASSERT (result != -1);

  const char *t = ACE_ALPHABET;
  ACE_Shared_Memory_MM shm_child;

  result = shm_child.open (shm_key);
  ACE_TEST_ASSERT (result != -1);

  char *shm = (char *) shm_child.malloc ();

  ACE_TEST_ASSERT (shm != 0);

  for (char *s = shm; *s != '\0'; s++)
    {
      ACE_TEST_ASSERT (*t == s[0]);
      t++;
    }

  // Indicate to the parent that we're done.
  *shm = '*';

  return 0;
}

static void *
parent (void * = 0)
{
  int result;
  ACE_Shared_Memory_MM shm_parent;

  result = shm_parent.open (shm_key, SHMSZ);
  ACE_TEST_ASSERT (result != -1);

  char *shm = (char *) shm_parent.malloc ();

  ACE_TEST_ASSERT (shm != 0);

  char *s = shm;

  for (const char *c = ACE_ALPHABET; *c != '\0'; c++)
    *s++ = *c;

  *s = '\0';

  // Allow the child to proceed.
  result = synchronizer->release ();
  ACE_TEST_ASSERT (result != -1);

  // Perform a "busy wait" until the child sets the character to '*'.
  while (*shm != '*')
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%P) spinning in parent!\n")));

  result = shm_parent.remove ();
  ACE_TEST_ASSERT (result != -1);

  ACE_OS::unlink (shm_key);
  return 0;
}

static int
spawn ()
{
  // Create the synchronizer before spawning the child process/thread,
  // to avoid race condition between the creation in the parent and
  // use in the child.
  ACE_NEW_RETURN (synchronizer,
                  SYNCHRONIZER (0u /*locked*/ SYNC_EXTRA_ARGS),
                  -1);

#if !defined (ACE_LACKS_FORK)
  switch (ACE_OS::fork (ACE_TEXT ("child")))
    {
    case -1:
      ACE_ERROR_RETURN ((LM_ERROR,
                         ACE_TEXT ("(%P|%t) %p\n"),
                         ACE_TEXT ("fork failed")),
                        1);
      /* NOTREACHED */
    case 0:
      child ();
      // Remove the semaphore.
      synchronizer->remove ();
      delete synchronizer;
      break;
      /* NOTREACHED */
    default:
      parent ();
      delete synchronizer;
      break;
      /* NOTREACHED */
    }
#elif defined (ACE_HAS_THREADS)
  if (ACE_Thread_Manager::instance ()->spawn
      (ACE_THR_FUNC (child),
       (void *) 0,
       THR_NEW_LWP | THR_DETACHED) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("(%P|%t) %p\n"),
                       ACE_TEXT ("thread create failed")),
                      1);
  else if (ACE_Thread_Manager::instance ()->spawn
      (ACE_THR_FUNC (parent),
       (void *) 0,
       THR_NEW_LWP | THR_DETACHED) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("(%P|%t) %p\n"),
                       ACE_TEXT ("thread create failed")),
                      1);
  ACE_Thread_Manager::instance ()->wait ();
  delete synchronizer;
#else
  ACE_UNUSED_ARG (synchronizer);
  ACE_ERROR_RETURN ((LM_ERROR,
                     ACE_TEXT ("only one thread may be run in a process on this platform\n")),
                    1);
#endif /* ACE_HAS_THREADS */
  return 0;
}
#endif /* !ACE_DISABLE_MKTEMP */
#endif /* !ACE_LACKS_MMAP */

int
run_main (int, ACE_TCHAR *[])
{
  ACE_START_TEST (ACE_TEXT ("MM_Shared_Memory_Test"));

#if !defined (ACE_LACKS_MMAP) && !defined (ACE_DISABLE_MKTEMP)
  ACE_TCHAR temp_file[MAXPATHLEN + 1];

  // Get the temporary directory,
  // The - 24 is for the filename, mm_shared_mem_testXXXXXX
  if (ACE::get_temp_dir (temp_file, MAXPATHLEN - 24) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("Temporary path too long\n")), -1);

  // Add the filename to the end
  ACE_OS::strcat (temp_file, ACE_TEXT ("mm_shared_mem_testXXXXXX"));

  // Store in the global variable.
  shm_key = temp_file;

  if (ACE_OS::mktemp (shm_key) == 0
      || (ACE_OS::unlink (shm_key) == -1
          && errno == EPERM))
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("(%P|%t) %p\n"),
                       shm_key),
                      1);
  spawn ();

#else /* !ACE_LACKS_MMAP */
  ACE_ERROR ((LM_INFO,
              ACE_TEXT ("mmap and mktemp")
              ACE_TEXT ("are required for this test\n")));
#endif /* !ACE_LACKS_MMAP */

  ACE_END_TEST;
  return 0;
}
