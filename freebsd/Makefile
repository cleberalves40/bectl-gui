PORTNAME=	bectl-gui
DISTVERSION=	1.0.0
CATEGORIES=	sysutils

MAINTAINER=	cleberalves40@github.com
COMMENT=	Qt6 graphical front-end for FreeBSD bectl(8)
WWW=		https://github.com/cleberalves40/bectl-gui

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/LICENSE

RUN_DEPENDS=	qt-sudo:security/qt-sudo

USES=		cmake compiler:c++17-lang qt:6
USE_QT=		base tools:build
USE_GITHUB=	yes
GH_ACCOUNT=	cleberalves40

PLIST_FILES=	bin/bectl_gui \
		share/applications/bectl_gui.desktop

.include <bsd.port.mk>
