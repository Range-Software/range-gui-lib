#ifndef RGL_CLOUD_AI_QUERY_WIDGET_H
#define RGL_CLOUD_AI_QUERY_WIDGET_H

#include <rcl_cloud_ai_query_response.h>
#include <rcl_file_info.h>
#include <rcl_cloud_client.h>

#include <QElapsedTimer>
#include <QList>
#include <QPair>
#include <QTextCursor>
#include <QUuid>
#include <QWidget>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QTimer;
class RAIQuery;
class RTextBrowser;

class RCloudAiQueryWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Time in milliseconds between two AI query result requests.
        static const int pollInterval;
        //! Maximum time in milliseconds to wait for an AI query result.
        static const int pollTimeout;
        //! Time in milliseconds between two waiting message updates.
        static const int waitingInterval;
        //! Maximum number of previous questions and answers kept as a conversation context.
        static const int maxHistoryTurns;

        RApplicationSettings *applicationSettings;

        RCloudClient *cloudClient;

        //! Response language selector.
        QComboBox *languageCombo;
        //! Query history.
        RTextBrowser *queryHistory;
        //! Query input.
        QLineEdit *queryEdit;
        //! Send / cancel button.
        QPushButton *queryButton;

        //! Timer requesting AI query result.
        QTimer *pollTimer;
        //! Timer updating waiting message.
        QTimer *waitingTimer;

        //! Id of the submitted AI query.
        QUuid queryId;
        //! Number of responses belonging to canceled queries which are still to be received.
        int discardedResponses;
        //! Waiting for an answer.
        bool waiting;
        //! Time elapsed since the query was submitted.
        QElapsedTimer waitingElapsedTimer;
        //! Cursor selecting the waiting message.
        QTextCursor waitingCursor;
        //! Position of the waiting message.
        int waitingAnchor;
        //! Waiting message fade phase.
        double waitingFadePhase;

        //! Previous questions and their answers sent as a context with every query.
        QList<QPair<QString,QString>> conversationHistory;
        //! Question which is being answered.
        QString pendingQuestion;
        //! Information about the user was already sent within this conversation.
        bool userInfoSent;

    public:

        //! Constructor.
        explicit RCloudAiQueryWidget(RCloudConnectionHandler *connectionHandler,
                                   RApplicationSettings *applicationSettings,
                                   QWidget *parent = nullptr);

        //! Stop waiting for an answer and forget the submitted query.
        //! Nothing is done if no answer is being waited for.
        void cancelQuery();

    protected:

        //! Populate language combo box with all available languages.
        void populateLanguages();

        //! Submit AI query.
        void submitQuery();

        //! Fill in optional fields of the query which is about to be submitted.
        //! Default implementation does nothing.
        virtual void buildQuery(RAIQuery &aiQuery);

        //! Build conversation context out of previous questions and answers.
        QString buildContext() const;

        //! Build information about the user asking the question.
        //! Empty string is returned if no such information is available.
        QString buildUserInfo() const;

        //! Return true if there are previous questions and answers.
        bool hasHistory() const;

        //! Forget all previous questions and answers.
        void clearHistory();

        //! Append markdown formatted text to the query history.
        void appendMarkdown(const QString &markdown);

        //! Set waiting for an answer state.
        void setWaiting(bool waiting);

        //! Insert waiting message at the end of the query history.
        void showWaitingMessage();

        //! Remove waiting message from the query history.
        void hideWaitingMessage();

        //! Finish waiting for an answer and display given message.
        void finishQuery(const QString &responseMessage);

        //! Finish waiting for an answer and display given error message.
        void failQuery(const QString &errorMessage);

    protected slots:

        //! Client has failed.
        void onClientFailed();

        //! AI query was accepted by the Cloud.
        void onAiQueryResponseAvailable(RCloudAIQueryResponse aiQueryResponse);

        //! AI query result was received from the Cloud.
        void onAiQueryResultAvailable(RCloudAIQueryResponse aiQueryResponse);

        //! Query button was clicked.
        void onQueryButtonClicked();

        //! AI query result is to be requested.
        void onPollTimeout();

        //! Waiting message is to be updated.
        void onWaitingTimeout();

};

#endif // RGL_CLOUD_AI_QUERY_WIDGET_H
