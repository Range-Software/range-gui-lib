#ifndef RGL_APPLICATION_SETTINGS_H
#define RGL_APPLICATION_SETTINGS_H

#include <QSettings>
#include <QString>
#include <QLocale>

#include <rbl_version.h>
#include <rcl_http_proxy_settings.h>

class RApplicationSettings : public QSettings
{

    Q_OBJECT

    public:

        struct Style
        {
            //! Windows-dark style constant.
            static const QString WindowsDark;
            //! Fusion-dark style constant.
            static const QString FusionDark;
            //! Fusion-range style constant.
            static const QString FusionRange;
            //! Fusion style constant.
            static const QString Fusion;
        };

        struct Language
        {
            struct En
            {
                static const QString Code;
                static const QString Name;
            };
            struct Sk
            {
                static const QString Code;
                static const QString Name;
            };
        };

        enum Format
        {
            System = 0,
            Language,
            C,
            nFormats
        };

    protected:

        //! Cloud session file name without path.
        static const QString cloudSessionFileName;
        //! Log file name without path.
        static const QString logFileName;
        //! License file name without path.
        static const QString licenseFileName;
        //! Release notes file name without path.
        static const QString releaseNotesFileName;
        //! OpenSSL configuration file name without path.
        static const QString opensslCnfFileName;

        static const QString helpDirKey;
        static const QString styleKey;
        static const QString languageKey;
        static const QString formatKey;
        static const QString toolbarIconSizeKey;
        static const QString cloudRefreshTimeoutKey;
        static const QString cloudSyncDataDirectoryKey;
        static const QString cloudSyncDataCachePathKey;
        static const QString softwareSendUsageInfoKey;
        static const QString softwareCheckUpdatesKey;
        static const QString proxyTypeKey;
        static const QString proxyHostKey;
        static const QString proxyPortKey;
        static const QString proxyUserKey;
        static const QString proxyPasswordKey;
        static const QString userFullNameKey;
        static const QString userEmailKey;
        static const QString userTerritoryKey;
        static const QString opensslToolPathKey;
        static const QString opensslConfPathKey;

        //! Application path.
        QString applicationDirPath;
        //! Stored version.
        RVersion storedVersion;

    public:

        //! Constructor.
        explicit RApplicationSettings(QObject *parent = nullptr);

        //! Set application path.
        void setDirApplicationPath(const QString &applicationDirPath);

        //! Return cloud session file name.
        QString getCloudSessionFileName() const;

        //! Return log file name.
        QString getLogFileName() const;

        //! Return license file name.
        QString findLicenseFileName() const;

        //! Return release notes file name.
        QString findReleaseNotesFileName() const;

        //! Return openssl.cnf file name.
        QString findOpensslConfPath() const;

        //! Return resources/etc directory path.
        QString findEtcDir() const;

        //! Return help directory path.
        QString findHelpDir() const;

        //! Return pixmaps directory path.
        QString findPixmapsDir() const;

        //! Return const reference to stored version.
        const RVersion &getStoredVersion() const;

        //! Return supported image formats (extensions).
        static QStringList getSupportedImageFormats();

        //! Return supported video formats (extensions).
        static QStringList getSupportedVideoFormats();

        //! Return supported document formats (extensions).
        static QStringList getSupportedDocumentFormats();

        //! Return and construct directory path.
        static QString getDir(const QString &topDir, const QString &subDir);

        //! Return const reference to help directory.
        QString getHelpDir() const;

        //! Set new help directory.
        void setHelpDir(const QString &helpDir);

        //! Return style.
        QString getStyle() const;

        //! Set style.
        void setStyle(const QString &style);

        //! Return language code.
        QString getLanguageCode() const;

        //! Set language code.
        void setLanguageCode(const QString &languageCode);

        //! Return format.
        Format getFormat() const;

        //! Set newFormat.
        void setFormat(Format newFormat);

        //! Return toolbar icon size.
        int getToolbarIconSize() const;

        //! Set toolbar icon size.
        void setToolbarIconSize(int toolbarIconSize);

        //! Return cloud refresh timeout.
        uint getCloudRefreshTimeout() const;

        //! Set cloud refresh timeout.
        void setCloudRefreshTimeout(uint cloudRefreshTimeout);

        //! Return cloud sync data directory.
        bool getCloudSyncDataDirectory() const;

        //! Set cloud sync data directory.
        void setCloudSyncDataDirectory(bool cloudSyncDataDirectory);

        //! Return cloud sync cache path.
        QString getCloudSyncDataCachePath() const;

        //! Set cloud sync cache path.
        void setCloudSyncDataCachePath(const QString &cloudSyncDataCachePath);

        //! Return software send usage information.
        uint getSoftwareSendUsageInfo() const;

        //! Set software send usage information.
        void setSoftwareSendUsageInfo(bool cloudSendUsageInfo);

