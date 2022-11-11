FROM alpine:3.16.2 as builder
RUN apk update && apk add\
    make=4.3-r0\
    git=2.36.3-r0\
    cmake=3.23.1-r0\
    libc-dev=0.7.2-r3\
    build-base=0.5-r3\
    curl-dev=7.83.1-r4
WORKDIR /clib
RUN git clone https://github.com/clibs/clib.git
RUN cd clib && make && make install
RUN clib --version
WORKDIR /src
COPY . .
RUN make clib && make build

FROM alpine:3.16.2
WORKDIR /bin
COPY --from=builder /src/build/commutator commutator
RUN mkdir -p /tmp/d /tmp/c
ENTRYPOINT [ "commutator" ]
