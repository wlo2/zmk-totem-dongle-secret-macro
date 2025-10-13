FROM zmkfirmware/zmk-build-arm:stable

ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
WORKDIR /opt/zmk-env

# Initialize west using petejohanson/zmk core/move-to-zephyr-4-1 manifest
RUN west init -m https://github.com/petejohanson/zmk --mr core/move-to-zephyr-4-1 --mf app/west.yml . && \
    west update --fetch-opt=--filter=tree:0 && \
    west zephyr-export

# Create venv and install Twister deps into it (PEP 668 compliant)
RUN apt-get update && \
    apt-get install -y --no-install-recommends python3-venv ca-certificates && \
    rm -rf /var/lib/apt/lists/* && \
    python3 -m venv /opt/venv && \
    /opt/venv/bin/python -m ensurepip --upgrade && \
    /opt/venv/bin/pip install --upgrade pip && \
    /opt/venv/bin/pip install -r zephyr/scripts/requirements.txt

# Ensure venv is used by default
ENV PATH="/opt/venv/bin:${PATH}"