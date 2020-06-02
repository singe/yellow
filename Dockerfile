FROM alpine:latest as builder
LABEL maintainer="@singe at SensePost <research@sensepost.com>"

RUN apk update && apk --no-cache add \
    build-base \
  && rm -rf /var/cache/apk/*
WORKDIR /
COPY yellow.c canary32.c canary32.h /
RUN gcc -o yellow yellow.c canary32.c

From alpine:latest
LABEL maintainer="@singe at SensePost <research@sensepost.com>"

COPY --from=builder /yellow /bin/

RUN mv /usr/bin/id /usr/bin/id.canary \
  && ln -s /bin/yellow /usr/bin/id
