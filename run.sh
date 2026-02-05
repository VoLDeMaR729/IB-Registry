#!/bin/bash

xhost +local:docker
sudo docker compose down --remove-orphans
sudo docker compose up --build
