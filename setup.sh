#!/bin/bash
set -e

echo -n "Deploying git hooks: "

HOOKDIR=.git/hooks

# when repo is a subrepo, .git is a file describing the path to the actual gitdir
if [ -f .git ]
then
    # reroute $HOOKDIR to the actual directory
    HOOKDIR=$(perl -nle 'print $1 if /gitdir: (.+)/' .git)
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