        //! Return whether to check for software updates.
        uint getSoftwareCheckUpdates() const;

        //! Set to check for software updates.
        void setSoftwareCheckUpdates(bool checkSoftwareUpdates);

        //! Return proxy settings.
        RHttpProxySettings getProxySettings() const;

        //! Set new proxy settings.
        void setProxySettings(const RHttpProxySettings &proxySettings);

        //! Return user full name.
        QString getUserFullName() const;

        //! Set user full name.
        void setUserFullName(const QString &userFullName);

        //! Return user email.
        QString getUserEmail() const;

        //! Set user email.
        void setUserEmail(const QString &userEmail);

        //! Return user territory.
        QString getUserTerritory() const;

        //! Set user territory.
        void setUserTerritory(const QString &userTerritory);

        //! Return path to OpenSSL tool.
        QString getOpensslToolPath() const;

        //! Set path to OpenSSL tool.
        void setOpensslToolPath(const QString &opensslToolPath);

        //! Return path to OpenSSL configuration file.
        QString getOpensslConfPath() const;

        //! Set path to OpenSSL configuration file.
        void setOpensslConfPath(const QString &opensslConfPath);

        //! Return users home directory path.
        static QString getHomeDir();

        //! Return users application home directory path.
        static QString getAppHomeDir();

        //! Return users application data directory path.
        static QString getAppDataDir(const QString &subDir);

        //! Return users data directory path.
        static QString getDataDir();

        //! Return users cert directory path.
        static QString getCertDir();

        //! Return users document directory path.
        static QString getDocDir();

        //! Return users etc directory path.
        static QString getEtcDir();

        //! Return users log directory path.
        static QString getLogDir();

        //! Return users temporary directory path.
        static QString getTmpDir();

        //! Return users session directory path.
        static QString getSessionDir();

        //! Return list of styles.
        static QStringList getStyles();

        //! Return list of language codes.
        static QStringList getLanguageCodes();

        //! Return list of language names.
        static QStringList getLanguageNames();

        //! Convert language name to code.
        static QString languageNameToCode(const QString &languageName);

        //! Convert language code to QLocale::Language.
        static QLocale::Language languageCodeToLanguage(const QString &languageCode);

        //! Convert format to code.
        static Format codeToFormat(const QString &code);

        //! Convert format to code.
        static QString formatToCode(Format format);

        //! Convert format to name.
        static QString formatToName(Format format);

        //! Return default style.
        static QString getDefaultStyle();

        //! Return default language code.
        static QString getDefaultLanguageCode();

        //! Return default format.
        static Format getDefaultFormat();

        //! Return default toolbar icon size.
        static int getDefaultToolbarIconSize();

        //! Return default cloud refresh timeout.
        static uint getDefaultCloudRefreshTimeout();

        //! Return default cloud sync data directory.
        static bool getDefaultCloudSyncDataDirectory();

        //! Return default cloud sync data cache path.
        static QString getDefaultCloudSyncDataCachePath();

        //! Return default cloud send usage information.
        static bool getDefaultSoftwareSendUsageInfo();

        //! Return default scheck for software updates.
        static bool getDefaultSoftwareCheckUpdates();

        //! Return default user territory.
        static const QString getDefaultUserTerritory();

        //! Return default private key path.
        static const QString getDefaultPrivateKeyPath();

        //! Return default csr path.
        static const QString getDefaultCsrPath();

        //! Return default certificate path.
        static const QString getDefaultCertificatePath();

        //! Return default CA certificate path.
        static const QString getDefaultCaCertificatePath();

        //! Return default OpenSSL tool path.
        static const QString getDefaultOpensslToolPath();

        //! Return default background color for style.
        static QColor getDefaultBackgroundColor(const QString &style);

    signals:

        //! Style has changed.
        void styleChanged(const QString &style);

        //! Language has changed.
        void languageChanged(const QString &language);

        //! Format has changed.
        void formatChanged(const RApplicationSettings::Format &format);

        //! Toolbar icon size has changed.
        void toolbarIconSizeChanged(int toolbarIconSize);

        //! Proxy settings has changed.
        void proxySettingsChanged(RHttpProxySettings proxySettings);

        //! Cloud refresh timeout has changed.
        void cloudRefreshTimeoutChanged(uint cloudRefreshTimeout);

        //! Cloud sync data directory has changed.
        void cloudSyncDataDirectoryChanged(bool cloudSyncDataDirectory);

        //! Cloud sync data directory has changed.
        void cloudSyncDataCachePathChanged(const QString &cloudSyncDataCachePath);

        //! Cloud send usage information has changed.
        void cloudSendUsageInfoChanged(bool cloudSenfUsageInfoChanged);

};

#endif /* RGL_APPLICATION_SETTINGS_H */
