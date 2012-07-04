#! /bin/sh

VERSION="0.1"
MAINTAINER="Frank Denis <j@pureftpd.org>"
CATEGORY="net"
URL="https://github.com/jedisct1/msgpack-autosplit"
VENDOR="Frank Denis"
DESCRIPTION="A simple tool to safely rotate logs made of MessagePack records.
Since records in a MessagePack stream are not delimited by carriage
returns, tools like Logrotate can hardly safely rotate this kind of
log file without breaking arbitrary records.

msgpack-autosplit reads a MessagePack stream on the standard input,
writes this stream to disk, and automatically, and safely perform
logfile rotation after a file reaches a maximum size, or after a
maximum delay."
TMPDIR=${TMPDIR:-/tmp}
BASE_DIR=$(mktemp -d "$TMPDIR"/msgpack-autosplit.XXXXXX)
INSTALL_DIR="${BASE_DIR}/usr"
PKG_NAME="msgpack-autosplit"
COPYRIGHT_FILE="COPYING"
DEBIAN_COPYRIGHT_FILE="${INSTALL_DIR}/share/doc/${PKG_NAME}/copyright"
DEBIAN_CHANGELOG_FILE="${INSTALL_DIR}/share/doc/${PKG_NAME}/changelog.gz"
LICENSE="bsd"

export TZ=""
export LC_ALL="C"
export LC_TIME="C"

./configure --prefix="$INSTALL_DIR" && make -j4 install

mkdir -p -- $(dirname "$DEBIAN_COPYRIGHT_FILE") || exit 1
cp -- "$COPYRIGHT_FILE" "$DEBIAN_COPYRIGHT_FILE" || exit 1

echo "${PKG_NAME} (${VERSION}) unstable; urgency=low
  * See ${URL}

 -- ${MAINTAINER}  $(date -R)" | gzip -9 > "$DEBIAN_CHANGELOG_FILE"

find "${INSTALL_DIR}/share/man" -type f -name "*.[0-9]" -exec gzip -9 {} \;

[ -d "${INSTALL_DIR}/bin" ] && \
  find "${INSTALL_DIR}/bin" -type f -perm +111 -exec strip {} \;

[ -d "${INSTALL_DIR}/sbin" ] && \
  find "${INSTALL_DIR}/sbin" -type f -perm +111 -exec strip {} \;

find "$BASE_DIR" -type d -exec chmod 755 {} \;

sudo chown -R 0:0 "$BASE_DIR" || exit 1

for t in deb rpm; do
  fpm -s dir -t "$t" -n "$PKG_NAME" -v "$VERSION" -C "$BASE_DIR" \
    -m "$MAINTAINER" --category "$CATEGORY" --url "$URL" --license "$LICENSE" \
    --vendor "$VENDOR" --description "$DESCRIPTION" \
    .
done
