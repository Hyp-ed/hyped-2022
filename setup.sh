#!/bin/bash
set -e

echo -n "Deploying git hooks: "
HOOKDIR=.git/hooks

# remove old hooks
rm -rf $HOOKDIR
mkdir -p $HOOKDIR

# copy new hooks, make executable
cp utils/githooks/* $HOOKDIR/
for file in .git/hooks/*
do
  chmod u+x $file
done
echo "SUCCESS"
