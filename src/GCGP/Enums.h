#ifdef __cplusplus
#ifndef GCGP_ENUMS_H
#define GCGP_ENUMS_H

enum class MotionType {
    None,
    Rapid, // G0
    Feed,  // G1
    ArcCW, // G2
    ArcCCW // G3
           // G33 Rigid-tapping (not supported)
           // G73 High-speed peck drilling (not supported)
           // G76 Threading cycle (not supported)
           // G80 Cancel canned cycle (not supported)
           // G89 Boring cycle (not supported)
           // .....
};

enum class DistanceMode {
    None,
    Absolute, // G90
    Relative  // G91
};

enum class LengthUnits {
    None,
    Metric,  // G21
    Imperial // G20
};

enum class ArcPlaneMode {
    None,
    XY, // G17
    XZ, // G18
    YZ, // G19
};

enum class FeedrateMode {
    None,
    InverseTime,       // G93 -> every command MUST have F, each move is completed in 1/F
                       // minutes. In this mode, F on non-motion commands is ignored.
    UnitsPerMinute,    // G94 -> mm/min or in/min
    UnitsPerRevolution // G95 -> mm/rev or in/rev
};

enum class SpindleAction {
    None,
    Clockwise,        // M3
    CounterClockwise, // M4
    Stop              // M5
};

enum class CoolantAction {
    None,
    Mist,  // M7
    Flood, // M8
    Stop   // M9
};

enum class OverrideAction {
    None,
    EnableSpeedAndFeedOverride,  // M48
    DisableSpeedAndFeedOverride, // M49
    // EnableSpindleOverride,           // M50 (not supported)
    // DisableSpindleOverride,          // M51 (not supported)
    // EnableRapidOverride,             // M52 (not supported)
    // DisableRapidOverride             // M53 (not supported)
};

enum class ReferencePositionAction {
    None,
    GoToPrimaryReferencePosition,   // G28
    SetPrimaryReferencePosition,    // G28.1
    GoToSecondaryReferencePosition, // G30
    SetSecondaryReferencePosition   // G30.1
};

enum class
    SetOffsetAction { // https://linuxcnc.org/docs/html/gcode/g-code.html#gcode:g10-l11
        None,
        SetToolOffset, // 'G10 L1' (tool radius not supported) -> Set tool offset to given
                       // values
        SetToolOffsetToCurrentPosition, // 'G10 L10' (tool radius not supported) ->
                                        // Calculate tool offset such that the given value
                                        // is the new current position
        SetToolOffsetToCurrentPositionSkipOffset, // 'G10 L11' (tool radius not supported)
                                                  // -> Calculate tool offset such that
                                                  // the given value is the new current
                                                  // position and skip some kind of offset
        SetCoordinateSystemOffset, // 'G10 L2' (rotation not supported) -> Set coordinate
                                   // system offset to the given value
        SetCoordinateSystemOffsetToCurrentPosition, // 'G10 L20' (rotation not supported)
                                                    // -> Calculate coordinate system
                                                    // offset such that the given value is
                                                    // the new current position
    };

enum class AxisOffsetAction {
    None,
    SetAxisOffset,   // G92 (X, Y and Z are optional, but one must be used)
    ClearAxisOffset, // G92.1, G92.2, G92.3  -> TODO: THIS IS NOT REALLY COMPLIANT!!!
};

enum class StopAction {
    None,
    Pause,        // M0 -> Pause program
    OptionalStop, // M1 -> Pause program only if optional stop switch is on
    EndProgram,   // M2, M30 -> End program, Cycle Start will restart it
    // Reset,                             // M30 -> Reset program. (not implemented, M30 =
    // M2)
}; // Both M2 and M30 will reset the controller: G54, G17, G90, G94, M48, G40, M5, G1, M9

// enum class
//     ModalGroup { //
//     http://www.linuxcnc.org/docs/2.4/html/gcode_overview.html#cap:Modal-Groups
//         None, // These are all the modal groups, which defines that every modal group
//         has Motion, // many possible values, but only one can ever be active at a time.
//         For
//                 // example,
//         PlaneSelection, // the Motion group can have be in Rapid or Feed mode, but
//         never
//                         // both at the same time.
//         DiameterRadiusSelection, // Different groups are independent of each other.
//         DistanceMode,
//         FeedRateMode,
//         MetricImperialSelection,
//         CutterRadiusCompensation,
//         ToolLengthOffset,
//         ReturnModeInCannedCycles,
//         CoordinateSystemSelection,
//         Stopping,
//         ToolChange,
//         SpindleTurning,
//         Coolant,
//         OverrideSwitches,
//         FlowControl
//     };
//
// #define G0_MODAL_GROUP ModalGroup::Motion
// #define G1_MODAL_GROUP ModalGroup::Motion
// #define G2_MODAL_GROUP ModalGroup::Motion
// #define G3_MODAL_GROUP ModalGroup::Motion
//
// #define G17_MODAL_GROUP ModalGroup::PlaneSelection
// #define G18_MODAL_GROUP ModalGroup::PlaneSelection
// #define G19_MODAL_GROUP ModalGroup::PlaneSelection

