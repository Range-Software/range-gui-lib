## Version 1.1.0

### Improvements

#### Cloud AI query

- `RCloudAiQueryWidget`: new widget asking questions answered by an AI service through
  Range Cloud. The question is submitted with `RCloudClient::requestAIQuery()` and the
  answer is fetched with `RCloudClient::requestAIQueryResult()` repeated until the query
  is completed. The hosting dialog is provided by the application
- Unlike the AI chat widget no AI agent is selected and no file upload is supported.
  Only the response language can be chosen, defaulting to the application language
- Questions are submitted by pressing Enter or the default "Send" button and are
  displayed together with the answers as Markdown in a query history. An answer is
  awaited with an animated waiting message, a pending query can be canceled and failed
  requests are reported in the query history
- Conversation is preserved: up to 20 last answered questions with their answers are
  sent back as a query context, so follow-up questions can be asked. Information about
  the user (full name, e-mail and territory taken from `RApplicationSettings`) is sent
  with the first query of a conversation only
- Derived widgets fill in optional fields of the submitted query in `buildQuery()` and
  restart the conversation with `hasHistory()` and `clearHistory()`. Public
  `cancelQuery()` does nothing when no answer is being awaited, so a hosting dialog can
  cancel a running query when it is being closed

#### Cloud file manager

- `RCloudFileManager::findRemoteFile()` returns the file info of a synchronized Cloud
  file matching given local file name. No additional file listing is requested, so the
  data directory synchronization is not disturbed
- `RApplication` keeps the cloud file manager and provides it through
  `getCloudFileManager()`

#### Application settings

- `RApplicationSettings` holds a new **do not use native menubar** option, stored under
  `application/dontUseNativeMenuBar` and disabled by default. `RApplication` turns it into
  the `Qt::AA_DontUseNativeMenuBar` application attribute before the main window is built,
  so the menubar is drawn inside the application window instead of the system menubar
- The option is offered in the **Appearance** tab of `RApplicationSettingsWidget` and is
  only available on macOS, the single platform of the supported ones providing a native
  menubar. `RApplicationSettings::isNativeMenuBarSupported()` reports the availability;
  elsewhere the getter always returns false and the setter does nothing
- Taking the option into account requires an application restart, which is reported the
  same way as a language or a format change

### Bug fixes

- `RAction::createSeparator()` returns a new separator action, so a widget can be
  given more than one. A single shared separator action could not be used twice
  in the same menu, tool bar or widget: `QWidget::insertAction()` removes an
  action the widget already holds before inserting it again, so every further
  add moved the one separator to the end and only the last one was displayed

---

## Version 1.0.0

### Improvements

#### AI assistant

- Abstract AI chat widget for embedding AI conversations in the application
- AI agent settings widget with configurable model parameters including temperature
- AI agent configuration with file tools and extended agent settings

#### Certificate management

- Automatic client certificate renewal
- Configurable days-to-expiry warning threshold
- Improved CSR creation with predefined O and OU subject fields
- Keystore and truststore paths replaced with dedicated configuration objects

#### Cloud file manager

- File manager improvements: sorting by filename, cache file, and auto data sync
- GUI support for file-replace functionality
- Cloud user registration
- Sync to cloud disabled by default
- Refresh button always enabled; removed background refresh timer

#### Output and logging

- Qt debug log level support added
- Timestamps printed in output window
- Qt library information printed on startup

#### User interface

- Color scheme support; obsolete themes removed
- Custom progress dialog
- Configurable software update check
- Report sender
- Help viewer reads content based on file extension; defaults to Markdown
- Register user button made more visually distinguishable
- Replaced `QDialog::exec()` with asynchronous alternatives

### Bug fixes

- Crash when reverting settings to defaults
- Default value not being applied on reset to defaults
- Software update check radio button showing wrong status
- Default format not being set correctly
- Incorrect signal blocking in cloud session widget when identity becomes available
