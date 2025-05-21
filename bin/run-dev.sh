#!/bin/sh

exec podman-compose -f docker-compose.yml \
    run --rm --build \
    devrunner

