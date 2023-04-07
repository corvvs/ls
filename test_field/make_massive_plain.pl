$num_files = 2000;
for (1..$num_files) {
	$filename = "";
	$filename .= ("a".."z", "A".."Z", "0".."9")[rand 62] for 1..12;
	`touch $filename`
}
