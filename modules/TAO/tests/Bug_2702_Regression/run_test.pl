eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
     & eval 'exec perl -S $0 $argv:q'
     if 0;

# $Id$
# -*- perl -*-

use lib "$ENV{ACE_ROOT}/bin";
use PerlACE::Run_Test;

$iorfile = PerlACE::LocalFile ("server_on_localhost_1192.ior");
$status = 0;

## Get the perl interpreter that invoked us and remove any
## executable extension (if there is one).
my($perl) = $^X;
$perl =~ s/\.exe$//i;

$SV = new PerlACE::Process ($perl, "fakeserver2.pl");
$CL = new PerlACE::Process ("client", " -k file://$iorfile -ORBdebuglevel 1 -ORBlogfile client.log");
unlink "client.log";

$SV->IgnoreExeSubDir(1);
$SV->Spawn ();
sleep(1);  # give the server a chance to come up

if (PerlACE::waitforfile_timed ($iorfile,
                        $PerlACE::wait_interval_for_process_creation) == -1) {
    print STDERR "ERROR: cannot find file <$iorfile>\n";
    $SV->Kill (); $SV->TimedWait (1);
    exit 1;
}

$client = $CL->SpawnWaitKill (60);

# We expect to have to kill both client and server.

#if ($client != 0) {
#    print STDERR "ERROR: client returned $client\n";
#    $status = 1;
#}

$server = $SV->WaitKill (10);

#if ($server != 0) {
#    print STDERR "ERROR: server returned $server\n";
#    $status = 1;
#}

open (LOG, "client.log") or die "Couldn't open client log file client.log: $!\n";
while (<LOG>) {
  $ccmsgfound = 1 if (/process_parsed_messages, received CloseConnection message/);
}
close (LOG);
if (not $ccmsgfound) {
  print STDERR "ERROR: didn't find CloseConnection debug message in client log.\n";
  $status = 1;
}

exit $status;
