#!/usr/bin/env bash

# Builds the ECR image reference from Account, Region, and Tag
ACCOUNT=$(aws sts get-caller-identity | jq -r .Account)
REGION=$(curl -s 169.254.169.254/latest/meta-data/placement/availability-zone | sed 's/[a-z]$//')
if [[ -z $1 ]]; then
  # Get a list of image tags in the repository, sort numerically and return the most recent (highest) tag
  TAG=$(aws ecr list-images --repository-name dystopiagold | jq -r .imageIds[].imageTag | sort -n | tail -n 1)
else
  # Supplied with the command as an argument
  TAG=$1
fi
IMAGE="${ACCOUNT}.dkr.ecr.${REGION}.amazonaws.com/dystopiagold:${TAG}"

# Identifies if container is already running
RUNNING=$(/usr/bin/docker ps -q -f name=dystopiagold)

if [[ ! -z $RUNNING ]]; then
  echo "Dystopia Gold is currently running."
else
  # Launches Dystopia Gold
  /usr/bin/docker run -d -n dystopiagold --restart always $IMAGE
  if [[ $? != 0 ]]; then
    echo "Issue with starting Dystopia Gold."
  else
    echo "Started Dystopia Gold."
  fi
fi
