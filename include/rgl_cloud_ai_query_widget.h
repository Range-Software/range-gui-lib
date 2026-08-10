#ifndef RGL_CLOUD_AI_QUERY_WIDGET_H
#define RGL_CLOUD_AI_QUERY_WIDGET_H

#include <rcl_cloud_ai_query_response.h>
#include <rcl_file_info.h>
#include <rcl_cloud_client.h>

#include <QElapsedTimer>
#include <QTextCursor>
#include <QUuid>
#include <QWidget>

#include "rgl_application_settings.h"
#include "rgl_cloud_connection_handler.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QTimer;
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

    public:

        //! Constructor.
        explicit RCloudAiQueryWidget(RCloudConnectionHandler *connectionHandler,
                                   RApplicationSettings *applicationSettings,
                                   QWidget *parent = nullptr);

    protected:

        //! Populate language combo box with all available languages.
        void populateLanguages();

        //! Submit AI query.
        void submitQuery();

        //! Stop waiting for an answer and forget the submitted query.
        void cancelQuery();

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
