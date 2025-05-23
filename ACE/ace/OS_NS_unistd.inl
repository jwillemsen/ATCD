// -*- C++ -*-
#include "ace/OS_NS_sys_utsname.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_macros.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/Default_Constants.h"
#include "ace/OS_Memory.h"
#include "ace/Truncate.h"

#if defined (ACE_HAS_CLOCK_GETTIME)
# include "ace/os_include/os_time.h"
#endif /* ACE_HAS_CLOCK_GETTIME */

#if defined (ACE_LACKS_ACCESS)
#  include "ace/OS_NS_stdio.h"
#endif /* ACE_LACKS_ACCESS */

#if defined (ACE_VXWORKS) && (ACE_VXWORKS <= 0x690)
#  if defined (__RTP__)
#    include "ace/os_include/os_strings.h"
#  else
#    include "ace/os_include/os_string.h"
#  endif
#endif

#ifdef ACE_MQX
#  include "ace/MQX_Filesystem.h"
#endif

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_INLINE int
ACE_OS::access (const char *path, int amode)
{
  ACE_OS_TRACE ("ACE_OS::access");
#if defined (ACE_LACKS_ACCESS)
    ACE_UNUSED_ARG (path);
    ACE_UNUSED_ARG (amode);
    ACE_NOTSUP_RETURN (-1);
#elif defined(ACE_WIN32)
  // Windows doesn't support checking X_OK(6)
#  if defined (ACE_ACCESS_EQUIVALENT)
     return ACE_ACCESS_EQUIVALENT (path, amode & 6);
#  else
     return ::access (path, amode & 6);
#  endif
#else
  return ::access (path, amode);
#endif /* ACE_LACKS_ACCESS */
}


