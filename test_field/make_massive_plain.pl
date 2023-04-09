my $num_files = 50000;
my @names = ();
for (1..$num_files) {
	my $filename = "massive_plain/";
	$filename .= ("a".."z", "A".."Z", "0".."9")[rand 62] for 1..12;
	push @names, $filename;
	if (($#names + 1) % 200 == 0 || $#names + 1 == $num_files) {
		my $ns = join(" ", @names);
		`touch $ns`;
		@names = ();
	}
}
