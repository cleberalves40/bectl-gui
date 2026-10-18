PORTNAME=	bectl-gui
DISTVERSIONPREFIX=	v
DISTVERSION=	1.1.2
CATEGORIES=	sysutils

MAINTAINER=	cleber@bsd.com.br
COMMENT=	Qt6 graphical front-end for FreeBSD bectl(8)
WWW=		https://github.com/cleberalves40/bectl-gui

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/LICENSE

RUN_DEPENDS=	qt-sudo:security/qt-sudo

USES=		cmake compiler:c++17-lang gl qt:6
USE_GL=		gl opengl
USE_QT=		base
USE_GITHUB=	yes
GH_ACCOUNT=	cleberalves40

PLIST_FILES=	bin/bectl_gui \
		share/applications/bectl_gui.desktop

.include <bsd.port.mk>
