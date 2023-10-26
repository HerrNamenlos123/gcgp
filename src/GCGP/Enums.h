#ifdef __cplusplus
#ifndef GCGP_ENUMS_H
#define GCGP_ENUMS_H

enum class MotionType {
    Undefined,
    Rapid,                      // G0
    Feed,                       // G1 
    ArcCW,                      // G2
    ArcCCW                      // G3
                                // G33 Rigid-tapping (not supported)
                                // G73 High-speed peck drilling (not supported)
                                // G76 Threading cycle (not supported)
                                // G80 Cancel canned cycle (not supported)
                                // G89 Boring cycle (not supported)
                                // .....
};

enum class DistanceMode {
	Undefined,
	Absolute,                   // G90
	Relative                    // G91
};

enum class LengthUnits {
	Undefined,
	Metric,                     // G21
	Imperial                    // G20
};

enum class ArcPlaneMode {
    Undefined,
    XY,                         // G17
    XZ,                         // G18
    YZ,                         // G19
};

enum class FeedrateMode {
    Undefined,
    InverseTime,                // G93 -> every command MUST have F, each move is completed in 1/F minutes. In this mode, F on non-motion commands is ignored.
    UnitsPerMinute,             // G94 -> mm/min or in/min
    UnitsPerRevolution          // G95 -> mm/rev or in/rev
};

enum class SpindleAction {
    Undefined,
    Clockwise,                  // M3
    CounterClockwise,           // M4
    Stop                        // M5
};

enum class CoolandAction {
    Undefined,
    Mist,                       // M7
    Flood,                      // M8
    Stop                        // M9
};

enum class OverrideAction {
    Undefined,
    EnableSpeedAndFeedOverride,         // M48
    DisableSpeedAndFeedOverride,        // M49
    // EnableSpindleOverride,           // M50 (not supported)
    // DisableSpindleOverride,          // M51 (not supported)
    // EnableRapidOverride,             // M52 (not supported)
    // DisableRapidOverride             // M53 (not supported)
};

enum class ReferencePositionAction {
    Undefined,
    GoToPrimaryReferencePosition,      // G28
    SetPrimaryReferencePosition,       // G28.1
    GoToSecondaryReferencePosition,    // G30
    SetSecondaryReferencePosition      // G30.1
};

enum class SetOffsetAction {      // https://linuxcnc.org/docs/html/gcode/g-code.html#gcode:g10-l11
    Undefined,
    SetToolOffset,                                 // 'G10 L1' (tool radius not supported) -> Set tool offset to given values
    SetToolOffsetToCurrentPosition,                // 'G10 L10' (tool radius not supported) -> Calculate tool offset such that the given value is the new current position
    SetToolOffsetToCurrentPositionSkipOffset,      // 'G10 L11' (tool radius not supported) -> Calculate tool offset such that the given value is the new current position and skip some kind of offset
    SetCoordinateSystemOffset,                     // 'G10 L2' (rotation not supported) -> Set coordinate system offset to the given value
    SetCoordinateSystemOffsetToCurrentPosition,    // 'G10 L20' (rotation not supported) -> Calculate coordinate system offset such that the given value is the new current position
};

enum class AxisOffsetAction {
    Undefined,
    SetAxisOffset,                       // G92 (X, Y and Z are optional, but one must be used)
    ClearAxisOffset,                     // G92.1, G92.2, G92.3  -> TODO: THIS IS NOT REALLY COMPLIANT!!!
};

enum class StopAction {
    Undefined,
    Pause,                               // M0 -> Pause program
    OptionalStop,                        // M1 -> Pause program only if optional stop switch is on
    EndProgram,                          // M2, M30 -> End program, Cycle Start will restart it
    //Reset,                             // M30 -> Reset program. (not implemented, M30 = M2)
};                                       // Both M2 and M30 will reset the controller: G54, G17, G90, G94, M48, G40, M5, G1, M9

enum class ModalGroup {          // http://www.linuxcnc.org/docs/2.4/html/gcode_overview.html#cap:Modal-Groups
    None,                        // These are all the modal groups, which defines that every modal group has
    Motion,                      // many possible values, but only one can ever be active at a time. For example,
    PlaneSelection,              // the Motion group can have be in Rapid or Feed mode, but never both at the same time.
    DiameterRadiusSelection,     // Different groups are independent of each other.
    DistanceMode,
    FeedRateMode,
    MetricImperialSelection,
    CutterRadiusCompensation,
    ToolLengthOffset,
    ReturnModeInCannedCycles,
    CoordinateSystemSelection,
    Stopping,
    ToolChange,
    SpindleTurning,
    Coolant,
    OverrideSwitches,
    FlowControl
};

#define G0_MODAL_GROUP ModalGroup::Motion
#define G1_MODAL_GROUP ModalGroup::Motion
#define G2_MODAL_GROUP ModalGroup::Motion
#define G3_MODAL_GROUP ModalGroup::Motion

#define G17_MODAL_GROUP ModalGroup::PlaneSelection
#define G18_MODAL_GROUP ModalGroup::PlaneSelection
#define G19_MODAL_GROUP ModalGroup::PlaneSelection

enum class GrblError {  // https://www.sainsmart.com/blogs/news/grbl-v1-1-quick-reference
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
    FeedRateHasNotYetBeenSetOrIsUndefined = 22,
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
    GCodeG10MissingParameter,

    FeatureNotYetImplemented = 200,
    TurningFeaturesNotYetImplemented,
};

#endif // GCGP_ENUMS_H
#endif // __cplusplus