#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::access (const wchar_t *path, int amode)
{
#if defined (ACE_WIN32)
  return ::_waccess (path, amode);
#else /* ACE_WIN32 */
  return ACE_OS::access (ACE_Wide_To_Ascii (path).char_rep (), amode);
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE u_int
ACE_OS::alarm (u_int secs)
{
  ACE_OS_TRACE ("ACE_OS::alarm");
#if defined (ACE_LACKS_ALARM)
  ACE_UNUSED_ARG (secs);
  ACE_NOTSUP_RETURN (0);
#else
  return ::alarm (secs);
#endif /* ACE_LACKS_ALARM */
}

ACE_INLINE long
ACE_OS::getpagesize ()
{
  ACE_OS_TRACE ("ACE_OS::getpagesize");
#if defined (ACE_WIN32)
  SYSTEM_INFO sys_info;
  ::GetSystemInfo (&sys_info);
  return (long) sys_info.dwPageSize;
#elif defined (_SC_PAGESIZE)
  return ACE_OS::sysconf (_SC_PAGESIZE);
#elif defined (ACE_HAS_GETPAGESIZE)
  return ::getpagesize ();
#else
  // Use the default set in config.h
  return ACE_PAGE_SIZE;
#endif /* ACE_WIN32 */
}

ACE_INLINE long
ACE_OS::allocation_granularity ()
{
#if defined (ACE_WIN32)
  SYSTEM_INFO sys_info;
  ::GetSystemInfo (&sys_info);
  return sys_info.dwAllocationGranularity;
#else
  return ACE_OS::getpagesize ();
#endif /* ACE_WIN32 */
}

ACE_INLINE int
ACE_OS::chdir (const char *path)
{
  ACE_OS_TRACE ("ACE_OS::chdir");
#if defined (ACE_LACKS_CHDIR)
  ACE_UNUSED_ARG (path);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_CHDIR_EQUIVALENT)
  return ACE_CHDIR_EQUIVALENT (path);
#else
  return ::chdir (path);
#endif /* ACE_CHDIR_EQUIVALENT */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::chdir (const wchar_t *path)
{
#if defined (ACE_WIN32)
  return ::_wchdir (path);
#else /* ACE_WIN32 */
  return ACE_OS::chdir (ACE_Wide_To_Ascii (path).char_rep ());
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
ACE_OS::rmdir (const char *path)
{
#if defined (ACE_LACKS_RMDIR)
  ACE_UNUSED_ARG (path);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_RMDIR_EQUIVALENT)
  return ACE_RMDIR_EQUIVALENT (path);
#else
  return ::rmdir (path);
#endif /* ACE_WIN32 */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::rmdir (const wchar_t *path)
{
#if defined (ACE_WIN32)
  return ::_wrmdir (path);
#else
  ACE_Wide_To_Ascii n_path (path);
  return ACE_OS::rmdir (n_path.char_rep ());
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

// @todo: which 4 and why???  dhinton
// NOTE: The following four function definitions must appear before
// ACE_OS::sema_init ().

ACE_INLINE int
ACE_OS::close (ACE_HANDLE handle)
{
  ACE_OS_TRACE ("ACE_OS::close");
#if defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CloseHandle (handle), ace_result_), int, -1);
#elif defined (ACE_MQX)
  return MQX_Filesystem::inst ().close (handle);
#elif defined (ACE_LACKS_CLOSE)
  ACE_UNUSED_ARG (handle);
  ACE_NOTSUP_RETURN (-1);
#else
  return ::close (handle);
#endif /* ACE_WIN32 */
}

ACE_INLINE ACE_HANDLE
ACE_OS::dup (ACE_HANDLE handle)
{
  ACE_OS_TRACE ("ACE_OS::dup");
#if defined (ACE_LACKS_DUP)
  ACE_UNUSED_ARG (handle);
  ACE_NOTSUP_RETURN (ACE_INVALID_HANDLE);
#elif defined (ACE_WIN32)
  ACE_HANDLE new_fd;
  if (::DuplicateHandle(::GetCurrentProcess (),
                        handle,
                        ::GetCurrentProcess(),
                        &new_fd,
                        0,
                        TRUE,
                        DUPLICATE_SAME_ACCESS))
    return new_fd;
  else
    ACE_FAIL_RETURN (ACE_INVALID_HANDLE);
  /* NOTREACHED */
#else
  return ::dup (handle);
#endif /* ACE_LACKS_DUP */
}

ACE_INLINE ACE_HANDLE
ACE_OS::dup(ACE_HANDLE handle, pid_t pid)
{
  ACE_OS_TRACE("ACE_OS::dup");
#if defined (ACE_LACKS_DUP)
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (pid);
  ACE_NOTSUP_RETURN (ACE_INVALID_HANDLE);
#elif defined (ACE_WIN32)
  ACE_HANDLE new_fd;
  ACE_HANDLE hTargetProcess = ::OpenProcess (PROCESS_DUP_HANDLE,
                                             FALSE,
                                             pid);
  if(::DuplicateHandle(::GetCurrentProcess (),
                       handle,
                       hTargetProcess,
                       &new_fd,
                       0,
                       TRUE,
                       DUPLICATE_SAME_ACCESS))
    {
      ::CloseHandle (hTargetProcess);
      return new_fd;
    }
  else
    ACE_FAIL_RETURN (ACE_INVALID_HANDLE);
  /*NOTREACHED*/
#else
  ACE_UNUSED_ARG (pid);
  return ::dup(handle);
#endif /* ACE_LACKS_DUP */
}

ACE_INLINE int
ACE_OS::dup2 (ACE_HANDLE oldhandle, ACE_HANDLE newhandle)
{
  ACE_OS_TRACE ("ACE_OS::dup2");
#if defined (ACE_LACKS_DUP2)
  // msvcrt has _dup2 ?!
  ACE_UNUSED_ARG (oldhandle);
  ACE_UNUSED_ARG (newhandle);
  ACE_NOTSUP_RETURN (-1);
#else
  return ::dup2 (oldhandle, newhandle);
#endif /* ACE_LACKS_DUP2 */
}

ACE_INLINE int
ACE_OS::execv (const char *path,
               char *const argv[])
{
  ACE_OS_TRACE ("ACE_OS::execv");
#if defined (ACE_LACKS_EXEC)
  ACE_UNUSED_ARG (path);
  ACE_UNUSED_ARG (argv);

  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
# if defined (__BORLANDC__)
  return ::execv (path, argv);
# elif defined (__MINGW32__)
  return ::_execv (path, (char *const *) argv);
# else
  // Why this odd-looking code? If execv() returns at all, it's an error.
  // Windows defines this as returning an intptr_t rather than a simple int,
  // and the conversion triggers compile warnings. So just return -1 if
  // the call returns.
  ::_execv (path, (const char *const *) argv);
  return -1;
# endif /* __BORLANDC__ */
#else
  return ::execv (path, argv);
#endif /* ACE_LACKS_EXEC */
}

ACE_INLINE int
ACE_OS::execve (const char *path,
                char *const argv[],
                char *const envp[])
{
  ACE_OS_TRACE ("ACE_OS::execve");
#if defined (ACE_LACKS_EXEC)
  ACE_UNUSED_ARG (path);
  ACE_UNUSED_ARG (argv);
  ACE_UNUSED_ARG (envp);

  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
# if defined (__BORLANDC__)
  return ::execve (path, argv, envp);
# elif defined (__MINGW32__)
  return ::_execve (path, (char *const *) argv, (char *const *) envp);
# else
  // Why this odd-looking code? If execv() returns at all, it's an error.
  // Windows defines this as returning an intptr_t rather than a simple int,
  // and the conversion triggers compile warnings. So just return -1 if
  // the call returns.
  ::_execve (path, (const char *const *) argv, (const char *const *) envp);
  return -1;
# endif /* __BORLANDC__ */
#else
  return ::execve (path, argv, envp);
#endif /* ACE_LACKS_EXEC */
}

ACE_INLINE int
ACE_OS::execvp (const char *file,
                char *const argv[])
{
  ACE_OS_TRACE ("ACE_OS::execvp");
#if defined (ACE_LACKS_EXEC) || defined (ACE_LACKS_EXECVP)
  ACE_UNUSED_ARG (file);
  ACE_UNUSED_ARG (argv);

  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
# if defined (__BORLANDC__)
  return ::execvp (file, argv);
# elif defined (__MINGW32__)
  return ::_execvp (file, (char *const *) argv);
# else
  // Why this odd-looking code? If execv() returns at all, it's an error.
  // Windows defines this as returning an intptr_t rather than a simple int,
  // and the conversion triggers compile warnings. So just return -1 if
  // the call returns.
  ::_execvp (file, (const char *const *) argv);
  return -1;
# endif /* __BORLANDC__ */
#else
  return ::execvp (file, argv);
#endif /* ACE_LACKS_EXEC */
}

ACE_INLINE pid_t
ACE_OS::fork ()
{
  ACE_OS_TRACE ("ACE_OS::fork");
#if defined (ACE_LACKS_FORK)
  ACE_NOTSUP_RETURN (pid_t (-1));
#else
  return ::fork ();
#endif /* ACE_LACKS_FORK */
}

ACE_INLINE int
ACE_OS::fsync (ACE_HANDLE handle)
{
  ACE_OS_TRACE ("ACE_OS::fsync");
# if defined (ACE_LACKS_FSYNC)
  ACE_UNUSED_ARG (handle);
  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::FlushFileBuffers (handle), ace_result_), int, -1);
# else
  return ::fsync (handle);
# endif /* ACE_LACKS_FSYNC */
}

ACE_INLINE int
ACE_OS::ftruncate (ACE_HANDLE handle, ACE_OFF_T offset)
{
  ACE_OS_TRACE ("ACE_OS::ftruncate");
#if defined (ACE_WIN32)
  LARGE_INTEGER loff;
  loff.QuadPart = offset;
  if (::SetFilePointerEx (handle, loff, 0, FILE_BEGIN))
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::SetEndOfFile (handle), ace_result_), int, -1);
  else
    ACE_FAIL_RETURN (-1);
