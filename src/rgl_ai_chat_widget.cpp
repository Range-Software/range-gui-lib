#include <rgl_ai_chat_widget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLocale>
#include <QMap>
#include <QScrollBar>
#include <QElapsedTimer>
#include <QTimer>

#include <cmath>
#include <memory>

#include <rai_agent.h>

#include <rgl_message_box.h>
#include <rgl_text_browser.h>

RAiChatWidget::~RAiChatWidget()
{
    this->agentThread->quit();
    this->agentThread->wait();
}

void RAiChatWidget::onAgentSelected(const QString &name)
{
    if (name.isEmpty()) return;
    RAgentSettings agentSettings = this->agentSettingsManager->findSettings(name);
    agentSettings.setProxyHost(this->appSettings->getProxySettings().getHost());
    agentSettings.setProxyPort(this->appSettings->getProxySettings().getPort());
    agentSettings.setProxyUser(this->appSettings->getProxySettings().getUser());
    agentSettings.setProxyPassword(this->appSettings->getProxySettings().getPassword());
    agentSettings.setSystemPrompt(this->buildPrompt(this->languageCombo->currentText()));
    RAgent *a = this->agent;
    QMetaObject::invokeMethod(a, [a, agentSettings]() {
        a->setSettings(agentSettings);
    }, Qt::QueuedConnection);
}

void RAiChatWidget::applyCurrentAgent()
{
    const QString name = this->aiAgentCombo->currentText();
    if (!name.isEmpty())
    {
        this->onAgentSelected(name);
    }
}

