#!/bin/bash

# Command to run the container using the bind mount functionality to make the directory of
# the host computer accessible from inside the container
docker run --rm -it --mount src="$(pwd)",target=/stoc,type=bind jgarciapueyo/stoc-build