#elif defined (ACE_LACKS_FTRUNCATE)
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (offset);
  ACE_NOTSUP_RETURN (-1);
#else
  return ::ftruncate (handle, offset);
#endif /* ACE_WIN32 */
}

ACE_INLINE char *
ACE_OS::getcwd (char *buf, size_t size)
{
  ACE_OS_TRACE ("ACE_OS::getcwd");
#if defined (ACE_LACKS_GETCWD)
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (size);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_GETCWD_EQUIVALENT)
  return ACE_GETCWD_EQUIVALENT (buf, static_cast<int> (size));
#elif defined (ACE_WIN32)
  return ::getcwd (buf, static_cast<int> (size));
#else
  return ::getcwd (buf, size);
#endif /* ACE_LACKS_GETCWD */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
ACE_OS::getcwd (wchar_t *buf, size_t size)
{
#  if defined (ACE_WIN32)
  return ::_wgetcwd (buf, static_cast<int> (size));
#  else
  char *narrow_buf = new char[size];
  char *result = 0;
  result = ACE_OS::getcwd (narrow_buf, size);
  ACE_Ascii_To_Wide wide_buf (result);
  delete [] narrow_buf;
  if (result != 0)
    ACE_OS::strsncpy (buf, wide_buf.wchar_rep (), size);
  return result == 0 ? 0 : buf;
#  endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE gid_t
ACE_OS::getgid ()
{
  ACE_OS_TRACE ("ACE_OS::getgid");
#if defined (ACE_LACKS_GETGID)
  ACE_NOTSUP_RETURN (static_cast<gid_t> (-1));
# else
  return ::getgid ();
# endif /* ACE_LACKS_GETGID */
}

ACE_INLINE gid_t
ACE_OS::getegid ()
{
  ACE_OS_TRACE ("ACE_OS::getegid");
#if defined (ACE_LACKS_GETEGID)
  ACE_NOTSUP_RETURN (static_cast<gid_t> (-1));
# else
  return ::getegid ();
# endif /* ACE_LACKS_GETEGID */
}

ACE_INLINE int
ACE_OS::getopt (int argc, char *const *argv, const char *optstring)
{
  ACE_OS_TRACE ("ACE_OS::getopt");
#if defined (ACE_LACKS_GETOPT)
  ACE_UNUSED_ARG (argc);
  ACE_UNUSED_ARG (argv);
  ACE_UNUSED_ARG (optstring);
  ACE_NOTSUP_RETURN (-1);
# else
  return ::getopt (argc, argv, optstring);
# endif /* ACE_LACKS_GETOPT */
}

ACE_INLINE pid_t
ACE_OS::getpgid (pid_t pid)
{
  ACE_OS_TRACE ("ACE_OS::getpgid");
#if defined (ACE_LACKS_GETPGID)
  ACE_UNUSED_ARG (pid);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_LINUX) && __GLIBC__ > 1 && __GLIBC_MINOR__ >= 0
  // getpgid() is from SVR4, which appears to be the reason why GLIBC
  // doesn't enable its prototype by default.
  // Rather than create our own extern prototype, just use the one
  // that is visible (ugh).
  return ::__getpgid (pid);
#else
  return ::getpgid (pid);
#endif /* ACE_LACKS_GETPGID */
}

ACE_INLINE pid_t
ACE_OS::getpid ()
{
  // ACE_OS_TRACE ("ACE_OS::getpid");
#if defined (ACE_LACKS_GETPID)
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
  return ::GetCurrentProcessId ();
#else
  return ::getpid ();
#endif /* ACE_LACKS_GETPID */
}

