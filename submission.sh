#! /bin/bash

if [[ $# -ne "1" ]]; then
  echo "Invalid arguments passed: $*"
  echo "Usage: submission.sh \$userName"
  exit 1
fi

set user=$1
git add -A 
git commit -m "$user - Auto-commit for submission: `date`"
git archive -o $user-lab4asn.zip --format zip --prefix $user/ HEAD
