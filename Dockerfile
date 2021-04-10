# FROM centos:7 as SourceBuilder
FROM ubuntu:latest as SourceBuilder

# RUN yum install gcc zlib-devel make -y
RUN apt update
RUN apt install gcc zlib1g-dev make gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu -y

COPY ./dystopia /dystopia

WORKDIR /dystopia/src
RUN /usr/bin/make clean
RUN /usr/bin/make

WORKDIR /dystopia
RUN find . -type d | xargs -I {} mkdir -p /build/dystopia/{}

RUN cp -R /dystopia/area /dystopia/txt /dystopia/doc /dystopia/notes /build/dystopia
RUN cp -R /dystopia/src/dystopia /build/dystopia/area/

FROM scratch
COPY --from=SourceBuilder /build/dystopia /dystopia
  # COPY --from=SourceBuilder /lib/ld-linux-aarch64.so.1 /lib/
# COPY --from=SourceBuilder /lib64/libz.so.1 /lib64/
# COPY --from=SourceBuilder /lib64/libcrypt.so.1 /lib64/
# COPY --from=SourceBuilder /lib64/libpthread.so.0 /lib64/
# COPY --from=SourceBuilder /lib64/libc.so.6 /lib64/
# COPY --from=SourceBuilder /lib64/libfreebl3.so /lib64/
# COPY --from=SourceBuilder /lib64/libdl.so.2 /lib64/
# COPY --from=SourceBuilder /lib64/libgcc_s.so.1 /lib64/
# COPY ./libs/ld-2.26.so /lib/
WORKDIR /dystopia/area
EXPOSE 9000
ENTRYPOINT ["./dystopia", "9000"]
