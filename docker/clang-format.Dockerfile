FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

# Install minimal tools and clang-format-18
RUN apt-get update &&\
    apt-get install -y clang-format

WORKDIR /workspace
