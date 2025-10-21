FROM zmkfirmware/zmk-build-arm:stable

ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
WORKDIR /opt/zmk-env

# Initialize west
RUN west init -m https://github.com/petejohanson/zmk --mr core/move-to-zephyr-4-1 --mf app/west.yml . && \
    west update --fetch-opt=--filter=tree:0 && \
    west zephyr-export