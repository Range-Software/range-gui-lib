# Range GUI Library

Shared Qt widget library providing reusable UI components, dialogs, and application-level services for Range desktop applications.

## What it provides

**Application & settings** — `RApplication` manages the Qt application lifecycle; `RApplicationSettings`/`Widget`/`Dialog` provide a unified preferences layer.

**Cloud UI** — `RCloudSessionManager`/`Widget`/`Dialog`, `RCloudFileManager`/`Widget`/`Dialog`, `RCloudConnectionHandler`, and `RCloudReportSender` expose cloud connectivity to the user.

**AI agent UI** — `RAiAgentSettingsManager`/`Widget`/`Dialog`/`List` and `RAiChatWidget` integrate LLM-backed agents into the application UI.

**Dialogs** — General-purpose dialogs for access control, crash reporting, document viewing, file tags and versions, first-run setup, help, OpenSSL CSR generation, software updates, and progress display.

**UI components** — `RLogBrowser`, `RHtmlViewer`, `RTextBrowser`, `RDocumentWidget`, `RFlowLayout`, `RTreeWidget`, `RProgressBar`, `RFileChooserButton`, `RKeyboardShortcutsEditorWidget`, `RTerritoryComboBox`, and others.

**Utilities** — `RAction`/`RActionList` (menu and action management), `RStyle`, `RMessageBox`, `RFileUpdater`, `RSoftwareUpdateChecker`, `RLoggerHandler`, `RProgressHandler`, `RLockerHandler`, `RProxySettingsWidget`.

## Class prefix

All public classes use the `R` prefix (e.g. `RApplication`, `RCloudFileManager`). Header files use the `rgl_` prefix.

## Dependencies

- Qt 6.10+
- range-base-lib
- range-model-lib
- range-cloud-lib
- range-ai-lib
- C++17
