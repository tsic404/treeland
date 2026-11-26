// Copyright (C) 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "regionformatprovider.h"

#include "common/treelandlogging.h"
#include "core/waylandserver.h"
#include "dconfig/userconfigbackend.h"
#include "greeter/user.h"
#include "greeter/usermodel.h"

#include <utility>

namespace {
constexpr auto regionFormatResource = "org.deepin.region-format";

bool isRegionFormatKey(const QString &key)
{
    return key == QLatin1String("shortTimeFormat") || key == QLatin1String("shortDateFormat")
        || key == QLatin1String("localeName");
}

// The daemon answers with the C locale for an empty or unknown name.
QLocale resolveLocale(const QString &name, const QLocale &fallback)
{
    if (!name.isEmpty()) {
        const QLocale candidate(name);
        if (candidate.name() != QLatin1String("C"))
            return candidate;
    }

    return fallback.name() == QLatin1String("C") ? QLocale() : fallback;
}
} // namespace

RegionFormatProvider::RegionFormatProvider(QObject *parent)
    : QObject(parent)
{
    connect(WaylandServer::instance()->userModel(),
            &UserModel::currentUserNameChanged,
            this,
            &RegionFormatProvider::refresh);
    refresh();
}

RegionFormatProvider::~RegionFormatProvider() = default;

void RegionFormatProvider::refresh()
{
    auto *userModel = WaylandServer::instance()->userModel();
    const auto user = userModel->getUser(userModel->currentUserName());
    if (!user) {
        updateFormats({}, {}, QLocale());
        return;
    }

    setUser(static_cast<uint>(user->UID()));

    const auto timeFormat = m_config->value(QStringLiteral("shortTimeFormat")).toString();
    const auto dateFormat = m_config->value(QStringLiteral("shortDateFormat")).toString();
    const auto localeName = m_config->value(QStringLiteral("localeName")).toString();
    qCDebug(lcTlGreeter) << "region format of user" << userModel->currentUserName() << timeFormat
                         << dateFormat << localeName;
    updateFormats(timeFormat, dateFormat, resolveLocale(localeName, user->locale()));
}

void RegionFormatProvider::setUser(uint uid)
{
    if (m_config && uid == m_uid)
        return;

    delete m_config;
    m_config = nullptr;
    m_uid = uid;

    const auto resource = QString::fromLatin1(regionFormatResource);
    auto *backend = new UserConfigBackend(uid, resource);
    m_config = DTK_CORE_NAMESPACE::DConfig::createGeneric(backend, resource, QString(), this);
    if (!m_config->isValid()) {
        qCWarning(lcTlGreeter) << "Failed to read region format of uid" << uid;
        return;
    }

    connect(m_config, &DTK_CORE_NAMESPACE::DConfig::valueChanged, this, [this](const QString &key) {
        if (isRegionFormatKey(key))
            refresh();
    });
    // forwards the daemon's change notifications to the config above
    backend->subscribe(m_config);
}

void RegionFormatProvider::updateFormats(QString timeFormat, QString dateFormat, QLocale locale)
{
    if (timeFormat == m_shortTimeFormat && dateFormat == m_shortDateFormat && locale == m_locale)
        return;

    m_shortTimeFormat = std::move(timeFormat);
    m_shortDateFormat = std::move(dateFormat);
    m_locale = locale;
    Q_EMIT formatChanged();
}
