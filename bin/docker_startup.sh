#!/usr/bin/env bash

# General Configuration
MUD_NAME="Dystopia Gold"

# Port Mapping
HOST_PORT=9000
CONTAINER_PORT=9000

# Container Configuration
CONTAINER_NAME="dystopiagold"
MUD_DIRECTORY="dystopia"

# Repository Configuration
REPOSITORY_NAME="dystopiagold"

# Builds the ECR image reference from Account, Region, and Tag
ACCOUNT=$(aws sts get-caller-identity | jq -r .Account)
REGION=$(curl -s 169.254.169.254/latest/meta-data/placement/availability-zone | sed 's/[a-z]$//')
if [[ -z $1 ]]; then
  # Get a list of image tags in the repository, sort numerically and return the most recent (highest) tag
  TAG=$(aws ecr list-images --repository-name ${REPOSITORY_NAME} | jq -r .imageIds[].imageTag | sort -n | tail -n 1)
else
  # Supplied with the command as an argument
  TAG=$1
fi
IMAGE="${ACCOUNT}.dkr.ecr.${REGION}.amazonaws.com/${REPOSITORY_NAME}:${TAG}"

# Login to ECR
aws ecr get-login-password --region ${REGION} | docker login --username AWS --password-stdin ${ACCOUNT}.dkr.ecr.${REGION}.amazonaws.com

# Verify ECR login worked and then prepare to start container
if [[ $? == 0 ]]; then

  # Identifies if container is already running
  RUNNING=$(/usr/bin/docker ps -q -f name=${CONTAINER_NAME})

  if [[ ! -z $RUNNING ]]; then
    echo "${MUD_NAME} is currently running."
  else
    # Launches Container
    /usr/bin/docker run -d \
      --name ${CONTAINER_NAME} \
      -p ${HOST_PORT}:${CONTAINER_PORT} \
      -v ${CONTAINER_NAME}_player:/${MUD_DIRECTORY}/player \
      --restart always \
      $IMAGE
    if [[ $? != 0 ]]; then
      echo "Issue with starting ${MUD_NAME}."
    else
      echo "Started ${MUD_NAME}."
    fi
  fi

else
  echo "Issue with logging into Docker registry."
fi
