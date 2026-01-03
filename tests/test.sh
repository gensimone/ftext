#!/bin/sh

res=$(../ftext < ./input.txt)

if [ $? -gt 0 ]; then
    echo "ftext: error." >&2
    echo "Test aborted." >&2
    exit 1
fi

expected_words=$(cat ./expected.txt | wc -w)
received_words=$(echo $res | wc -w)

if ! [ $received_words -eq $expected_words ]; then
    echo "Test failed"
    echo ":: expected: $expected_words"
    echo ":: received: $received_words"
else
    echo "Test passed"

exit 0
