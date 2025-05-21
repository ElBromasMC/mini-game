FROM alpine:latest

# Install dependencies
RUN apk update \
    && apk add --no-cache \
    alpine-sdk \
    clang19 \
    bear \
    git \
    raylib-dev \
    alsa-lib-dev \
    mesa \
    mesa-utils \
    mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libxcursor-dev \
    libxinerama-dev \
    wayland-dev \
    libxkbcommon-dev \
    wayland-protocols

RUN apk add --no-cache freeglut-dev

RUN apk add --no-cache watchexec

# Create and change to non-root user
RUN addgroup -g 1000 devrunner \
    && adduser -G devrunner -u 1000 -D devrunner \
    && addgroup devrunner video
USER devrunner
WORKDIR /home/devrunner

# Create required folders
RUN mkdir /home/devrunner/src
VOLUME /home/devrunner/src

# Default entrypoint
WORKDIR /home/devrunner/src

