FROM zmkfirmware/zmk-build-arm:stable

ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
WORKDIR /opt/zmk-env

# Initialize west and ensure Zephyr SDK is exported
RUN west init -m https://github.com/zmkfirmware/zmk --mr main --mf app/west.yml . && \
    west update --fetch-opt=--filter=tree:0 && \
    west zephyr-export && \
    echo "export ZEPHYR_BASE=/opt/zmk-env/zephyr" >> /root/.bashrc

# Ensure Zephyr SDK paths are available
ENV ZEPHYR_BASE=/opt/zmk-env/zephyr
ENV CMAKE_PREFIX_PATH=${ZEPHYR_BASE}