#!/usr/bin/perl -w
use strict;

# this file is used to run the entire test.
# for the script to work properly, it should be in the same directory
# as stage2.sh, with template/branch_mispred.c

# these sentinels are to indicate the start and stop of line
# replacement in branch_mispred.c
my $SENTINEL_START = "DJB2167 START";
my $SENTINEL_STOP = "DJB2167 STOP";

my $GENERATED_START = "------------------------------------------------------------------------------------------s";
my $GENERATED_STOP = "------------------------------------------------------------------------------------------p";

my $TEST_RESULT_FILE = "full_results.txt";

# run the C code on the provided query file and submission file.

if (@ARGV != 2) {
    die("usage: ./run_test.pl <query_file> <config_file>");
}


# run the query optimizer on the given query file and config file.
`./stage2.sh $ARGV[0] $ARGV[1] > stage2_result.txt`;
if ($? != 0) {
    die ("stage2.sh did not exit successfully.\n");
}

my $counter = 0; # TODO: remove this from final script; just for testing.
my @generated_cs;
open(GFIN, "<", "stage2_result.txt") || die ("failed to open stage2_result.txt");
while (<GFIN>) {
    # find every generated result.
    if (/$GENERATED_START/../$GENERATED_STOP/) {

	if ($_ =~ $GENERATED_START) {
	    push(@generated_cs, ""); # push an empty string to start this c snippet.
	} elsif ($_ =~ $GENERATED_STOP) {
	    $counter += 1; # prepare for next snippet.
	} else {
	    $generated_cs[$counter] .= $_;
	}

    } # end line range selection from GFIN
}



# foreach (@generated_cs) {
#     print "*************************\n";
#     print $_;
#     print "*************************\n";
# }
# exit;


$counter = 0; # get rid of this, eventually.
foreach (@generated_cs) {
    my $c_snippet = $_; # stash it, since $_ will change momentarily.
    open(FIN, "<", "template/branch_mispred.c") || die("failed to open 'template/branch_mispred.c'\n");
    open(FOUT, ">", "template/branch_mispred$counter.c") || die ("failed to open 'template/branch_mispred$counter.c'\n");

    while (<FIN>) {
	if (/$SENTINEL_START/../$SENTINEL_STOP/) {
	    if ($_ =~ $SENTINEL_START) {
		# keep the start sentinel;
		print FOUT $_;
	    } elsif($_ =~ $SENTINEL_STOP) {
		# replace it with the new value.
		print FOUT $c_snippet;

		print FOUT $_; # SENTINEL STOP line.
		$counter += 1;
	    } else {
		# do nothing; discard.
	    }
	} else {
	    print FOUT $_;
	}
    }
    close(FIN);
}

# at this point, we have generated all the files that are necessary to
# run the experiment.

chdir("template/");
# we already have the original branch_mispred.c file backed up, so we
# feel no regret in overwriting it.
`ls`;
my $test_idx = 0;
open (QUERY_IN, "<", "../$ARGV[0]") || die ("failed to open the query file...again.");
while (<QUERY_IN>) {
    # prep the selectivities line.
    my $selectivities = $_;
    chomp($selectivities);


    # build the new branch_mispred.c
    my $new_file = "branch_mispred" . $test_idx . ".c";
    `cp $new_file branch_mispred.c`;
    `make 2> /dev/null`; # quiet make

    # run the actual test.
    `echo '*************************' >> $TEST_RESULT_FILE`;
    `echo 'results of test $test_idx' >> $TEST_RESULT_FILE`;
    `echo './branch_mispred $selectivities' >> $TEST_RESULT_FILE`;
    `echo '*************************' >> $TEST_RESULT_FILE`;

    `./branch_mispred $selectivities >> $TEST_RESULT_FILE`;
    `echo '*******TEST DONE******' >> $TEST_RESULT_FILE`;

    # prep for next loop
    print "test $test_idx complete.\n";
    $test_idx += 1;
}

chdir("../");
print "test complete.\n";
