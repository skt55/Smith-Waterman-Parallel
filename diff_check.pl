#!/usr/bin/perl
use strict;
use warnings;

# Check for correct number of arguments
if (@ARGV != 2) {
    die "Usage: perl compare_files.pl file1.txt file2.txt\n";
}

my ($file1, $file2) = @ARGV;

# Open the first file
open(my $fh1, '<', $file1) or die "Cannot open $file1: $!";

# Open the second file
open(my $fh2, '<', $file2) or die "Cannot open $file2: $!";

my $line_num = 1;
my $differences = 0;

while (1) {
    my $line1 = <$fh1>;
    my $line2 = <$fh2>;

    # End of both files
    last unless defined $line1 or defined $line2;

    # Remove trailing newline characters for accurate comparison
    chomp($line1) if defined $line1;
    chomp($line2) if defined $line2;

    if (!defined $line1) {
        print "File1 ended, File2 has additional line $line_num: $line2\n";
        $differences++;
    }
    elsif (!defined $line2) {
        print "File2 ended, File1 has additional line $line_num: $line1\n";
        $differences++;
    }
    elsif ($line1 ne $line2) {
        print "Difference at line $line_num:\n";
        print "File1: $line1\n";
        print "File2: $line2\n";
        $differences++;
    }

    $line_num++;
}

close($fh1);
close($fh2);

if ($differences == 0) {
    print "Files are identical.\n";
} else {
    print "Total differences found: $differences\n";
}

