# ghcr.io/OWNER/REPO-zmk-zephyr:4.1.0
FROM zmkfirmware/zmk-build-arm:stable

ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
WORKDIR /opt/zmk-env

# Seed west manifest (use your module's manifest if needed; empty config dir is fine for Zephyr upstream)
RUN mkdir -p /opt/zmk-env/config

# Initialize and fetch Zephyr + modules per zmk's manifest
# West is preinstalled in the base image
RUN west init -l /opt/zmk-env/config && \
    west update --fetch-opt=--filter=tree:0 && \
    west zephyr-export

# Install Twister Python deps from Zephyr
RUN python3 -m pip install --upgrade pip && \
    pip install -r zephyr/scripts/requirements.txt

# Optional: keep Twister cache writable
ENV PIP_CACHE_DIR=/tmp/pip-cache