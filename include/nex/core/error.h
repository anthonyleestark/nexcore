/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @enum    ErrorCode
 * @brief   Standardized error codes for Nex-ecosystem
 * 
 * Defines a set of error codes that can be returned by various functions
 * across the Nex-ecosystem. These codes provide a consistent way to
 * represent different error conditions, allowing for better error handling
 * and user feedback.
 * 
 * @details
 * The error codes are categorized into different ranges for general errors,
 * platform-specific errors, system errors, and persistence errors. Each code
 * has a unique value and can be converted to a string for logging or user
 * messages. Additionally, utility functions are provided to check if an error
 * is recoverable or to get a user-friendly message for a given error code.
 * 
 * @note The Ok code (0) indicates no error. All other codes represent specific error conditions.
 * @see Error structure, error handling utilities
 */
enum class ErrorCode : uint32 {
    // No error / success
    Ok = 0,

    // =======================================================================================
    // General errors (0x1000-0x1FFF)
    // Including most basic errors that can occur in various contexts, such as invalid 
    // arguments, null pointers, out of range errors, etc.
    // =======================================================================================

    InvalidArgument                 = 0x1000,
    NullPointer                     = 0x1001,
    OutOfRange                      = 0x1002,
    InvalidState                    = 0x1003,
    NotImplemented                  = 0x1004,
    OperationFailed                 = 0x1005,
    Timeout                         = 0x1006,
    OperationCancelled              = 0x1007,
    InternalError                   = 0x1008,
    UnknownError                    = 0x1009,
    NotSupported                    = 0x100A,
    DependencyMissing               = 0x100B,
    FeatureDisabled                 = 0x100C,
    InvalidFormat                   = 0x100D,
    DataCorrupted                   = 0x100E,
    BufferTooSmall                  = 0x100F,
    InvalidType                     = 0x1010,
    ResourceUnavailable             = 0x1011,
    EntryNotFound                   = 0x1012,
    AlreadyExists                   = 0x1013,

    // =======================================================================================
    // Platform errors (0x2000-0x2FFF)
    // Including errors related to OS, hardware, platform APIs, etc.
    // =======================================================================================

    PlatformNotSupported            = 0x2000,
    PlatformOperationFailed         = 0x2001,
    OsVersionUnsupported            = 0x2002,
    ArchitectureUnsupported         = 0x2003,
    PlatformApiError                = 0x2004,
    WindowsSpecificError            = 0x2005,   // wrapper for GetLastError()
    MacOsSpecificError              = 0x2006,   // wrapper for errno / OSStatus
    LinuxSpecificError              = 0x2007,   // wrapper for errno

    // =======================================================================================
    // System errors (0x3000-0x3FFF)
    // Including errors related to system resources, permissions, processes, threads, etc.
    // =======================================================================================

    InsufficientPrivileges          = 0x3000,
    SystemNotSupported              = 0x3001,
    PermissionDenied                = 0x3002,
    ResourceExhausted               = 0x3003,
    OutOfMemory                     = 0x3004,
    DeviceNotAvailable              = 0x3005,
    DriverError                     = 0x3006,
    HandleLimitReached              = 0x3007,
    ThreadCreationFailed            = 0x3008,
    ProcessLimitExceeded            = 0x3009,

    // =======================================================================================
    // Persistence / Storage errors (0x4000-0x4FFF)
    // Including errors related to I/O mechanisms, e.g., file I/O, registry access, 
    // serialization, deserialization, etc.
    // =======================================================================================

    RegistryAccessFailed            = 0x4000,
    FileNotFound                    = 0x4001,
    FileReadError                   = 0x4002,
    FileWriteError                  = 0x4003,
    FilePermissionDenied            = 0x4004,
    SerializationFailed             = 0x4005,
    DeserializationFailed           = 0x4006,
    DirectoryNotFound               = 0x4007,
    AccessDenied                    = 0x4008,
    IoError                         = 0x4009,
    DiskFull                        = 0x400A,
    FileLocked                      = 0x400B,
    PathTooLong                     = 0x400C,
    CorruptedData                   = 0x400D,
    WriteProtected                  = 0x400E,
    QuotaExceeded                   = 0x400F,

    // =======================================================================================
    // Configuration errors (0x5000-0x5FFF)
    // Including errors related to configuration management, such as missing entries, invalid 
    // values, failed loads or saves, and version mismatches.
    // =======================================================================================

    ConfigNotFound                  = 0x5000,
    InvalidConfig                   = 0x5001,
    ConfigLoadFailed                = 0x5002,
    ConfigSaveFailed                = 0x5003,
    MissingRequiredSetting          = 0x5004,
    InvalidSettingValue             = 0x5005,
    ConfigParseError                = 0x5006,
    InvalidConfigFormat             = 0x5007,
    ConfigVersionMismatch           = 0x5008,
    SettingsMigrationFailed         = 0x5009,
    SettingTypeMismatch             = 0x500A,

