#!/bin/bash
set -e

echo -n "Deploying git hooks: "

HOOKDIR=.git/hooks

# when repo is a subrepo, .git is not a directory, but a file describing the path to the actual gitdir
if [ ! -d .git ]
then
    echo "IGNORING SINCE SUBMODULE"
    exit
fi

# remove old hooks
rm -rf $HOOKDIR
mkdir -p $HOOKDIR

# copy new hooks, make executable
cp utils/githooks/* $HOOKDIR/
for file in $HOOKDIR/*
do
  chmod u+x $file
done
echo "SUCCESS"