RAiChatWidget::RAiChatWidget(RAgentSettingsManager *aiAgentSettingsManager,
                             const RApplicationSettings *applicationSettings,
                             const QString &initialAgent,
                             const QString &initialLanguage,
                             QWidget *parent)
    : QWidget{parent}
    , agentSettingsManager{aiAgentSettingsManager}
    , appSettings{applicationSettings}
{
    RAgent *agentPtr = new RAgent(RAgentSettings(RAgentSettings::Type::None), nullptr);
    this->agentThread = new QThread(this);
    QObject::connect(this->agentThread, &QThread::finished, agentPtr, &QObject::deleteLater);
    agentPtr->moveToThread(this->agentThread);
    this->agentThread->start();
    this->agent = agentPtr;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    this->aiAgentCombo = new QComboBox;
    for (const QString &aiAgentName : aiAgentSettingsManager->getNames())
    {
        this->aiAgentCombo->addItem(aiAgentName);
    }
    if (!initialAgent.isEmpty())
    {
        int idx = this->aiAgentCombo->findText(initialAgent);
        if (idx >= 0)
        {
            this->aiAgentCombo->setCurrentIndex(idx);
        }
    }
    formLayout->addRow(tr("AI agent"), this->aiAgentCombo);

    this->languageCombo = new QComboBox;
    {
        QMap<QString, QLocale::Language> languageMap;
        const QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyTerritory);
        for (const QLocale &locale : locales)
        {
            if (locale.language() == QLocale::C)
            {
                continue;
            }
            const QString name = QLocale::languageToString(locale.language());
            if (!languageMap.contains(name))
            {
                languageMap.insert(name, locale.language());
            }
        }
        for (auto it = languageMap.cbegin(); it != languageMap.cend(); ++it)
        {
            this->languageCombo->addItem(it.key(), static_cast<int>(it.value()));
        }

        int selectedIndex = initialLanguage.isEmpty() ? -1 : this->languageCombo->findText(initialLanguage);
        if (selectedIndex < 0)
        {
            selectedIndex = this->languageCombo->findData(static_cast<int>(QLocale::system().language()));
        }
        if (selectedIndex < 0)
        {
            selectedIndex = this->languageCombo->findData(static_cast<int>(QLocale::English));
        }
        if (selectedIndex >= 0)
        {
            this->languageCombo->setCurrentIndex(selectedIndex);
        }
    }
    formLayout->addRow(tr("Language"), this->languageCombo);

    RTextBrowser *chatHistory = new RTextBrowser(true);
    mainLayout->addWidget(chatHistory);

    QHBoxLayout *queryLayout = new QHBoxLayout;
    mainLayout->addLayout(queryLayout);

    QLineEdit *queryEdit = new QLineEdit;
    queryEdit->setPlaceholderText(tr("Ask your question an press Enter"));
    queryEdit->setClearButtonEnabled(true);
    queryLayout->addWidget(queryEdit);

    QPushButton *queryButton = new QPushButton(tr("Send"));
    queryLayout->addWidget(queryButton);

    auto appendMarkdown = [chatHistory](const QString &markdown)
    {
        QTextCursor cursor(chatHistory->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertBlock();
        cursor.insertMarkdown(markdown);
        chatHistory->verticalScrollBar()->setValue(chatHistory->verticalScrollBar()->maximum());
    };

    auto waitingCursor = std::make_shared<QTextCursor>();
    auto waitingAnchor = std::make_shared<int>(-1);
    auto fadePhase = std::make_shared<double>(0.0);
    auto elapsedTimer = std::make_shared<QElapsedTimer>();
    QTimer *waitingTimer = new QTimer(chatHistory);
    waitingTimer->setInterval(50);

    auto showWaiting = [chatHistory, waitingCursor, waitingAnchor, waitingTimer, fadePhase, elapsedTimer]()
    {
        QTextCursor cursor(chatHistory->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertBlock();
        *waitingAnchor = cursor.position();
        cursor.insertText(tr("Waiting for an answer ... "));
        QTextCursor sel(chatHistory->document());
        sel.setPosition(*waitingAnchor);
        sel.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        *waitingCursor = sel;
        *fadePhase = 0.0;
        elapsedTimer->start();
        chatHistory->verticalScrollBar()->setValue(chatHistory->verticalScrollBar()->maximum());
        waitingTimer->start();
    };

    auto hideWaiting = [waitingCursor, waitingAnchor, waitingTimer]()
    {
        waitingTimer->stop();
        if (*waitingAnchor >= 0)
        {
            QTextCursor sel(waitingCursor->document());
            sel.setPosition(*waitingAnchor - 1);
            sel.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            sel.removeSelectedText();
            *waitingCursor = QTextCursor();
            *waitingAnchor = -1;
        }
    };

    QObject::connect(waitingTimer, &QTimer::timeout, [chatHistory, waitingCursor, waitingAnchor, fadePhase, elapsedTimer]()
    {
        if (*waitingAnchor < 0) return;
        const qint64 secs = elapsedTimer->elapsed() / 1000;
        const QString timeStr = secs < 60
            ? QStringLiteral("(%1s)").arg(secs)
            : QStringLiteral("(%1m %2s)").arg(secs / 60).arg(secs % 60);
        waitingCursor->insertText(tr("Waiting for an answer ... ") + timeStr);
        QTextCursor sel(chatHistory->document());
        sel.setPosition(*waitingAnchor);
        sel.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        *waitingCursor = sel;
        *fadePhase += 0.08;
        const double t = (std::sin(*fadePhase) + 1.0) / 2.0;
        const QColor text = chatHistory->palette().color(QPalette::Text);
        const QColor base = chatHistory->palette().color(QPalette::Base);
        const QColor blended(
            qRound(base.red()   + t * (text.red()   - base.red())),
            qRound(base.green() + t * (text.green() - base.green())),
            qRound(base.blue()  + t * (text.blue()  - base.blue()))
        );
        QTextCharFormat fmt;
        fmt.setForeground(blended);
        waitingCursor->mergeCharFormat(fmt);
    });

    auto isWaiting = std::make_shared<bool>(false);

    auto setWaiting = [queryEdit, queryButton, isWaiting](bool waiting)
    {
        *isWaiting = waiting;
        queryButton->setText(waiting ? tr("Cancel") : tr("Send"));
        queryEdit->setEnabled(!waiting);
    };

    auto onQuery = [this, chatHistory, queryEdit, agentPtr, appendMarkdown, showWaiting, setWaiting]()
    {
        QString text = queryEdit->text();
        if (text.isEmpty()) return;
        appendMarkdown(QStringLiteral("---\n**") + tr("Me") + QStringLiteral(":** *") + text + QStringLiteral("*"));
        queryEdit->clear();
        showWaiting();
        QMetaObject::invokeMethod(agentPtr, [agentPtr, text]() {
            agentPtr->chat(text);
        }, Qt::QueuedConnection);
        // Defer setWaiting to avoid the Enter key event activating the button after focus shifts
        QTimer::singleShot(0, this, [setWaiting]() { setWaiting(true); });
    };

    QObject::connect(this->aiAgentCombo, &QComboBox::currentTextChanged, this, [this](const QString &name) {
        this->onAgentSelected(name);
        emit this->aiAgentNameChanged(name);
    });

    QObject::connect(this->languageCombo, &QComboBox::currentTextChanged, this, [this](const QString &language) {
        QString prompt = this->buildPrompt(language);
        RAgent *a = this->agent;
        QMetaObject::invokeMethod(a, [a, prompt]() {
            a->setSystemPrompt(prompt);
        }, Qt::QueuedConnection);
        emit this->languageNameChanged(language);
    });

    QObject::connect(aiAgentSettingsManager, &RAgentSettingsManager::settingsAdded, this->aiAgentCombo, [this](const QString &name) {
        this->aiAgentCombo->addItem(name);
    });
    QObject::connect(aiAgentSettingsManager, &RAgentSettingsManager::settingsRenamed, this->aiAgentCombo, [this](const QString &oldName, const QString &newName) {
        int idx = this->aiAgentCombo->findText(oldName);
        if (idx >= 0)
        {
            this->aiAgentCombo->setItemText(idx, newName);
        }
    });
    QObject::connect(aiAgentSettingsManager, &RAgentSettingsManager::settingsRemoved, this->aiAgentCombo, [this](const QString &name) {
        int idx = this->aiAgentCombo->findText(name);
        if (idx >= 0)
        {
            this->aiAgentCombo->removeItem(idx);
        }
    });

    QObject::connect(queryEdit, &QLineEdit::returnPressed, onQuery);
    QObject::connect(queryButton, &QPushButton::clicked, this, [agentPtr, isWaiting, onQuery, hideWaiting, setWaiting]()
    {
        if (*isWaiting)
        {
            hideWaiting();
            setWaiting(false);
            QMetaObject::invokeMethod(agentPtr, [agentPtr]() {
                agentPtr->cancelRequest();
            }, Qt::QueuedConnection);
        }
        else
        {
            onQuery();
        }
    });

    QObject::connect(agentPtr, &RAgent::responseReceived, this, [appendMarkdown, hideWaiting, setWaiting, isWaiting](const QString &response) {
        if (!*isWaiting) return;
        hideWaiting();
        setWaiting(false);
        appendMarkdown(QStringLiteral("**") + tr("AI") + QStringLiteral(":**\n\n") + response);
    });

    QObject::connect(agentPtr, &RAgent::errorOccurred, this, [appendMarkdown, hideWaiting, setWaiting, isWaiting](const QString &error) {
        if (!*isWaiting) return;
        hideWaiting();
        setWaiting(false);
        appendMarkdown(QStringLiteral("**Error:** ") + error);
    });

    QObject::connect(agentPtr, &RAgent::requestCancelled, this, [hideWaiting, setWaiting]() {
        hideWaiting();
        setWaiting(false);
    });
}