    // =======================================================================================
    // Network / Communication errors (0x6000-0x6FFF)
    // Including errors related to network operations, such as connectivity issues, timeouts, 
    // protocol errors, etc.
    // =======================================================================================

    NetworkUnavailable              = 0x6000,
    ConnectionFailed                = 0x6001,
    RequestTimeout                  = 0x6002,
    ServerError                     = 0x6003,
    AuthenticationFailed            = 0x6004,
    CertificateInvalid              = 0x6005,
    DnsResolutionFailed             = 0x6006,
    RateLimitExceeded               = 0x6007,
    ProxyError                      = 0x6008,

    // =======================================================================================
    // UI / Graphics / Rendering errors (0x7000-0x7FFF)
    // Including errors related to UI components, graphics rendering, theme loading, font 
    // issues, etc.
    // =======================================================================================

    WindowCreationFailed            = 0x7000,
    RenderingError                  = 0x7001,
    GraphicsInitializationFailed    = 0x7002,
    ThemeLoadFailed                 = 0x7003,
    FontNotFound                    = 0x7004,
    WidgetNotFound                  = 0x7005,
    InvalidUiState                  = 0x7006,
    OverlayCreationFailed           = 0x7007,
    TrayIconError                   = 0x7008,

    // =======================================================================================
    // Plugin / Extension errors (0x8000-0x8FFF)
    // Including errors related to plugin or extension management, such as failed loads, 
    // compatibility issues, initialization failures, and security violations. 
    // This is important because Nex-ecosystem projects might be designed to be extensible and 
    // support extensions or plugins, so having a clear set of error codes for plugin-related 
    // issues is crucial for debugging and user feedback.
    // =======================================================================================

    PluginLoadFailed                = 0x8000,
    PluginNotCompatible             = 0x8001,
    InvalidPlugin                   = 0x8002,
    PluginInitializationFailed      = 0x8003,
    DependencyNotMet                = 0x8004,
    PluginAlreadyLoaded             = 0x8005,
    PluginSignatureInvalid          = 0x8006,
    PluginSecurityViolation         = 0x8007,

    // =======================================================================================
    // Security / Authentication errors (0x9000-0x9FFF)
    // Including errors related to security mechanisms, such as authentication failures, token
    // invalidation, encryption/decryption failures, and access violations. 
    // This is important for ensuring that security-related issues are clearly identified and 
    // handled appropriately, especially if a Nex-ecosystem project includes features that 
    // require authentication or handle sensitive data.
    // =======================================================================================

    SecurityViolation               = 0x9000,
    AuthenticationRequired          = 0x9001,
    TokenInvalid                    = 0x9002,
    EncryptionFailed                = 0x9003,
    DecryptionFailed                = 0x9004,
    SignatureInvalid                = 0x9005,
    AccessRevoked                   = 0x9006,

    // =======================================================================================
    // Search / Indexing errors (0xA000-0xAFFF)
    // Including errors related to search functionality, such as index corruption, failed 
    // builds, and invalid queries.
    // =======================================================================================

    IndexCorrupted                  = 0xA000,
    SearchIndexBuildFailed          = 0xA001,
    InvalidSearchQuery              = 0xA002,
    DatabaseConnectionFailed        = 0xA003,
    QueryExecutionFailed            = 0xA004,
    IndexOutOfDate                  = 0xA005,
    SearchServiceUnavailable        = 0xA006,

    // =======================================================================================
    // Input / Hotkey / Shortcut errors (0xB000-0xBFFF)
    // Including errors related to input handling, such as hotkey registration failures, invalid 
    // shortcuts, and input capture issues. This is important for ensuring that input-related
    // issues are clearly identified and can be addressed, especially if a Nex-ecosystem project 
    // includes features that rely on user input or customizable hotkeys and shortcuts.
    // =======================================================================================

    HotkeyRegistrationFailed        = 0xB000,
    HotkeyAlreadyRegistered         = 0xB001,
    InvalidHotkey                   = 0xB002,
    InputCaptureFailed              = 0xB003,
    ShortcutConflict                = 0xB004,
    KeyboardHookFailed              = 0xB005,
    MouseHookFailed                 = 0xB006,

    // =======================================================================================
    // Process / Execution errors (0xC000-0xCFFF)
    // Including errors related to process management and command execution, such as failed
    // process spawns, command execution failures, timeouts, and elevated process requirements.
    // This is important for ensuring that issues related to executing external processes or 
    // commands are clearly identified and can be handled appropriately, especially if 
    // a Nex-ecosystem project includes features that involve running external tools or 
    // commands as part of its functionality.
    // =======================================================================================

    ProcessSpawnFailed              = 0xC000,
    CommandExecutionFailed          = 0xC001,
    ProcessNotFound                 = 0xC002,
    ExecutionTimeout                = 0xC003,
    ExitCodeError                   = 0xC004,
    ElevatedProcessRequired         = 0xC005,