ACE_INLINE pid_t
ACE_OS::getppid ()
{
  ACE_OS_TRACE ("ACE_OS::getppid");
#if defined (ACE_LACKS_GETPPID)
  ACE_NOTSUP_RETURN (-1);
#else
  return ::getppid ();
#endif /* ACE_LACKS_GETPPID */
}

ACE_INLINE uid_t
ACE_OS::getuid ()
{
  ACE_OS_TRACE ("ACE_OS::getuid");
#if defined (ACE_LACKS_GETUID)
  ACE_NOTSUP_RETURN (static_cast<uid_t> (-1));
# else
  return ::getuid ();
# endif /* ACE_LACKS_GETUID*/
}

ACE_INLINE uid_t
ACE_OS::geteuid ()
{
  ACE_OS_TRACE ("ACE_OS::geteuid");
#if defined (ACE_LACKS_GETEUID)
  ACE_NOTSUP_RETURN (static_cast<uid_t> (-1));
# else
  return ::geteuid ();
# endif /* ACE_LACKS_GETEUID */
}

ACE_INLINE int
ACE_OS::hostname (char name[], size_t maxnamelen)
{
  ACE_OS_TRACE ("ACE_OS::hostname");
#if defined (ACE_VXWORKS)
  return ::gethostname (name, maxnamelen);
#elif defined (ACE_WIN32)
  if (::gethostname (name, ACE_Utils::truncate_cast<int> (maxnamelen)) == 0)
  {
    return 0;
  }
  else
  {
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::GetComputerNameA (name,
                                            LPDWORD (&maxnamelen)),
                                            ace_result_), int, -1);
  }
#elif defined (ACE_MQX)
  const int enet_device = 0;
  IPCFG_IP_ADDRESS_DATA ip_data;
  if (ipcfg_get_ip (enet_device, &ip_data))
  {
    ACE_OS::snprintf(name, maxnamelen, "%d.%d.%d.%d", IPBYTES(ip_data.ip));
    return 0;
  }
  return -1;
#elif defined (ACE_LACKS_GETHOSTNAME)
  ACE_NOTSUP_RETURN (-1);
#else /* ACE_VXWORKS */
  ACE_utsname host_info;

  if (ACE_OS::uname (&host_info) == -1)
    return -1;
  else
    {
      ACE_OS::strsncpy (name, host_info.nodename, maxnamelen);
      return 0;
    }
#endif /* ACE_VXWORKS */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::hostname (wchar_t name[], size_t maxnamelen)
{
#if defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (GetComputerNameW (name,
                                                        LPDWORD (&maxnamelen)),
                                          ace_result_), int, -1);
#else /* ACE_WIN32 */
  // Emulate using the char version
  char *char_name = 0;

  ACE_NEW_RETURN (char_name, char[maxnamelen], -1);

  int result = ACE_OS::hostname(char_name, maxnamelen);
  ACE_OS::strcpy (name, ACE_Ascii_To_Wide (char_name).wchar_rep ());

  delete [] char_name;
  return result;
#endif /* ACE_WIN32  */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
ACE_OS::isatty (int handle)
{
  ACE_OS_TRACE ("ACE_OS::isatty");
#if defined (ACE_LACKS_ISATTY)
  ACE_UNUSED_ARG (handle);
  return 0;
# elif defined (ACE_WIN32)
  return ::_isatty (handle);
# else
  return ::isatty (handle);
# endif /* ACE_LACKS_ISATTY */
}

#if defined (ACE_WIN32)
ACE_INLINE int
ACE_OS::isatty (ACE_HANDLE handle)
{
#if defined (ACE_LACKS_ISATTY)
  ACE_UNUSED_ARG (handle);
  return 0;
#else
  int const fd = ::_open_osfhandle (intptr_t (handle), 0);
  int status = 0;
  if (fd != -1)
    {
      status = ::_isatty (fd);
      ::_close (fd);
    }
  return status;
#endif /* ACE_LACKS_ISATTY */
}

#endif /* ACE_WIN32 */

