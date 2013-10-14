#!/bin/bash

function handle_input()
{
   while [ "$*" != "" ]
   do
      echo "|$1|"
      shift
   done
}

function handle_input2()
{
   echo "==== $#"
   echo -n "$1-$2-$3-$4-$5"
   echo ""
   echo "****"
}

IFS="
"

OUTPUT=`./t`
#OUTPUT=`ls -l ../test`

for line in $OUTPUT
do
   IFS=" "
   handle_input2 $line
   IFS="
"
done