    // =======================================================================================
    // Update / Versioning errors (0xD000-0xDFFF)
    // Including errors related to application updates, such as failed downloads, installation
    // failures, version mismatches, and update conflicts. This is important for ensuring that
    // issues related to updating the application are clearly identified and can be addressed,
    // especially if a Nex-ecosystem project includes an auto-update mechanism or relies on 
    // version checks for compatibility.
    // =======================================================================================

    UpdateCheckFailed               = 0xD000,
    DownloadFailed                  = 0xD001,
    InstallationFailed              = 0xD002,
    VersionMismatch                 = 0xD003,
    UpdateNotAvailable              = 0xD004,
    PatchApplicationFailed          = 0xD005,
    SelfUpdateInProgress            = 0xD006,

    // =======================================================================================
    // User interaction errors (0xE000-0xEFFF)
    // Including errors related to user interactions, such as cancelled operations, invalid input,
    // and confirmation requirements. This is important for ensuring that issues related to user
    // interactions are clearly identified and can be handled appropriately, especially if
    // a Nex-ecosystem project includes features that involve user input or actions that can be 
    // cancelled or require confirmation.
    // =======================================================================================

    UserCancelled                   = 0xE000,
    InvalidUserInput                = 0xE001,
    ConfirmationNeeded              = 0xE002,   // may be treated as a "soft error"
    OperationAbortedByUser          = 0xE003,

    // =======================================================================================
    // Reserved for future / module-specific errors (0xF000-0xFFFF)
    // This range is reserved for future expansion or for module-specific error codes that 
    // may be defined by different components of the system. This allows for flexibility in 
    // adding new error codes as needed without conflicting with the existing standardized 
    // codes. Module-specific errors can be defined within this range to provide more 
    // specific error information for particular components or features of the system.
    // =======================================================================================

    ModuleSpecificErrorBase         = 0xF000,

    // =======================================================================================
    // Max error code (0xFFFF) - used for validation and bounds checking of error codes
    // Note: You can add more error codes as needed, but try to keep them organized and 
    // avoid conflicts with existing codes. Always provide a clear description for each 
    // error code to ensure that they are easy to understand and use correctly in error 
    // handling scenarios.
    // =======================================================================================

    MaxErrorCode                    = 0xFFFF

    // =======================================================================================
    // End of error code definitions
    // =======================================================================================
};

/**
 * @brief Convert error code to string
 * 
 * @details Returns a string representation of the error code for logging or debugging purposes.
 * 
 * @param code The error code to convert
 * @return A string representation of the error code
 * 
 * Example usage:
 * ```
 * ErrorCode code = ErrorCode::InvalidArgument;
 * const char* message = errorCodeToString(code);
 * ```
 */
NEX_EXPORT const char* errorCodeToString(ErrorCode code);

/**
 * @brief Get user-friendly message for an error code
 * 
 * @details Returns a user-friendly message corresponding to the error code, suitable for display in UI or logs.
 * 
 * @param code The error code to get the message for
 * @return A user-friendly message for the error code
 * 
 * Example usage:
 * ```
 * ErrorCode code = ErrorCode::FileNotFound;
 * const char* userMessage = getUserMessage(code);
 * ```
 */
NEX_EXPORT const char* getUserMessage(ErrorCode code);

/**
 * @brief Check if an error code is recoverable
 * 
 * @details Determines whether the given error code represents a recoverable error condition.
 * 
 * @param code The error code to check
 * @return true if the error is recoverable, false otherwise
 * 
 * Example usage:
 * ```
 * ErrorCode code = ErrorCode::OutOfRange;
 * if (isRecoverable(code)) {
 *     // Attempt recovery
 * } else {
 *     // Handle unrecoverable error
 * }
 * ```
 */
NEX_EXPORT bool isRecoverable(ErrorCode code);

/**
 * @struct Error
 * @brief Represents an error with code and message
 * 
 * This struct encapsulates an error condition, including an error code and a corresponding message.
 * It can be used to return detailed error information from functions, allowing for better error handling
 * and user feedback.
 * 
 * @details
 * The Error struct contains an ErrorCode to indicate the type of error and a const char* message that
 * provides additional context or details about the error. This structure can be extended in the future
 * to include more information such as error severity, source, or recovery suggestions.
 * 
 * Example usage:
 * ```
 * Error performOperation() {
 *     if (someCondition) {
 *         return { ErrorCode::InvalidArgument, "The provided argument is invalid." };
 *     }
 *     return { ErrorCode::Ok, "Operation completed successfully." };
 * }
 * ```
 */
struct NEX_EXPORT Error {
    /**
     * Error code which indicates the type of error.
     * This field is essential for identifying the specific error condition
     * that occurred during an operation. It allows developers to handle
     * different error scenarios appropriately based on the code.
     */
    ErrorCode code;

    /**
     * Human-readable message providing additional context about the error.
     * This message is intended for developers and users to understand
     * the nature of the error. It can include details about what went wrong,
     * possible causes, and suggestions for resolution.
     * This field is optional and can be left empty if not needed.
     */
    const char* message;
};

NEX_NAMESPACE_END
