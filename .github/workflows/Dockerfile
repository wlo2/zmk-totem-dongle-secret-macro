# ghcr.io/OWNER/REPO-zmk-zephyr:4.1.0
FROM zmkfirmware/zmk-build-arm:stable

ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
WORKDIR /opt/zmk-env

# Initialize west using ZMK beta branch (Zephyr 4.1 migration)
# West is preinstalled in the base image
RUN west init -m https://github.com/zmkfirmware/zmk --mr move-to-zephyr-4-1 . && \
    west update --fetch-opt=--filter=tree:0 && \
    west zephyr-export

# Install Twister Python deps from Zephyr
RUN python3 -m pip install --upgrade pip && \
    pip install -r zephyr/scripts/requirements.txt

# Optional: keep Twister cache writable
ENV PIP_CACHE_DIR=/tmp/pip-cache