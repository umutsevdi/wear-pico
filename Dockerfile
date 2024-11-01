# Use the official Debian base image
FROM ubuntu:20.04

# Set the maintainer information
LABEL maintainer="Umut Sevdi <mail@umutsevdi.com>"

RUN echo "export PICO_SDK_PATH=/pico-sdk" >> /root/.bashrc

RUN apt update && apt-get install -y \
    cmake \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    vim \
    python3 \
    g++ doxygen

# Any other setup or commands you need to run during the build process
# For example, you can install packages, configure settings, etc.
# RUN apt-get update && apt-get install -y package-name

# Set a default command to run when the container starts
CMD ["/bin/bash"]
