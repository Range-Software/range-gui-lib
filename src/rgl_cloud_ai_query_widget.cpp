#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLocale>
#include <QMap>
#include <QPushButton>
#include <QScrollBar>
#include <QTimer>

#include <cmath>

#include <QCoreApplication>

#include <rbl_logger.h>

#include <rcl_ai_query.h>
#include <rcl_cloud_ai_query_request.h>

#include "rgl_cloud_ai_query_widget.h"
#include "rgl_text_browser.h"

const int RCloudAiQueryWidget::pollInterval = 2000;
const int RCloudAiQueryWidget::pollTimeout = 300000;
const int RCloudAiQueryWidget::waitingInterval = 50;

RCloudAiQueryWidget::RCloudAiQueryWidget(RCloudConnectionHandler *connectionHandler,
                                     RApplicationSettings *applicationSettings,
                                     QWidget *parent)
    : QWidget{parent}
    , applicationSettings{applicationSettings}
    , discardedResponses{0}
    , waiting{false}
    , waitingAnchor{-1}
    , waitingFadePhase{0.0}
{
    this->cloudClient = connectionHandler->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    this->languageCombo = new QComboBox;
    this->populateLanguages();
    formLayout->addRow(tr("Language"),this->languageCombo);

    this->queryHistory = new RTextBrowser(true);
    mainLayout->addWidget(this->queryHistory);

    QHBoxLayout *queryLayout = new QHBoxLayout;
    mainLayout->addLayout(queryLayout);

    this->queryEdit = new QLineEdit;
    this->queryEdit->setPlaceholderText(tr("Ask your question an press Enter"));
    this->queryEdit->setClearButtonEnabled(true);
    queryLayout->addWidget(this->queryEdit);

    this->queryButton = new QPushButton(tr("Send"));
    this->queryButton->setAutoDefault(true);
    this->queryButton->setDefault(true);
    queryLayout->addWidget(this->queryButton);

    // Result is requested only once per timeout to avoid overlapping requests.
    this->pollTimer = new QTimer(this);
    this->pollTimer->setInterval(RCloudAiQueryWidget::pollInterval);
    this->pollTimer->setSingleShot(true);

    this->waitingTimer = new QTimer(this);
    this->waitingTimer->setInterval(RCloudAiQueryWidget::waitingInterval);

    QObject::connect(this->queryEdit,&QLineEdit::returnPressed,this,&RCloudAiQueryWidget::submitQuery);
    QObject::connect(this->queryButton,&QPushButton::clicked,this,&RCloudAiQueryWidget::onQueryButtonClicked);
    QObject::connect(this->pollTimer,&QTimer::timeout,this,&RCloudAiQueryWidget::onPollTimeout);
    QObject::connect(this->waitingTimer,&QTimer::timeout,this,&RCloudAiQueryWidget::onWaitingTimeout);

    QObject::connect(this->cloudClient,&RCloudClient::failed,this,&RCloudAiQueryWidget::onClientFailed);
    QObject::connect(this->cloudClient,&RCloudClient::aiQueryResponseAvailable,this,&RCloudAiQueryWidget::onAiQueryResponseAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::aiQueryResultAvailable,this,&RCloudAiQueryWidget::onAiQueryResultAvailable);
}

void RCloudAiQueryWidget::populateLanguages()
{
    QMap<QString,QLocale::Language> languageMap;
    const QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage,QLocale::AnyScript,QLocale::AnyTerritory);
    for (const QLocale &locale : locales)
    {
        if (locale.language() == QLocale::C)
        {
            continue;
        }
        const QString name = QLocale::languageToString(locale.language());
        if (!languageMap.contains(name))
        {
            languageMap.insert(name,locale.language());
        }
    }
    for (auto it = languageMap.cbegin(); it != languageMap.cend(); ++it)
    {
        this->languageCombo->addItem(it.key(),static_cast<int>(it.value()));
    }

    int selectedIndex = this->languageCombo->findData(static_cast<int>(RApplicationSettings::languageCodeToLanguage(this->applicationSettings->getLanguageCode())));
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

void RCloudAiQueryWidget::submitQuery()
{
    if (this->waiting)
    {
        return;
    }

    RAIQuery aiQuery;
    aiQuery.setQuestion(this->queryEdit->text());
    aiQuery.setLanguage(this->languageCombo->currentText());

    if (aiQuery.isEmpty())
    {
        return;
    }

    RCloudAIQueryRequest aiQueryRequest;
    aiQueryRequest.setApplication(QCoreApplication::applicationName());
    aiQueryRequest.setQuery(aiQuery);

    this->appendMarkdown(QStringLiteral("---\n**") + tr("Me") + QStringLiteral(":** *") + aiQuery.getQuestion() + QStringLiteral("*"));
    this->queryEdit->clear();
    this->showWaitingMessage();

    RLogger::info("Submitting AI query to Cloud.\n");
    this->cloudClient->requestAIQuery(aiQueryRequest);

    // Defer setWaiting to avoid the Enter key event activating the button after focus shifts
    QTimer::singleShot(0,this,[this]() { this->setWaiting(true); });
}

void RCloudAiQueryWidget::cancelQuery()
{
    RLogger::info("AI query was canceled.\n");
    if (this->queryId.isNull())
    {
        // Query was not acknowledged yet. Its response has to be discarded.
        this->discardedResponses++;
    }
    this->pollTimer->stop();
    this->queryId = QUuid();
    this->hideWaitingMessage();
    this->setWaiting(false);
}