enum class GrblError { // https://www.sainsmart.com/blogs/news/grbl-v1-1-quick-reference
    None = 0,

    // These are official GRBL error codes
    GCodeCommandLetterNotFound = 1,
    GCodeCommandValueInvalidOrMissing = 2,
    GrblSystemCmdNotRecognizedOrSupported = 3,
    NegativeValueForAnExpectedPositiveValue = 4,
    HomingFailHomingNotEnabledInSettings = 5,
    MinStepPulseMustBeGreaterThan3usec = 6,
    EEPROMReadFailedDefaultValuesUsed = 7,
    GrblSystemCmdOnlyValidWhenIdle = 8,
    GCodeCommandsInvalidInAlarmOrJogState = 9,
    SoftLimitsRequireHomingToBeEnabled = 10,
    MaxCharactersPerLineExceeded = 11,
    GrblSystemCmdSettingExceedsTheMaximumStepRate = 12,
    SafetyDoorOpenedAndDoorStateInitiated = 13,
    BuildInfoOrStartUpLineEEPROMLineLength = 14,
    JogTargetExceedsMachineTravelIgnored = 15,
    JogCmdMissingOrHasProhibitedGCode = 16,
    LaserModeRequiresPWMOutput = 17,
    UnsupportedOrInvalidGCodeCommand = 20,
    MoreThanOneGCodeCommandInAModalGroupInBlock = 21,
    FeedRateHasNotYetBeenSetOrIsNone = 22,
    GCodeCommandRequiresAnIntegerValue = 23,
    MoreThanOneGCodeCommandUsingAxisWordsFound = 24,
    RepeatedGCodeWordFoundInBlock = 25,
    NoAxisWordsFoundInCommandBlock = 26,
    LineNumberValueIsInvalid = 27,
    GCodeCmdMissingARequiredValueWord = 28,
    G59xWCSAreNotSupported = 29,
    G53OnlyValidWithG0AndG1MotionModes = 30,
    UnneededAxisWordsFoundInBlock = 31,
    G2G3ArcsNeedAtLeastOneInPlaneAxisWord = 32,
    MotionCommandTargetIsInvalid = 33,
    ArcRadiusValueIsInvalid = 34,
    G2G3ArcsNeedAtLeastOneInPlaneOffsetWord = 35,
    UnusedValueWordsFoundInBlock = 36,
    G431OffsetNotAssignedToToolLengthAxis = 37,
    ToolNumberGreaterThanMaxSupportedValue = 38,

    // These are additional custom error codes
    GCodeUnsupportedCommand = 100,
    GCodeUnsupportedGCommand,
    GCodeUnsupportedMCommand,
    GCodeFloatingPointValueNotAllowed,
    GCodeNegativeValueNotAllowed,
    GCodeNegativeValueAndZeroNotAllowed,
    GCodeMultipleModalCommandsInOneBlock,
    GCodeDwellTimeMissing,
    GCodeDwellTimeInvalid,
    GCodeLonelyParameter,
    GCodeMultiplyDefinedParameters,
    GCodeTooManyParameters,
    GCodeG10MissingParameter,

    FeatureNotYetImplemented = 200,
    TurningFeaturesNotYetImplemented,
};

