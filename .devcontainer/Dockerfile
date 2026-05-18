# Pin to linux/amd64 explicitly so this works on Apple Silicon without runArgs --platform
FROM --platform=linux/amd64 ghcr.io/zephyrproject-rtos/ci-base:latest

# SDK version must match the Zephyr version in the workspace (v4.4+ -> SDK 1.0.x)
ARG ZEPHYR_SDK_VERSION=1.0.1

# Install the Zephyr SDK cmake packages only (no cross-compilers, no host tools needed for native_sim)
# The ci-base image already provides system tools (dtc, openocd, etc.) so -h is not needed.
# -c registers the CMake package files so Zephyr's build system can locate the SDK.
RUN wget -q "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" \
    && tar -xf "zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" -C /opt \
    && rm "zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" \
    && /opt/zephyr-sdk-${ZEPHYR_SDK_VERSION}/setup.sh -c

ENV ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk-${ZEPHYR_SDK_VERSION}

WORKDIR /workdir
