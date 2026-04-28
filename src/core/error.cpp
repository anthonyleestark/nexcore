/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/error.h"

NEX_CORE_NAMESPACE_BEGIN

// Convert error code to string
const char* errorCodeToString(ErrorCode code) {
    switch (code) {
        // No error / success

        case ErrorCode::Ok:
            return "Ok";

        // General errors (0x1000-0x1FFF)
        
        case ErrorCode::InvalidArgument:
            return "InvalidArgument";
        case ErrorCode::NullPointer:
            return "NullPointer";
        case ErrorCode::OutOfRange:
            return "OutOfRange";
        case ErrorCode::InvalidState:
            return "InvalidState";
        case ErrorCode::NotImplemented:
            return "NotImplemented";
        case ErrorCode::OperationFailed:
            return "OperationFailed";
        case ErrorCode::Timeout:
            return "Timeout";
        case ErrorCode::OperationCancelled:
            return "OperationCancelled";
        case ErrorCode::InternalError:
            return "InternalError";
        case ErrorCode::UnknownError:
            return "UnknownError";
        case ErrorCode::NotSupported:
            return "NotSupported";
        case ErrorCode::DependencyMissing:
            return "DependencyMissing";
        case ErrorCode::FeatureDisabled:
            return "FeatureDisabled";
        case ErrorCode::InvalidFormat:
            return "InvalidFormat";
        case ErrorCode::DataCorrupted:
            return "DataCorrupted";
        case ErrorCode::BufferTooSmall:
            return "BufferTooSmall";
        case ErrorCode::InvalidType:
            return "InvalidType";
        case ErrorCode::ResourceUnavailable:
            return "ResourceUnavailable";
        case ErrorCode::EntryNotFound:
            return "EntryNotFound";
        case ErrorCode::AlreadyExists:
            return "AlreadyExists";

        // Platform errors (0x2000-0x2FFF)
        
        case ErrorCode::PlatformNotSupported:
            return "PlatformNotSupported";
        case ErrorCode::PlatformOperationFailed:
            return "PlatformOperationFailed";
        case ErrorCode::OsVersionUnsupported:
            return "OsVersionUnsupported";
        case ErrorCode::ArchitectureUnsupported:
            return "ArchitectureUnsupported";
        case ErrorCode::PlatformApiError:
            return "PlatformApiError";
        case ErrorCode::WindowsSpecificError:
            return "WindowsSpecificError";
        case ErrorCode::MacOsSpecificError:
            return "MacOsSpecificError";
        case ErrorCode::LinuxSpecificError:
            return "LinuxSpecificError";

        // System errors (0x3000-0x3FFF)

        case ErrorCode::InsufficientPrivileges:
            return "InsufficientPrivileges";
        case ErrorCode::SystemNotSupported:
            return "SystemNotSupported";
        case ErrorCode::PermissionDenied:
            return "PermissionDenied";
        case ErrorCode::ResourceExhausted:
            return "ResourceExhausted";
        case ErrorCode::OutOfMemory:
            return "OutOfMemory";
        case ErrorCode::DeviceNotAvailable:
            return "DeviceNotAvailable";
        case ErrorCode::DriverError:
            return "DriverError";
        case ErrorCode::HandleLimitReached:
            return "HandleLimitReached";
        case ErrorCode::ThreadCreationFailed:
            return "ThreadCreationFailed";
        case ErrorCode::ProcessLimitExceeded:
            return "ProcessLimitExceeded";
        
        // Persistence / Storage errors (0x4000-0x4FFF)
        case ErrorCode::RegistryAccessFailed:
            return "RegistryAccessFailed";
        case ErrorCode::FileNotFound:
            return "FileNotFound";
        case ErrorCode::FileReadError:
            return "FileReadError";
        case ErrorCode::FileWriteError:
            return "FileWriteError";
        case ErrorCode::FilePermissionDenied:
            return "FilePermissionDenied";
        case ErrorCode::SerializationFailed:
            return "SerializationFailed";
        case ErrorCode::DeserializationFailed:
            return "DeserializationFailed";
        case ErrorCode::DirectoryNotFound:
            return "DirectoryNotFound";
        case ErrorCode::AccessDenied:
            return "AccessDenied";
        case ErrorCode::IoError:
            return "IoError";
        case ErrorCode::DiskFull:
            return "DiskFull";
        case ErrorCode::FileLocked:
            return "FileLocked";
        case ErrorCode::PathTooLong:
            return "PathTooLong";
        case ErrorCode::CorruptedData:
            return "CorruptedData";
        case ErrorCode::WriteProtected:
            return "WriteProtected";
        case ErrorCode::QuotaExceeded:
            return "QuotaExceeded";

        // Configuration errors (0x5000-0x5FFF)

        case ErrorCode::ConfigNotFound:
            return "ConfigNotFound";
        case ErrorCode::InvalidConfig:
            return "InvalidConfig";
        case ErrorCode::ConfigLoadFailed:
            return "ConfigLoadFailed";
        case ErrorCode::ConfigSaveFailed:
            return "ConfigSaveFailed";
        case ErrorCode::MissingRequiredSetting:
            return "MissingRequiredSetting";
        case ErrorCode::InvalidSettingValue:
            return "InvalidSettingValue";
        case ErrorCode::ConfigParseError:
            return "ConfigParseError";
        case ErrorCode::InvalidConfigFormat:
            return "InvalidConfigFormat";
        case ErrorCode::ConfigVersionMismatch:
            return "ConfigVersionMismatch";
        case ErrorCode::SettingsMigrationFailed:
            return "SettingsMigrationFailed";
        case ErrorCode::SettingTypeMismatch:
            return "SettingTypeMismatch";
        
        // Network / Communication errors (0x6000-0x6FFF)

        case ErrorCode::NetworkUnavailable:
            return "NetworkUnavailable";
        case ErrorCode::ConnectionFailed:
            return "ConnectionFailed";
        case ErrorCode::RequestTimeout:
            return "RequestTimeout";
        case ErrorCode::ServerError:
            return "ServerError";
        case ErrorCode::AuthenticationFailed:
            return "AuthenticationFailed";
        case ErrorCode::CertificateInvalid:
            return "CertificateInvalid";
        case ErrorCode::DnsResolutionFailed:
            return "DnsResolutionFailed";
        case ErrorCode::RateLimitExceeded:
            return "RateLimitExceeded";
        case ErrorCode::ProxyError:
            return "ProxyError";

        // UI / Graphics / Rendering errors (0x7000-0x7FFF)

        case ErrorCode::WindowCreationFailed:
            return "WindowCreationFailed";
        case ErrorCode::RenderingError:
            return "RenderingError";
        case ErrorCode::GraphicsInitializationFailed:
            return "GraphicsInitializationFailed";
        case ErrorCode::ThemeLoadFailed:
            return "ThemeLoadFailed";
        case ErrorCode::FontNotFound:
            return "FontNotFound";
        case ErrorCode::WidgetNotFound:
            return "WidgetNotFound";
        case ErrorCode::InvalidUiState:
            return "InvalidUiState";
        case ErrorCode::OverlayCreationFailed:
            return "OverlayCreationFailed";
        case ErrorCode::TrayIconError:
            return "TrayIconError";

        // Plugin / Extension errors (0x8000-0x8FFF)

        case ErrorCode::PluginLoadFailed:
            return "PluginLoadFailed";
        case ErrorCode::PluginNotCompatible:
            return "PluginNotCompatible";
        case ErrorCode::InvalidPlugin:
            return "InvalidPlugin";
        case ErrorCode::PluginInitializationFailed:
            return "PluginInitializationFailed";
        case ErrorCode::DependencyNotMet:
            return "DependencyNotMet";
        case ErrorCode::PluginAlreadyLoaded:
            return "PluginAlreadyLoaded";
        case ErrorCode::PluginSignatureInvalid:
            return "PluginSignatureInvalid";
        case ErrorCode::PluginSecurityViolation:
            return "PluginSecurityViolation";

        // Security / Authentication errors (0x9000-0x9FFF)

        case ErrorCode::SecurityViolation:
            return "SecurityViolation";
        case ErrorCode::AuthenticationRequired:
            return "AuthenticationRequired";
        case ErrorCode::TokenInvalid:
            return "TokenInvalid";
        case ErrorCode::EncryptionFailed:
            return "EncryptionFailed";
        case ErrorCode::DecryptionFailed:
            return "DecryptionFailed";
        case ErrorCode::SignatureInvalid:
            return "SignatureInvalid";
        case ErrorCode::AccessRevoked:
            return "AccessRevoked";

        // Search / Indexing errors (0xA000-0xAFFF)

        case ErrorCode::IndexCorrupted:
            return "IndexCorrupted";
        case ErrorCode::SearchIndexBuildFailed:
            return "SearchIndexBuildFailed";
        case ErrorCode::InvalidSearchQuery:
            return "InvalidSearchQuery";
        case ErrorCode::DatabaseConnectionFailed:
            return "DatabaseConnectionFailed";
        case ErrorCode::QueryExecutionFailed:
            return "QueryExecutionFailed";
        case ErrorCode::IndexOutOfDate:
            return "IndexOutOfDate";
        case ErrorCode::SearchServiceUnavailable:
            return "SearchServiceUnavailable";

        // Input / Hotkey / Shortcut errors (0xB000-0xBFFF)

        case ErrorCode::HotkeyRegistrationFailed:
            return "HotkeyRegistrationFailed";
        case ErrorCode::HotkeyAlreadyRegistered:
            return "HotkeyAlreadyRegistered";
        case ErrorCode::InvalidHotkey:
            return "InvalidHotkey";
        case ErrorCode::InputCaptureFailed:
            return "InputCaptureFailed";
        case ErrorCode::ShortcutConflict:
            return "ShortcutConflict";
        case ErrorCode::KeyboardHookFailed:
            return "KeyboardHookFailed";
        case ErrorCode::MouseHookFailed:
            return "MouseHookFailed";

        // Process / Execution errors (0xC000-0xCFFF)

        case ErrorCode::ProcessSpawnFailed:
            return "ProcessSpawnFailed";
        case ErrorCode::CommandExecutionFailed:
            return "CommandExecutionFailed";
        case ErrorCode::ExecutionTimeout:
            return "ExecutionTimeout";
        case ErrorCode::ProcessNotFound:
            return "ProcessNotFound";
        case ErrorCode::ExitCodeError:
            return "ExitCodeError";
        case ErrorCode::ElevatedProcessRequired:
            return "ElevatedProcessRequired";

        // Update / Versioning errors (0xD000-0xDFFF)

        case ErrorCode::UpdateCheckFailed:
            return "UpdateCheckFailed";
        case ErrorCode::DownloadFailed:
            return "DownloadFailed";
        case ErrorCode::InstallationFailed:
            return "InstallationFailed";
        case ErrorCode::VersionMismatch:
            return "VersionMismatch";
        case ErrorCode::UpdateNotAvailable:
            return "UpdateNotAvailable";
        case ErrorCode::PatchApplicationFailed:
            return "PatchApplicationFailed";
        case ErrorCode::SelfUpdateInProgress:
            return "SelfUpdateInProgress";

        // User interaction errors (0xE000-0xEFFF)

        case ErrorCode::UserCancelled:
            return "UserCancelled";
        case ErrorCode::InvalidUserInput:
            return "InvalidUserInput";
        case ErrorCode::ConfirmationNeeded:
            return "ConfirmationNeeded";
        case ErrorCode::OperationAbortedByUser:
            return "OperationAbortedByUser";

        // Reserved for future / module-specific errors (0xF000-0xFFFF)

        case ErrorCode::ModuleSpecificErrorBase:
            return "ModuleSpecificErrorBase";

        // Otherwise, return a generic unknown error string
        // This should never happen if all error codes are handled above, 
        // but it's here as a fallback

        default:
            return "UnknownError";
    }
}

// Get user-friendly error message
const char* getUserMessage(ErrorCode code) {
    // TODO: Implement localized user messages
    return errorCodeToString(code);
}

// Check if error is recoverable
bool isRecoverable(ErrorCode code) {
    switch (code) {
        case ErrorCode::InvalidArgument:
        case ErrorCode::OutOfRange:
            return true;
        default:
            return false;
    }
}

NEX_CORE_NAMESPACE_END
