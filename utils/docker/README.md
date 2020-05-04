## Building Stock with Docker

### System Requirements
You will need to install [docker](https://docs.docker.com/get-docker/).

### Explanation
* Dockerfile.stoc-build is a Dockerfile that contains the necessary dependencies to build the Stoc compiler.
    
    - Go to the root path of the repository
    - Build the image `stoc-build` 
    ```sh
    docker build -t stoc-build -f utils/docker/Dockerfile.stoc-build .
    ```
    - Run the container
    ```sh
    bash utils/docker/stoc-build-container.sh
    ```
    or 
    ```sh
    docker run -it --mount src="$(pwd)",target=/stoc,type=bind stoc-build
    ```
    As you can see, when running the docker container, you make use of the bind mount functionality to mount the directory
    of the host machine into the container. Please, see [Use bind mounts](https://docs.docker.com/storage/bind-mounts/)
    to learn more.
    - Already inside the container, check that you are in the directory: `/stoc`. Then, build the compiler.
    ```sh
    cmake .
    make
    ```
    - Run the compiler
    ```sh
    ./stoc <file.st>
    ```
  
- For the development of `stoc`, it has been used the [Clion IDE](https://www.jetbrains.com/clion/) using docker containers
for building the project through ssh remote development. To do this in Clion or any other IDE or code editor that supports it:
    
    - Go to the root path of the repository
    - Build the image `stoc-remote-dev`
    ```sh
    docker build -t stoc-remote-dev -f utils/docker/Dockerfile.stoc-remote-dev .
    ```
    - Run the container
    ```sh
    bash utils/docker/stoc-remote-dev-conainer.sh
    ```
    or
    ```sh
    docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 --name stoc-remote-dev stoc-remote-dev
    ```
    This opens in localhost the port 2222 to be able to ssh into the container with user dev and password dev.
    - Configure [Clion IDE to develop in remote mode with docker containers](https://blog.jetbrains.com/clion/2020/01/using-docker-with-clion/)
    or any other IDE or code editor that supports it.
  
    
    
