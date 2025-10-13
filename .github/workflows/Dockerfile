FROM zmkfirmware/zmk-build-arm:stable

ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
WORKDIR /opt/zmk-env

# Initialize west using petejohanson/zmk core/move-to-zephyr-4-1 manifest
RUN west init -m https://github.com/petejohanson/zmk --mr core/move-to-zephyr-4-1 --mf app/west.yml . && \
    west update --fetch-opt=--filter=tree:0 && \
    west zephyr-export

# Install pip (and venv) in the image, then Twister deps from Zephyr
RUN apt-get update && \
    apt-get install -y --no-install-recommends python3-pip python3-venv ca-certificates && \
    rm -rf /var/lib/apt/lists/* && \
    python3 -m pip install --upgrade pip && \
    pip install -r zephyr/scripts/requirements.txt

ENV PIP_CACHE_DIR=/tmp/pip-cache