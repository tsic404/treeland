// Copyright (C) 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#pragma once

#include <DConfig>

#include <QLocale>
#include <QObject>
#include <QQmlEngine>
#include <QString>

/**
 * Region format of the user the lock screen is shown for.
 *
 * The greeter runs as the "dde" user, so the default DConfig backend would only
 * answer with dde's own values. The settings are kept per uid by the DSG config
 * daemon, so read them through UserConfigBackend for the shown user's uid.
 */
class RegionFormatProvider : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString shortTimeFormat READ shortTimeFormat NOTIFY formatChanged)
    Q_PROPERTY(QString shortDateFormat READ shortDateFormat NOTIFY formatChanged)
    Q_PROPERTY(QLocale locale READ locale NOTIFY formatChanged)

public:
    explicit RegionFormatProvider(QObject *parent = nullptr);
    ~RegionFormatProvider() override;

    [[nodiscard]] QString shortTimeFormat() const noexcept
    {
        return m_shortTimeFormat;
    }
    [[nodiscard]] QString shortDateFormat() const noexcept
    {
        return m_shortDateFormat;
    }
    [[nodiscard]] QLocale locale() const noexcept
    {
        return m_locale;
    }

Q_SIGNALS:
    void formatChanged();

private Q_SLOTS:
    void refresh();

private:
    void setUser(uint uid);
    void updateFormats(QString timeFormat, QString dateFormat, QLocale locale);

    DTK_CORE_NAMESPACE::DConfig *m_config = nullptr;
    uint m_uid = 0;
    QString m_shortTimeFormat;
    QString m_shortDateFormat;
    QLocale m_locale;
};
