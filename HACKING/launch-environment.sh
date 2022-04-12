#!/bin/bash
IMAGINE_DIR=/home/$USER/Documents/imagine2022-tags
MOUNT_DIR=/mnt/ImagineRIT2022
mkdir -p "$IMAGINE_DIR"
podman run --rm -it                             \
    --name=imaginerit-tag-dev              \
    --device=/dev/ttyUSB0  \
    -v "$IMAGINE_DIR":"$MOUNT_DIR"              \
    --group-add keep-groups                     \
    --security-opt label=disable                \
    --annotation run.oci.keep_original_groups=1 \
    imaginerit-embedded-dev
#podman exec -it imaginerit-embedded-dev bash
