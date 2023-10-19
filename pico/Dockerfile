# Use the official Debian base image
FROM debian:bookworm-slim

# Set the maintainer information
LABEL maintainer="Umut Sevdi <mail@umutsevdi.com>"

# Copy files from the host into the image
COPY ./install.sh /install.sh
RUN /bin/bash 'install.sh'

# Any other setup or commands you need to run during the build process
# For example, you can install packages, configure settings, etc.
# RUN apt-get update && apt-get install -y package-name

# Set a default command to run when the container starts
CMD ["/bin/bash"]
