FROM muslcc/i686:x86_64-linux-musl AS build

RUN apk update && \
    apk add make

WORKDIR /usr/src/
COPY src .
RUN make


FROM scratch
COPY --from=build /usr/src/server /usr/bin/http-redirect

ENTRYPOINT [ "/usr/bin/http-redirect" ]