ACE_INLINE ACE_OFF_T
ACE_OS::lseek (ACE_HANDLE handle, ACE_OFF_T offset, int whence)
{
  ACE_OS_TRACE ("ACE_OS::lseek");
#if defined (ACE_WIN32)
# if SEEK_SET != FILE_BEGIN || SEEK_CUR != FILE_CURRENT || SEEK_END != FILE_END
  //#error Windows NT is evil AND rude!
  switch (whence)
    {
    case SEEK_SET:
      whence = FILE_BEGIN;
      break;
    case SEEK_CUR:
      whence = FILE_CURRENT;
      break;
    case SEEK_END:
      whence = FILE_END;
      break;
    default:
      errno = EINVAL;
      return static_cast<ACE_OFF_T> (-1); // rather safe than sorry
    }
# endif  /* SEEK_SET != FILE_BEGIN || SEEK_CUR != FILE_CURRENT || SEEK_END != FILE_END */
  LONG low_offset = ACE_LOW_PART(offset);
  LONG high_offset = ACE_HIGH_PART(offset);
  DWORD const result =
    ::SetFilePointer (handle, low_offset, &high_offset, whence);
  if (result == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    ACE_FAIL_RETURN (static_cast<ACE_OFF_T> (-1));
  else
    return result;
#elif defined (ACE_MQX)
  switch (whence)
    {
    case SEEK_SET:
      whence = IO_SEEK_SET;
      break;
    case SEEK_CUR:
      whence = IO_SEEK_CUR;
      break;
    case SEEK_END:
      whence = IO_SEEK_END;
      break;
    default:
      errno = EINVAL;
      return static_cast<ACE_OFF_T> (-1);
    }
  return static_cast<ACE_OFF_T> (MQX_Filesystem::inst ().lseek (handle, offset, whence));
#elif defined (ACE_LACKS_LSEEK)
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (offset);
  ACE_UNUSED_ARG (whence);
  ACE_NOTSUP_RETURN (static_cast<off_t> (-1));
#else
  return ::lseek (handle, offset, whence);
#endif /* ACE_WIN32 */
}

#if defined (ACE_HAS_LLSEEK) || defined (ACE_HAS_LSEEK64)
ACE_INLINE ACE_LOFF_T
ACE_OS::llseek (ACE_HANDLE handle, ACE_LOFF_T offset, int whence)
{
  ACE_OS_TRACE ("ACE_OS::llseek");

#if ACE_SIZEOF_LONG == 8
  /* The native lseek is 64 bit.  Use it. */
  return ACE_OS::lseek (handle, offset, whence);
#elif defined (ACE_HAS_LLSEEK) && defined (ACE_HAS_LSEEK64)
# error Either ACE_HAS_LSEEK64 and ACE_HAS_LLSEEK should be defined, not both!
#elif defined (ACE_HAS_LSEEK64)
  return ::lseek64 (handle, offset, whence);
#elif defined (ACE_HAS_LLSEEK)
# if defined (ACE_WIN32)
  LARGE_INTEGER distance, new_file_pointer;

  distance.QuadPart = offset;

  return
    (::SetFilePointerEx (handle, distance, &new_file_pointer, whence)
     ? new_file_pointer.QuadPart
     : static_cast<ACE_LOFF_T> (-1));
# else
    return ::llseek (handle, offset, whence);
# endif /* WIN32 */
#endif
}
#endif /* ACE_HAS_LLSEEK || ACE_HAS_LSEEK64 */

ACE_INLINE ssize_t
ACE_OS::read (ACE_HANDLE handle, void *buf, size_t len)
{
  ACE_OS_TRACE ("ACE_OS::read");
#if defined (ACE_WIN32)
  DWORD ok_len;
  if (::ReadFile (handle, buf, static_cast<DWORD> (len), &ok_len, 0))
    return (ssize_t) ok_len;
  else
    ACE_FAIL_RETURN (-1);

#elif defined (ACE_LACKS_READ)
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (len);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_MQX)
  return MQX_Filesystem::inst ().read (handle, reinterpret_cast<unsigned char *> (buf), len);

#else

  ssize_t result;

# if defined (ACE_HAS_CHARPTR_SOCKOPT)
  ACE_OSCALL (::read (handle, static_cast <char *> (buf), len), ssize_t, result);
# else
  ACE_OSCALL (::read (handle, buf, len), ssize_t, result);
# endif /* ACE_HAS_CHARPTR_SOCKOPT */

# if !(defined (EAGAIN) && defined (EWOULDBLOCK) && EAGAIN == EWOULDBLOCK)
  // Optimize this code out if we can detect that EAGAIN ==
  // EWOULDBLOCK at compile time.  If we cannot detect equality at
  // compile-time (e.g. if EAGAIN or EWOULDBLOCK are not preprocessor
  // macros) perform the check at run-time.  The goal is to avoid two
  // TSS accesses in the _REENTRANT case when EAGAIN == EWOULDBLOCK.
  if (result == -1
#  if !defined (EAGAIN) || !defined (EWOULDBLOCK)
      && EAGAIN != EWOULDBLOCK
#  endif  /* !EAGAIN || !EWOULDBLOCK */
      && errno == EAGAIN)
    {
      errno = EWOULDBLOCK;
    }
# endif /* EAGAIN != EWOULDBLOCK*/

  return result;
#endif /* ACE_WIN32 */
}

ACE_INLINE ssize_t
ACE_OS::read (ACE_HANDLE handle, void *buf, size_t len,
              ACE_OVERLAPPED *overlapped)
{
  ACE_OS_TRACE ("ACE_OS::read");
#if defined (ACE_WIN32)
  DWORD ok_len;
  DWORD short_len = static_cast<DWORD> (len);
  if (::ReadFile (handle, buf, short_len, &ok_len, overlapped))
    return (ssize_t) ok_len;
  else
    ACE_FAIL_RETURN (-1);
#else
  ACE_UNUSED_ARG (overlapped);
  return ACE_OS::read (handle, buf, len);
#endif /* ACE_WIN32 */
}

ACE_INLINE ssize_t
ACE_OS::readlink (const char *path, char *buf, size_t bufsiz)
{
  ACE_OS_TRACE ("ACE_OS::readlink");
# if defined (ACE_LACKS_READLINK)
  ACE_UNUSED_ARG (path);
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (bufsiz);
  ACE_NOTSUP_RETURN (-1);
# elif defined(ACE_HAS_NONCONST_READLINK)
  return ::readlink (const_cast <char *>(path), buf, bufsiz);
# else
  return ::readlink (path, buf, bufsiz);
# endif /* ACE_LACKS_READLINK */
}

