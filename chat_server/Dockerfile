FROM alpine:latest
MAINTAINER org.opencontainers.image.authors="guminh@naver.com"
ENV PORT 8080

WORKDIR /var/app
COPY . .
RUN apk update && apk add gcc musl-dev --no-cache
RUN gcc chat_socket_server.c --output chat_socket_server -pthread

CMD ["sh", "-c", "./chat_socket_server $PORT"]
