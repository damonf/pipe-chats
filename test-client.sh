#!/bin/bash

padding=''

for x in {1..59};
do
  next=$(echo -n '\0')
  padding+=$next
done 

message='\x01\0\0\0hello'
message+=$padding

echo $message

echo -n -e $message > /tmp/pipe-chat-svr