ACE_INLINE int
ACE_OS::pipe (ACE_HANDLE fds[])
{
  ACE_OS_TRACE ("ACE_OS::pipe");
# if defined (ACE_LACKS_PIPE)
  ACE_UNUSED_ARG (fds);
  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL
                        (::CreatePipe (&fds[0], &fds[1], 0, 0),
                         ace_result_), int, -1);
# else
  return ::pipe (fds);
# endif /* ACE_LACKS_PIPE */
}

ACE_INLINE void *
ACE_OS::sbrk (intptr_t brk)
{
#if defined (ACE_LACKS_SBRK)
  ACE_UNUSED_ARG (brk);
  ACE_NOTSUP_RETURN (0);
#else
  return ::sbrk (brk);
#endif /* ACE_LACKS_SBRK */
}

ACE_INLINE int
ACE_OS::setgid (gid_t gid)
{
  ACE_OS_TRACE ("ACE_OS::setgid");
#if defined (ACE_LACKS_SETGID)
  ACE_UNUSED_ARG (gid);
  ACE_NOTSUP_RETURN (-1);
# else
  return ::setgid (gid);
# endif /* ACE_LACKS_SETGID */
}

ACE_INLINE int
ACE_OS::setegid (gid_t gid)
{
  ACE_OS_TRACE ("ACE_OS::setegid");
#if defined (ACE_LACKS_SETEGID)
  ACE_UNUSED_ARG (gid);
  ACE_NOTSUP_RETURN (-1);
# else
  return ::setegid (gid);
# endif /* ACE_LACKS_SETEGID */
}

ACE_INLINE int
ACE_OS::setpgid (pid_t pid, pid_t pgid)
{
  ACE_OS_TRACE ("ACE_OS::setpgid");
#if defined (ACE_LACKS_SETPGID)
  ACE_UNUSED_ARG (pid);
  ACE_UNUSED_ARG (pgid);
  ACE_NOTSUP_RETURN (-1);
#else
  return ::setpgid (pid, pgid);
#endif /* ACE_LACKS_SETPGID */
}

ACE_INLINE int
ACE_OS::setregid (gid_t rgid, gid_t egid)
{
  ACE_OS_TRACE ("ACE_OS::setregid");
#if defined (ACE_LACKS_SETREGID)
  ACE_UNUSED_ARG (rgid);
  ACE_UNUSED_ARG (egid);
  ACE_NOTSUP_RETURN (-1);
#else
  return ::setregid (rgid, egid);
#endif /* ACE_LACKS_SETREGID */
}

ACE_INLINE int
ACE_OS::setreuid (uid_t ruid, uid_t euid)
{
  ACE_OS_TRACE ("ACE_OS::setreuid");
#if defined (ACE_LACKS_SETREUID)
  ACE_UNUSED_ARG (ruid);
  ACE_UNUSED_ARG (euid);
  ACE_NOTSUP_RETURN (-1);
#else
  return ::setreuid (ruid, euid);
#endif /* ACE_LACKS_SETREUID */
}

ACE_INLINE pid_t
ACE_OS::setsid ()
{
  ACE_OS_TRACE ("ACE_OS::setsid");
#if defined (ACE_LACKS_SETSID)
  ACE_NOTSUP_RETURN (-1);
#else
  return ::setsid ();
# endif /* ACE_LACKS_SETSID */
}

ACE_INLINE int
ACE_OS::setuid (uid_t uid)
{
  ACE_OS_TRACE ("ACE_OS::setuid");
#if defined (ACE_LACKS_SETUID)
  ACE_UNUSED_ARG (uid);
  ACE_NOTSUP_RETURN (-1);
# else
  return ::setuid (uid);
# endif /* ACE_LACKS_SETUID */
}

ACE_INLINE int
ACE_OS::seteuid (uid_t uid)
{
  ACE_OS_TRACE ("ACE_OS::seteuid");
#if defined (ACE_LACKS_SETEUID)
  ACE_UNUSED_ARG (uid);
  ACE_NOTSUP_RETURN (-1);
# else
  return ::seteuid (uid);
# endif /* ACE_LACKS_SETEUID */
}

ACE_INLINE int
ACE_OS::sleep (u_int seconds)
{
  ACE_OS_TRACE ("ACE_OS::sleep");
#if defined (ACE_HAS_CLOCK_GETTIME)
  struct timespec rqtp;
  // Initializer doesn't work with Green Hills 1.8.7
  rqtp.tv_sec = seconds;
  rqtp.tv_nsec = 0L;
  //FUZZ: disable check_for_lack_ACE_OS
  return ::nanosleep (&rqtp, 0);
  //FUZZ: enable check_for_lack_ACE_OS
#elif defined (ACE_LACKS_SLEEP)
  ACE_UNUSED_ARG (seconds);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
  ::Sleep (seconds * ACE_ONE_SECOND_IN_MSECS);
  return 0;
#elif defined (ACE_MQX)
  _time_delay (seconds * ACE_ONE_SECOND_IN_MSECS);
  return 0;
#else
  return ::sleep (seconds);
#endif /* ACE_WIN32 */
}

