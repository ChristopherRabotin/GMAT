#!/bin/bash

set -e

appid=$1

builddir=flatpak_app
repodir=repo

flatpak-builder \
        --user --disable-rofiles-fuse \
        --stop-at=gtk \
        ${builddir} \
        build-aux/flatpak/${appid}.json

flatpak build ${builddir} meson \
                --prefix=/app \
                --libdir=/app/lib \
                --buildtype=release \
                -Dx11-backend=true \
                -Dwayland-backend=true \
                -Dprint-backends=file \
                -Dbuild-tests=false \
                -Dbuild-examples=false \
                -Dintrospection=disabled \
                -Ddemos=true \
                -Dprofile=devel \
                _flatpak_build

flatpak build ${builddir} ninja -C _flatpak_build install

flatpak-builder \
        --user --disable-rofiles-fuse \
        --finish-only \
        --repo=${repodir} \
        ${builddir} \
        build-aux/flatpak/${appid}.json

flatpak build-bundle \
        ${repodir} \
        ${appid}-dev.flatpak \
        --runtime-repo=https://nightly.gnome.org/gnome-nightly.flatpakrepo \
        ${appid}

# to be consumed by the nightly publish jobs
if [[ $CI_COMMIT_BRANCH == main ]]; then
        tar cf repo.tar ${repodir}
fi
