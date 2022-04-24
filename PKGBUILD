# Maintainer: Matias Hall <>
pkgname=rss_subscriber
pkgver=0.1
pkgrel=1
pkgdesc="A terminal feed aggregator."
arch=('x86_64')
url="https://github.com/Matias-Hall/rss_subscriber"
license=('GPL')
depends=()
makedepends=('libxml2' 'curl')
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
source=("src/")
noextract=()
md5sums=()
validpgpkeys=()

build() {
	cmake -B build -S "." \
		-DCMAKE_BUILD_TYPE='Release' \
		-DCMAKE_INSTALL_PREFIX='/usr' \
		-Wno-dev
	cmake --build build
	echo "" > build/rss_feeds_update
}

package() {
	id -u rss_subscriber &>/dev/null || sudo useradd --system -s /usr/bin/nologin -U rss_subscriber
        DESTDIR="$pkgdir" cmake --install build
        install -m644 -o rss_subscriber -g rss_subscriber -D build/rss_feeds_update ${pkgdir}/var/lib/rss_subscriber/rss_feeds_update
}
