# Create a static library
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_add_library(range-gui-lib
    STATIC
        src/rgl_access_mode_dialog.cpp
        src/rgl_access_owner_dialog.cpp
        src/rgl_action.cpp
        src/rgl_action_list.cpp
        src/rgl_application.cpp
        src/rgl_application_settings.cpp
        src/rgl_application_settings_dialog.cpp
        src/rgl_application_settings_widget.cpp
        src/rgl_cloud_connection_handler.cpp
        src/rgl_cloud_file_manager_dialog.cpp
        src/rgl_cloud_file_manager_widget.cpp
        src/rgl_cloud_session_dialog.cpp
        src/rgl_cloud_session_widget.cpp
        src/rgl_crash_report_dialog.cpp
        src/rgl_document_dialog.cpp
        src/rgl_document_widget.cpp
        src/rgl_file_chooser_button.cpp
        src/rgl_file_tags_dialog.cpp
        src/rgl_file_updater.cpp
        src/rgl_file_version_dialog.cpp
        src/rgl_first_run_dialog.cpp
        src/rgl_flow_layout.cpp
        src/rgl_help_actions_widget.cpp
        src/rgl_help_dialog.cpp
        src/rgl_help_document_widget.cpp
        src/rgl_html_viewer.cpp
        src/rgl_keyboard_shortcut_line_edit.cpp
        src/rgl_keyboard_shortcuts_editor_widget.cpp
        src/rgl_label_widget.cpp
        src/rgl_locker_handler.cpp
        src/rgl_log_browser.cpp
        src/rgl_log_browser_dialog.cpp
        src/rgl_logger_handler.cpp
        src/rgl_message_box.cpp
        src/rgl_open_ssl_csr_dialog.cpp
        src/rgl_open_ssl_tool.cpp
        src/rgl_open_ssl_tool_settings.cpp
        src/rgl_progress_bar.cpp
        src/rgl_progress_handler.cpp
        src/rgl_proxy_settings_widget.cpp
        src/rgl_software_manager_dialog.cpp
        src/rgl_software_manager_widget.cpp
        src/rgl_software_update_checker.cpp
        src/rgl_style.cpp
        src/rgl_text_browser.cpp
        src/rgl_text_browser_dialog.cpp
        src/rgl_tree_widget.cpp

        include/rgl_access_mode_dialog.h
        include/rgl_access_owner_dialog.h
        include/rgl_action.h
        include/rgl_action_list.h
        include/rgl_application.h
        include/rgl_application_settings_dialog.h
        include/rgl_application_settings.h
        include/rgl_application_settings_widget.h
        include/rgl_cloud_connection_handler.h
        include/rgl_cloud_file_manager_dialog.h
        include/rgl_cloud_file_manager_widget.h
        include/rgl_cloud_session_dialog.h
        include/rgl_cloud_session_widget.h
        include/rgl_crash_report_dialog.h
        include/rgl_document_dialog.h
        include/rgl_document_widget.h
        include/rgl_file_chooser_button.h
        include/rgl_file_tags_dialog.h
        include/rgl_file_updater.h
        include/rgl_file_version_dialog.h
        include/rgl_first_run_dialog.h
        include/rgl_flow_layout.h
        include/rgl_help_actions_widget.h
        include/rgl_help_dialog.h
        include/rgl_help_document_widget.h
        include/rgl_html_viewer.h
        include/rgl_keyboard_shortcut_line_edit.h
        include/rgl_keyboard_shortcuts_editor_widget.h
        include/rgl_label_widget.h
        include/rgl_locker_handler.h
        include/rgl_log_browser_dialog.h
        include/rgl_log_browser.h
        include/rgl_logger_handler.h
        include/rgl_message_box.h
        include/rgl_open_ssl_csr_dialog.h
        include/rgl_open_ssl_tool.h
        include/rgl_open_ssl_tool_settings.h
        include/rgl_progress_bar.h
        include/rgl_progress_handler.h
        include/rgl_proxy_settings_widget.h
        include/rgl_software_manager_dialog.h
        include/rgl_software_manager_widget.h
        include/rgl_software_update_checker.h
        include/rgl_style.h
        include/rgl_text_browser_dialog.h
        include/rgl_text_browser.h
        include/rgl_tree_widget.h
        include/rgl_progress_dialog.h src/rgl_progress_dialog.cpp
        include/rgl_cloud_report_sender.h src/rgl_cloud_report_sender.cpp
)

# Include directories
target_include_directories(range-gui-lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(range-gui-lib
    PUBLIC
        range-cloud-lib
        Qt6::Widgets
        common_defines
)
target_link_libraries(range-gui-lib PRIVATE Qt6::Core)

qt_add_translations(range-gui-lib
    TS_FILES
        translations/en.ts
        translations/sk.ts
    RESOURCE_PREFIX /i18n/range-gui-lib
)
