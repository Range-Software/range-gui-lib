## Version 1.1.0

### Improvements

#### Cloud AI query

- `RCloudAiQueryWidget`: implemented Range Cloud AI query. The question is submitted
  with `RCloudClient::requestAIQuery()` and the answer is fetched with
  `RCloudClient::requestAIQueryResult()` until the query is completed. Unlike the AI
  chat widget no AI agent is selected and no file upload is supported, only the
  response language can be chosen

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
