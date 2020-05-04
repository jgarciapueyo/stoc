#!/bin/bash

# Command to run the container making it useful to remote development through ssh
# From https://blog.jetbrains.com/clion/2020/01/using-docker-with-clion/
# The -d runs the container as a daemon, so control returns back to you.
# --cap-add sys_ptrace adds the ptrace capability, which is necessary for debugging.
docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 --name stoc-remote-dev stoc-remote-dev