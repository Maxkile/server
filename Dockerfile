FROM ubuntu:latest
RUN mkdir -p /usr/share/cli
WORKDIR /usr/share/cli
COPY ./client ./
EXPOSE 2424