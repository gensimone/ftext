#!/bin/sh

../ftext -c 3 -w 21 -g 6 -l 24 <./input.txt >provided.txt

if [ $? -gt 0 ]; then
	echo "ftext: error." >&2
	echo "Test aborted." >&2
	exit 1
fi

cat ./provided.txt

expected_words=$(cat ./expected.txt | wc -w)
received_words=$(cat ./provided.txt | wc -w)

if ! [ $received_words -eq $expected_words ]; then
	echo "Test failed"
	echo ":: expected: $expected_words"
	echo ":: received: $received_words"
else
	echo "Test passed"
fi

exit 0