static const char *ErrorEnumToString(GrblError error)
{
    switch (error) {
    case GrblError::None:
        return "";
    case GrblError::GCodeCommandLetterNotFound:
        return "GCode Command letter was not found.";
    case GrblError::GCodeCommandValueInvalidOrMissing:
        return "GCode Command value invalid or missing.";
    case GrblError::GrblSystemCmdNotRecognizedOrSupported:
        return "Grbl '$' not recognized or supported.";
    case GrblError::NegativeValueForAnExpectedPositiveValue:
        return "Negative value for an expected positive value.";
    case GrblError::HomingFailHomingNotEnabledInSettings:
        return "Homing fail. Homing not enabled in settings.";
    case GrblError::MinStepPulseMustBeGreaterThan3usec:
        return "Min step pulse must be greater than 3usec.";
    case GrblError::EEPROMReadFailedDefaultValuesUsed:
        return "EEPROM read failed. Default values used.";
    case GrblError::GrblSystemCmdOnlyValidWhenIdle:
        return "Grbl '$' command Only valid when Idle.";
    case GrblError::GCodeCommandsInvalidInAlarmOrJogState:
        return "GCode commands invalid in alarm or jog state.";
    case GrblError::SoftLimitsRequireHomingToBeEnabled:
        return "Soft limits require homing to be enabled.";
    case GrblError::MaxCharactersPerLineExceeded:
        return "Max characters per line exceeded. Ignored.";
    case GrblError::GrblSystemCmdSettingExceedsTheMaximumStepRate:
        return "Grbl '$' setting exceeds the maximum step rate.";
    case GrblError::SafetyDoorOpenedAndDoorStateInitiated:
        return "Safety door opened and door state initiated.";
    case GrblError::BuildInfoOrStartUpLineEEPROMLineLength:
        return "Build info or start-up line > EEPROM line length";
    case GrblError::JogTargetExceedsMachineTravelIgnored:
        return "Jog target exceeds machine travel, ignored.";
    case GrblError::JogCmdMissingOrHasProhibitedGCode:
        return "Jog Cmd missing '=' or has prohibited GCode.";
    case GrblError::LaserModeRequiresPWMOutput:
        return "Laser mode requires PWM output.";
    case GrblError::UnsupportedOrInvalidGCodeCommand:
        return "Unsupported or invalid GCode command.";
    case GrblError::MoreThanOneGCodeCommandInAModalGroupInBlock:
        return "> 1 GCode command in a modal group in block.";
    case GrblError::FeedRateHasNotYetBeenSetOrIsNone:
        return "Feed rate has not yet been set or is undefined.";
    case GrblError::GCodeCommandRequiresAnIntegerValue:
        return "GCode command requires an integer value.";
    case GrblError::MoreThanOneGCodeCommandUsingAxisWordsFound:
        return "> 1 GCode command using axis words found.";
    case GrblError::RepeatedGCodeWordFoundInBlock:
        return "Repeated GCode word found in block.";
    case GrblError::NoAxisWordsFoundInCommandBlock:
        return "No axis words found in command block.";
    case GrblError::LineNumberValueIsInvalid:
        return "Line number value is invalid.";
    case GrblError::GCodeCmdMissingARequiredValueWord:
        return "GCode Cmd missing a required value word.";
    case GrblError::G59xWCSAreNotSupported:
        return "G59.x WCS are not supported.";
    case GrblError::G53OnlyValidWithG0AndG1MotionModes:
        return "G53 only valid with G0 and G1 motion modes.";
    case GrblError::UnneededAxisWordsFoundInBlock:
        return "Unneeded Axis words found in block.";
    case GrblError::G2G3ArcsNeedAtLeastOneInPlaneAxisWord:
        return "G2/G3 arcs need >= 1 in-plane axis word.";
    case GrblError::MotionCommandTargetIsInvalid:
        return "Motion command target is invalid.";
    case GrblError::ArcRadiusValueIsInvalid:
        return "Arc radius value is invalid.";
    case GrblError::G2G3ArcsNeedAtLeastOneInPlaneOffsetWord:
        return "G2/G3 arcs need >= 1 in-plane offset word.";
    case GrblError::UnusedValueWordsFoundInBlock:
        return "Unused value words found in block.";
    case GrblError::G431OffsetNotAssignedToToolLengthAxis:
        return "G43.1 offset not assigned to tool length axis.";
    case GrblError::ToolNumberGreaterThanMaxSupportedValue:
        return "Tool number greater than max value.";
    case GrblError::GCodeUnsupportedCommand:
        return "Unsupported command letter encountered.";
    case GrblError::GCodeUnsupportedGCommand:
        return "Unsupported G-Code encountered.";
    case GrblError::GCodeUnsupportedMCommand:
        return "Unsupported M-Code encountered.";
    case GrblError::GCodeFloatingPointValueNotAllowed:
        return "Floating point value not allowed in this word.";
    case GrblError::GCodeNegativeValueNotAllowed:
        return "This argument must be > 0";
    case GrblError::GCodeNegativeValueAndZeroNotAllowed:
        return "This argument must be >= 0";
    case GrblError::GCodeMultipleModalCommandsInOneBlock:
        return "Multiple modal commands encountered in one block.";
    case GrblError::GCodeDwellTimeMissing:
        return "Dwell command is missing dwell time.";
    case GrblError::GCodeDwellTimeInvalid:
        return "Dwell command was supplied with invalid dwell time.";
    case GrblError::GCodeLonelyParameter:
        return "Command contains a lonely parameter.";
    case GrblError::GCodeMultiplyDefinedParameters:
        return "Command contains multiply defined parameters.";
    case GrblError::GCodeTooManyParameters:
        return "Command contains more parameters than supported.";
    case GrblError::GCodeG10MissingParameter:
        return "Command contains G10 but not enough parameters for it.";
    case GrblError::FeatureNotYetImplemented:
        return "Feature not yet implemented.";
    case GrblError::TurningFeaturesNotYetImplemented:
        return "Turning features are not yet implemented.";
    default:
        return "Invalid error enum";
    }
}

#endif // GCGP_ENUMS_H
#endif // __cplusplus