void RCloudAiQueryWidget::appendMarkdown(const QString &markdown)
{
    QTextCursor cursor(this->queryHistory->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.insertMarkdown(markdown);
    this->queryHistory->verticalScrollBar()->setValue(this->queryHistory->verticalScrollBar()->maximum());
}

void RCloudAiQueryWidget::setWaiting(bool waiting)
{
    this->waiting = waiting;
    this->queryButton->setText(waiting ? tr("Cancel") : tr("Send"));
    this->queryEdit->setEnabled(!waiting);
}

void RCloudAiQueryWidget::showWaitingMessage()
{
    QTextCursor cursor(this->queryHistory->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    this->waitingAnchor = cursor.position();
    cursor.insertText(tr("Waiting for an answer ... "));

    QTextCursor selection(this->queryHistory->document());
    selection.setPosition(this->waitingAnchor);
    selection.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
    this->waitingCursor = selection;
    this->waitingFadePhase = 0.0;
    this->waitingElapsedTimer.start();
    this->queryHistory->verticalScrollBar()->setValue(this->queryHistory->verticalScrollBar()->maximum());
    this->waitingTimer->start();
}

void RCloudAiQueryWidget::hideWaitingMessage()
{
    this->waitingTimer->stop();
    if (this->waitingAnchor >= 0)
    {
        QTextCursor selection(this->queryHistory->document());
        selection.setPosition(this->waitingAnchor - 1);
        selection.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
        selection.removeSelectedText();
        this->waitingCursor = QTextCursor();
        this->waitingAnchor = -1;
    }
}

void RCloudAiQueryWidget::finishQuery(const QString &responseMessage)
{
    this->pollTimer->stop();
    this->queryId = QUuid();
    this->hideWaitingMessage();
    this->setWaiting(false);
    this->appendMarkdown(QStringLiteral("**") + tr("AI") + QStringLiteral(":**\n\n") + responseMessage);
}

void RCloudAiQueryWidget::failQuery(const QString &errorMessage)
{
    RLogger::error("AI query has failed. %s\n",errorMessage.toUtf8().constData());
    this->pollTimer->stop();
    this->queryId = QUuid();
    this->hideWaitingMessage();
    this->setWaiting(false);
    this->appendMarkdown(QStringLiteral("**") + tr("Error") + QStringLiteral(":** ") + errorMessage);
}

void RCloudAiQueryWidget::onClientFailed()
{
    if (!this->waiting)
    {
        return;
    }
    this->failQuery(tr("Cloud request has failed."));
}

void RCloudAiQueryWidget::onAiQueryResponseAvailable(RCloudAIQueryResponse aiQueryResponse)
{
    if (this->discardedResponses > 0)
    {
        this->discardedResponses--;
        return;
    }

    if (!this->waiting)
    {
        return;
    }

    if (aiQueryResponse.getStatus() == RCloudAIQueryResponse::Completed)
    {
        this->finishQuery(aiQueryResponse.getResponseMessage());
        return;
    }

    if (aiQueryResponse.getStatus() != RCloudAIQueryResponse::Pending || aiQueryResponse.getId().isNull())
    {
        this->failQuery(tr("Cloud has not accepted the query."));
        return;
    }

    RLogger::info("AI query \'%s\' is being processed.\n",aiQueryResponse.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    this->queryId = aiQueryResponse.getId();
    this->pollTimer->start();
}

void RCloudAiQueryWidget::onAiQueryResultAvailable(RCloudAIQueryResponse aiQueryResponse)
{
    if (!this->waiting || this->queryId.isNull() || aiQueryResponse.getId() != this->queryId)
    {
        return;
    }

    if (aiQueryResponse.getStatus() == RCloudAIQueryResponse::Completed)
    {
        this->finishQuery(aiQueryResponse.getResponseMessage());
    }
    else if (aiQueryResponse.getStatus() == RCloudAIQueryResponse::Pending)
    {
        this->pollTimer->start();
    }
    else
    {
        this->failQuery(tr("Cloud does not know the submitted query."));
    }
}

void RCloudAiQueryWidget::onQueryButtonClicked()
{
    if (this->waiting)
    {
        this->cancelQuery();
    }
    else
    {
        this->submitQuery();
    }
}

void RCloudAiQueryWidget::onPollTimeout()
{
    if (!this->waiting || this->queryId.isNull())
    {
        this->pollTimer->stop();
        return;
    }

    if (this->waitingElapsedTimer.hasExpired(RCloudAiQueryWidget::pollTimeout))
    {
        this->failQuery(tr("Timed out while waiting for an answer."));
        return;
    }

    this->cloudClient->requestAIQueryResult(this->queryId);
}

void RCloudAiQueryWidget::onWaitingTimeout()
{
    if (this->waitingAnchor < 0)
    {
        return;
    }

    const qint64 seconds = this->waitingElapsedTimer.elapsed() / 1000;
    const QString timeStr = seconds < 60
        ? QStringLiteral("(%1s)").arg(seconds)
        : QStringLiteral("(%1m %2s)").arg(seconds / 60).arg(seconds % 60);
    this->waitingCursor.insertText(tr("Waiting for an answer ... ") + timeStr);

    QTextCursor selection(this->queryHistory->document());
    selection.setPosition(this->waitingAnchor);
    selection.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
    this->waitingCursor = selection;

    this->waitingFadePhase += 0.08;
    const double t = (std::sin(this->waitingFadePhase) + 1.0) / 2.0;
    const QColor text = this->queryHistory->palette().color(QPalette::Text);
    const QColor base = this->queryHistory->palette().color(QPalette::Base);
    const QColor blended(
        qRound(base.red()   + t * (text.red()   - base.red())),
        qRound(base.green() + t * (text.green() - base.green())),
        qRound(base.blue()  + t * (text.blue()  - base.blue()))
    );
    QTextCharFormat format;
    format.setForeground(blended);
    this->waitingCursor.mergeCharFormat(format);
}