ACE_INLINE int
ACE_OS::sleep (const ACE_Time_Value &tv)
{
  ACE_OS_TRACE ("ACE_OS::sleep");
#if defined (ACE_WIN32)
  ::Sleep (tv.msec ());
  return 0;
#elif defined (ACE_MQX)
  _time_delay (tv.msec ());
  return 0;
#elif defined (ACE_HAS_CLOCK_GETTIME)
  timespec_t rqtp = tv;
  //FUZZ: disable check_for_lack_ACE_OS
  return ::nanosleep (&rqtp, 0);
  //FUZZ: enable check_for_lack_ACE_OS
#else
# if !defined (ACE_LACKS_SELECT)
#  if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
  // Copy the timeval, because this platform doesn't declare the timeval
  // as a pointer to const.
  timeval tv_copy = tv;
  // FUZZ: disable check_for_lack_ACE_OS
  return ::select (0, 0, 0, 0, &tv_copy);
  // FUZZ: enable check_for_lack_ACE_OS
#  else
  const timeval* tvp = tv;
  // FUZZ: disable check_for_lack_ACE_OS
  return ::select (0, 0, 0, 0, tvp);
  // FUZZ: enable check_for_lack_ACE_OS
#  endif
# else
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (-1);
# endif
#endif /* ACE_WIN32 */
}

ACE_INLINE void
ACE_OS::swab (const void *src,
              void *dest,
              ssize_t length)
{
#if defined (ACE_LACKS_SWAB)
  // ------------------------------------------------------------
  // The following copyright notice applies to the swab()
  // implementation within this "ACE_LACKS_SWAB" block of code.
  // ------------------------------------------------------------
  /*
    Copyright (c) 1994-2006  Red Hat, Inc. All rights reserved.

    This copyrighted material is made available to anyone wishing to
    use, modify, copy, or redistribute it subject to the terms and
    conditions of the BSD License.   This program is distributed in
    the hope that it will be useful, but WITHOUT ANY WARRANTY
    expressed or implied, including the implied warranties of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  A copy of
    this license is available at
    http://www.opensource.org/licenses. Any Red Hat trademarks that
    are incorporated in the source code or documentation are not
    subject to the BSD License and may only be used or replicated with
    the express permission of Red Hat, Inc.
  */

  const char *from = static_cast<const char*> (src);
  char *to = static_cast<char *> (dest);
  ssize_t ptr = 0;
  for (ptr = 1; ptr < length; ptr += 2)
    {
      char p = from[ptr];
      char q = from[ptr-1];
      to[ptr-1] = p;
      to[ptr  ] = q;
    }
  if (ptr == length) /* I.e., if length is odd, */
    to[ptr-1] = 0;   /* then pad with a NUL. */
#elif defined (ACE_HAS_NONCONST_SWAB)
  const char *tmp = static_cast<const char*> (src);
  char *from = const_cast<char *> (tmp);
  char *to = static_cast<char *> (dest);
#  if defined (ACE_HAS_INT_SWAB)
  int ilength = ACE_Utils::truncate_cast<int> (length);
#    if defined (ACE_SWAB_EQUIVALENT)
  ACE_SWAB_EQUIVALENT (from, to, ilength);
#    else
  ::swab (from, to, ilength);
#    endif
#  else
  ::swab (from, to, length);
#  endif /* ACE_HAS_INT_SWAB */
#else
  ::swab (src, dest, length);
#endif /* ACE_LACKS_SWAB */

}

ACE_INLINE long
ACE_OS::sysconf (int name)
{
  ACE_OS_TRACE ("ACE_OS::sysconf");
#if defined (ACE_INTEGRITY)
  if (name == _SC_PAGESIZE) return ACE_PAGE_SIZE;
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_LACKS_SYSCONF)
  ACE_UNUSED_ARG (name);
  ACE_NOTSUP_RETURN (-1);
#else
  return ::sysconf (name);
#endif /* ACE_LACKS_SYSCONF */
}

ACE_INLINE long
ACE_OS::sysinfo (int /*cmd*/, char * /*buf*/, long /*count*/)
{
  ACE_OS_TRACE ("ACE_OS::sysinfo");
  ACE_NOTSUP_RETURN (0);
}

ACE_INLINE int
ACE_OS::truncate (const ACE_TCHAR *filename,
                  ACE_OFF_T offset)
{
  ACE_OS_TRACE ("ACE_OS::truncate");
#if defined (ACE_WIN32)
  ACE_HANDLE handle = ACE_OS::open (filename, O_WRONLY, ACE_DEFAULT_FILE_PERMS);

  LARGE_INTEGER loffset;
  loffset.QuadPart = offset;

  if (handle == ACE_INVALID_HANDLE)
    {
      ACE_FAIL_RETURN (-1);
    }
  else if (::SetFilePointerEx (handle, loffset, 0, FILE_BEGIN))
    {
      BOOL result = ::SetEndOfFile (handle);
      ::CloseHandle (handle);
      ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (result, ace_result_), int, -1);
    }
  else
    {
      ::CloseHandle (handle);
      ACE_FAIL_RETURN (-1);
    }
  /* NOTREACHED */
