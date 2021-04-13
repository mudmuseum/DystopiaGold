#!/usr/bin/env bash

RUNNING=$(/usr/bin/docker ps -q -f name=dystopiagold)

if [[ ! -z $RUNNING ]]; then
  echo "Dystopia Gold is currently running."
else
  /usr/bin/docker run -d -n dystopiagold --restart always
  if [[ $? != 0 ]]; then
    echo "Issue with starting Dystopia Gold."
  else
    echo "Started Dystopia Gold."
  fi
fi
