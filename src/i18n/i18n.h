#ifndef BECTL_I18N_H
#define BECTL_I18N_H

#include <QString>
#include <QStringList>
#include <utility>

/**
 * Internacionalização simples (sem depender de lrelease em tempo de build).
 * Idiomas: en_US (padrão/fallback), pt_BR, es_ES.
 * Na arranque usa QLocale::system(); se não houver catálogo, cai em en_US.
 */
namespace I18n {

/** Detecta idioma do SO e carrega catálogo (fallback en_US). */
void initFromSystem();

/** Define idioma explicitamente: "en_US", "pt_BR", "es_ES". */
void setLocale(const QString &locale);

QString locale();
QStringList availableLocales();
QString localeDisplayName(const QString &locale);

/** Traduz chave (texto base em inglês). */
QString tr(const char *key);
QString tr(const QString &key);

/** Atalho: tr com argumentos %1, %2, ... */
template<typename... Args>
QString trf(const char *key, Args &&... args)
{
    QString s = tr(key);
    ((s = s.arg(std::forward<Args>(args))), ...);
    return s;
}

QString tr1(const char *key, const QString &a1);
QString tr2(const char *key, const QString &a1, const QString &a2);

} // namespace I18n

#endif