#elif !defined (ACE_LACKS_TRUNCATE)
  return ::truncate (ACE_TEXT_ALWAYS_CHAR (filename), offset);
#else
  ACE_UNUSED_ARG (filename);
  ACE_UNUSED_ARG (offset);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

ACE_INLINE useconds_t
ACE_OS::ualarm (useconds_t usecs, useconds_t interval)
{
  ACE_OS_TRACE ("ACE_OS::ualarm");

#if defined (ACE_HAS_UALARM)
  return ::ualarm (usecs, interval);
#elif !defined (ACE_LACKS_UNIX_SIGNALS) && !defined (ACE_LACKS_ALARM)
  ACE_UNUSED_ARG (interval);
# if defined (ACE_VXWORKS) && ACE_VXWORKS >= 0x690 && defined (_WRS_CONFIG_LP64)
  return ::alarm (static_cast<unsigned int> (usecs * ACE_ONE_SECOND_IN_USECS));
# else
  return ::alarm (usecs * ACE_ONE_SECOND_IN_USECS);
#endif
#else
  ACE_UNUSED_ARG (usecs);
  ACE_UNUSED_ARG (interval);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_UALARM */
}

ACE_INLINE useconds_t
ACE_OS::ualarm (const ACE_Time_Value &tv,
                const ACE_Time_Value &tv_interval)
{
  ACE_OS_TRACE ("ACE_OS::ualarm");

#if defined (ACE_HAS_UALARM)
  useconds_t usecs = (tv.sec () * ACE_ONE_SECOND_IN_USECS) + tv.usec ();
  useconds_t interval =
    (tv_interval.sec () * ACE_ONE_SECOND_IN_USECS) + tv_interval.usec ();
  return ::ualarm (usecs, interval);
#elif !defined (ACE_LACKS_UNIX_SIGNALS) && !defined (ACE_LACKS_ALARM)
  ACE_UNUSED_ARG (tv_interval);
# if defined (ACE_VXWORKS) && ACE_VXWORKS >= 0x690 && defined (_WRS_CONFIG_LP64)
  return ::alarm (static_cast<unsigned int> (tv.sec ()));
# else
  return ::alarm (tv.sec ());
# endif
#else
  ACE_UNUSED_ARG (tv_interval);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_UALARM */
}

ACE_INLINE int
ACE_OS::unlink (const char *path)
{
  ACE_OS_TRACE ("ACE_OS::unlink");
#if defined (ACE_LACKS_UNLINK)
  ACE_UNUSED_ARG (path);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_UNLINK_EQUIVALENT)
  return ACE_UNLINK_EQUIVALENT (path);
#else
  return ::unlink (path);
#endif /* ACE_LACKS_UNLINK */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
ACE_OS::unlink (const wchar_t *path)
{
  ACE_OS_TRACE ("ACE_OS::unlink");
# if defined (ACE_WIN32)
  return ::_wunlink (path);
# else
  ACE_Wide_To_Ascii npath (path);
  return ACE_OS::unlink (npath.char_rep ());
# endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE ssize_t
ACE_OS::write (ACE_HANDLE handle, const void *buf, size_t nbyte)
{
  ACE_OS_TRACE ("ACE_OS::write");
#if defined (ACE_WIN32)
  DWORD bytes_written; // This is set to 0 byte WriteFile.

  // Strictly correctly, we should loop writing all the data if more
  // than a DWORD length can hold.
  DWORD short_nbyte = static_cast<DWORD> (nbyte);
  if (::WriteFile (handle, buf, short_nbyte, &bytes_written, 0))
    return (ssize_t) bytes_written;
  else
    ACE_FAIL_RETURN (-1);
#elif defined (ACE_MQX)
  return MQX_Filesystem::inst ().write (handle, reinterpret_cast<const unsigned char *> (buf), nbyte);
#elif defined (ACE_LACKS_WRITE)
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (nbyte);
  ACE_NOTSUP_RETURN (-1);
#else
# if defined (ACE_HAS_CHARPTR_SOCKOPT)
  return ::write (handle, static_cast <char *> (const_cast <void *> (buf)), nbyte);
# else
  return ::write (handle, buf, nbyte);
# endif /* ACE_HAS_CHARPTR_SOCKOPT */
#endif /* ACE_WIN32 */
}

ACE_INLINE ssize_t
ACE_OS::write (ACE_HANDLE handle,
               const void *buf,
               size_t nbyte,
               ACE_OVERLAPPED *overlapped)
{
  ACE_OS_TRACE ("ACE_OS::write");
#if defined (ACE_WIN32)
  DWORD bytes_written; // This is set to 0 byte WriteFile.

  DWORD short_nbyte = static_cast<DWORD> (nbyte);
  if (::WriteFile (handle, buf, short_nbyte, &bytes_written, overlapped))
    return (ssize_t) bytes_written;
  else
    ACE_FAIL_RETURN (-1);
#else
  ACE_UNUSED_ARG (overlapped);
  return ACE_OS::write (handle, buf, nbyte);
#endif /* ACE_WIN32 */
}

ACE_END_VERSIONED_NAMESPACE_DECL
