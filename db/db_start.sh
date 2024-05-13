#!/bin/bash

# The name of the image
image_name="mysql_db"

# Stop, remove and start the first container
container_name="kmsDBA"
docker stop $container_name
docker rm $container_name
docker run --name $container_name -d $image_name

# Stop, remove and start the second container
container_name="kmsDBB"
docker stop $container_name
docker rm $container_name
docker run --name $container_name -d $image_name

sleep 2
echo "Both containers started."