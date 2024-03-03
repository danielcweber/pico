FROM muslcc/i686:x86_64-linux-musl AS build

RUN apk update && \
    apk add make git

WORKDIR /usr/src/
RUN git clone -b fork https://github.com/danielcweber/pico.git

WORKDIR pico
RUN make


FROM scratch
COPY --from=build /usr/src/pico/server /usr/bin/http-redirect

ENTRYPOINT [ "/usr/bin/http-redirect" ]
