#include <cstdlib>
#ifdef __cplusplus
#ifndef GCGP_COMMAND_H
#define GCGP_COMMAND_H

#include "GCGP/Config.h"
#include "GCGP/Enums.h"
#include "GCGP/Serial.h"
#include "GCGP/String.h"
#include "GCGP/tokenize.h"

#define PRINT_FLOAT(name, text, ...)                                                     \
    if (!isnan(name)) {                                                                  \
        char buffer[64];                                                                 \
        snprintf(buffer, sizeof(buffer), text, __VA_ARGS__);                             \
        serial.println(buffer);                                                          \
    }
#define PRINT_BOOL(name, text, ...)                                                      \
    if (name) {                                                                          \
        char buffer[64];                                                                 \
        snprintf(buffer, sizeof(buffer), text, __VA_ARGS__);                             \
        serial.println(buffer);                                                          \
    }

template <size_t capacity> class Command { // https://linuxcnc.org/docs/html/
  public:
    // All actions are declared in the order they must be executed.
    FeedrateMode feedrateMode = FeedrateMode::None; // G93, G94, G95
    float setFeedrate = NAN;                        // F<>
    float setSpindleSpeed = NAN;                    // S<>
    bool prepareTool =
        false;               // T<> -> Prepare for tool change (can be on same line as M6)
    bool changeTool = false; // M6  -> This actually changes the tool
    SpindleAction spindleAction = SpindleAction::None; // M3, M4, M5
    CoolantAction coolantAction = CoolantAction::None; // M7, M8, M9
    // OverrideAction overrideAction = OverrideAction::None; // M48, M49 not supported
    // (M50, M51, M52, M53 are not supported)
    bool dwell = false; // G4 (P<> must be specified and positive)
    ArcPlaneMode arcPlaneMode = ArcPlaneMode::None; // G17, G18, G19
    LengthUnits lengthUnits = LengthUnits::None;    // G20, G21
    // CutterRadiusCompMode cutterRadiusCompMode = CutterRadiusCompMode::None; G41, G42
    // not supported, G40 is ignored CutterLengthCompMode cutterLengthCompMode =
    // CutterLengthCompMode::None;            // G43, G49 not supported CoordinateSystem
    // // G54-G59.3 not supported PathBlending G61.1, G64 not supported, G61 is ignored
    DistanceMode distanceMode = DistanceMode::None; // G90, G91
    // RetractMode G98, G99 not supported
    ReferencePositionAction referencePositionAction =
        ReferencePositionAction::None; // G28, G28.1, G30, G30.1
    SetOffsetAction offsetAction =
        SetOffsetAction::None; // G10 L<> (tool radius not supported)
    AxisOffsetAction axisOffsetAction =
        AxisOffsetAction::None;               // G92, G92.1, G92.2, G92.3
    MotionType motionType = MotionType::None; // G0, G1, G2, G3 | not supported: G33,
                                              // G38.2, G38.3, G38.4, G38.5, G80, G81,
                                              // G82, G83, G84, G85, G86, G87, G88, G89
    StopAction stopAction = StopAction::None; // M0, M1, M2, M30

    // These parameters are used by the actions defined above
    int toolNumber = -1;
    int coordinateSystem = -1;
    float dwellTime = NAN;
    float posX = NAN;
    float posY = NAN;
    float posZ = NAN;
    float arcI = NAN;
    float arcJ = NAN;
    float arcK = NAN;
    bool isGCode = false;
    bool isMCode = false;

    bool isSystemCommand = false;
    SystemCommand systemCommand;

    Command() = default;

    GrblError parse(const char *str, size_t strLength)
    {
        *this = {};
        CommandTokens<capacity> tokens;
        GrblError result = tokenizeCommand(tokens, str, strLength);
        if (result != GrblError::None) {
            return result;
        }

        bool G10 = false;
        int G10LNumber = -1;
        int G10PNumber = -1;

        for (size_t i = 0; i < tokens.numberOfValidTokens; i++) {
            char letter = tokens.tokens[i].type;
            float value = tokens.tokens[i].value;
            // int32_t intValue = (int32_t)value;
            // bool isInteger = fmodf(value, 1.f) == 0.f;  // Check if the float is a
            // whole number

            switch (letter) {

                case 'G':
                    isGCode = true;
                    if (value == 0) { // G0 (Rapid motion)
                        if (motionType != MotionType::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        motionType = MotionType::Rapid;
                    }
                    else if (value == 1.f) { // G1 (Linear motion)
                        if (motionType != MotionType::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        motionType = MotionType::Feed;
                    }
                    else if (value == 2.f) { // G2 (CW arc)
                        if (motionType != MotionType::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        motionType = MotionType::ArcCW;
                    }
                    else if (value == 3.f) { // G3 (CCW arc)
                        if (motionType != MotionType::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        motionType = MotionType::ArcCCW;
                    }
                    else if (value == 4.f) { // G4 (Dwell time)
                        if (dwell) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        dwell = true;
                    }
                    else if (value == 10.f) { // G10 (Offsets)
                        if (G10) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        G10 = true;
                    }
                    else if (value == 17.f) { // G17 (XY Arc plane)
                        if (arcPlaneMode != ArcPlaneMode::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        arcPlaneMode = ArcPlaneMode::XY;
                    }
                    else if (value == 18.f) { // G18 (XZ Arc plane)
                        if (arcPlaneMode != ArcPlaneMode::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        arcPlaneMode = ArcPlaneMode::XZ;
                    }
                    else if (value == 19.f) { // G19 (YZ Arc plane)
                        if (arcPlaneMode != ArcPlaneMode::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        arcPlaneMode = ArcPlaneMode::YZ;
                    }
                    else if (value == 20.f) { // G20 (Imperial)
                        if (lengthUnits != LengthUnits::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        lengthUnits = LengthUnits::Imperial;
                    }
                    else if (value == 21.f) { // G21 (Metric)
                        if (lengthUnits != LengthUnits::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        lengthUnits = LengthUnits::Metric;
                    }
                    else if (value == 28.f) { // G28 (Move to reference position)
                        if (referencePositionAction != ReferencePositionAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        referencePositionAction =
                            ReferencePositionAction::GoToPrimaryReferencePosition;
                    }
                    else if (value == 28.1f) { // G28.1 (Set reference position)
                        if (referencePositionAction != ReferencePositionAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        referencePositionAction =
                            ReferencePositionAction::SetPrimaryReferencePosition;
                    }
                    else if (value == 30.f) { // G30 (Move to second reference position)
                        if (referencePositionAction != ReferencePositionAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        referencePositionAction =
                            ReferencePositionAction::GoToPrimaryReferencePosition;
                    }
                    else if (value == 30.1f) { // G30.1 (Set second reference position)
                        if (referencePositionAction != ReferencePositionAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        referencePositionAction =
                            ReferencePositionAction::SetPrimaryReferencePosition;
                    }
                    else if (value == 54.f) { // G54 (Use coordinate system)
                                              // Not implemented
                    }
                    else if (value == 90.f) { // G90 (Absolute distance)
                        if (distanceMode != DistanceMode::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        distanceMode = DistanceMode::Absolute;
                    }
                    else if (value == 91.f) { // G91 (Absolute distance)
                        if (distanceMode != DistanceMode::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        distanceMode = DistanceMode::Relative;
                    }
                    else if (value == 92.f) { // G92 (Coordinate system offset)
                        if (axisOffsetAction != AxisOffsetAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        axisOffsetAction = AxisOffsetAction::SetAxisOffset;
                    }
                    else if (value == 92.1f) { // G92.1 (Reset Coordinate system offset)
                        if (axisOffsetAction != AxisOffsetAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        axisOffsetAction = AxisOffsetAction::ClearAxisOffset;
                    }
                    else if (value == 92.2f) { // G92.2 (Reset Coordinate system offset)
                        if (axisOffsetAction != AxisOffsetAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        axisOffsetAction = AxisOffsetAction::ClearAxisOffset;
                    }
                    else if (value == 92.3f) { // G92.3 (Reset Coordinate system offset)
                        if (axisOffsetAction != AxisOffsetAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        axisOffsetAction = AxisOffsetAction::ClearAxisOffset;
                    }
                    else if (value == 93.f) { // G93 (Feedrate mode inverse time)
                        if (feedrateMode != FeedrateMode::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        feedrateMode = FeedrateMode::InverseTime;
                    }
                    else if (value == 94.f) { // G94 (Feedrate mode units per minute)
                        if (feedrateMode != FeedrateMode::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        feedrateMode = FeedrateMode::UnitsPerMinute;
                    }
                    else if (value == 95.f) { // G95 (Feedrate mode units per revolution)
                        return GrblError::TurningFeaturesNotYetImplemented;
                    }
                    else {
                        return GrblError::GCodeUnsupportedGCommand;
                    }
                    break;

                case 'M':
                    isMCode = true;
                    if (value == 0.f) { // M0 (Program Pause)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else if (value == 1.f) { // M1 (Program Pause optional)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else if (value == 2.f) { // M2 (Program end, reset)
                        if (stopAction != StopAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        stopAction = StopAction::EndProgram;
                    }
                    else if (value == 3.f) { // M3 (Spindle on, clockwise)
                        if (spindleAction != SpindleAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        spindleAction = SpindleAction::Clockwise;
                    }
                    else if (value == 4.f) { // M4 (Spindle on, counterclockwise)
                        if (spindleAction != SpindleAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        spindleAction = SpindleAction::CounterClockwise;
                    }
                    else if (value == 5.f) { // M5 (Spindle off)
                        if (spindleAction != SpindleAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        spindleAction = SpindleAction::Stop;
                    }
                    else if (value == 6.f) { // M6 (Do Tool change)
                        if (changeTool) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        changeTool = true;
                    }
                    else if (value == 7.f) { // M7 (Mist coolant on)
                        if (coolantAction != CoolantAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        coolantAction = CoolantAction::Mist;
                    }
                    else if (value == 8.f) { // M8 (Flood coolant on)
                        if (coolantAction != CoolantAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        coolantAction = CoolantAction::Flood;
                    }
                    else if (value == 9.f) { // M9 (Coolant off)
                        if (coolantAction != CoolantAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        coolantAction = CoolantAction::Stop;
                    }
                    else if (value == 30.f) { // M30 (Program end, reset)
                        if (stopAction != StopAction::None) {
                            return GrblError::GCodeMultipleModalCommandsInOneBlock;
                        }
                        stopAction = StopAction::EndProgram;
                    }
                    else if (value == 48.f) { // M48 (Enable speed and feed overrides)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else if (value == 49.f) { // M49 (Disable speed and feed overrides)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else if (value == 50.f) { // M50 (Enable feed override)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else if (value == 51.f) { // M51 (Spindle speed override)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else if (value == 52.f) { // M52 (Adaptive feed control)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else if (value == 53.f) { // M53 (Feed stop control)
                        return GrblError::FeatureNotYetImplemented;
                    }
                    else {
                        return GrblError::GCodeUnsupportedMCommand;
                    }
                    break;

                case 'F':
                    if (!isnan(setFeedrate)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    setFeedrate = value;
                    break;

                case 'X':
                    if (!isnan(posX)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    posX = value;
                    break;

                case 'Y':
                    if (!isnan(posY)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    posY = value;
                    break;

                case 'Z':
                    if (!isnan(posZ)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    posZ = value;
                    break;

                case 'I':
                    if (!isnan(arcI)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    arcI = value;
                    break;

                case 'J':
                    if (!isnan(arcJ)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    arcJ = value;
                    break;

                case 'K':
                    if (!isnan(arcK)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    arcK = value;
                    break;

                case 'T':
                    if (toolNumber != -1) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    toolNumber = static_cast<int>(value);
                    break;

                case 'S':
                    if (!isnan(setSpindleSpeed)) {
                        return GrblError::GCodeMultiplyDefinedParameters;
                    }
                    setSpindleSpeed = value;
                    break;

                case 'L':
                    if (G10) {
                        if (G10LNumber != -1) {
                            return GrblError::GCodeMultiplyDefinedParameters;
                        }
                        G10LNumber = static_cast<int>(value);
                    }
                    else {
                        return GrblError::GCodeLonelyParameter;
                    }
                    break;

                case 'P':
                    if (G10) {
                        if (G10PNumber != -1) {
                            return GrblError::GCodeMultiplyDefinedParameters;
                        }
                        G10PNumber = static_cast<int>(value);
                    }
                    else {
                        if (dwell) {
                            if (dwellTime) {
                                return GrblError::GCodeMultiplyDefinedParameters;
                            }
                            dwellTime = value;
                        }
                        else {
                            return GrblError::GCodeLonelyParameter;
                        }
                    }
                    break;

                default:
                    return GrblError::GCodeUnsupportedCommand;
            }
        }

        // Finalize G10 command
        if (G10) {
            if (G10PNumber == -1) {
                return GrblError::GCodeG10MissingParameter;
            }
            if (G10LNumber == -1) {
                return GrblError::GCodeG10MissingParameter;
            }
            if (G10PNumber < 0) {
                return GrblError::GCodeNegativeValueNotAllowed;
            }
            if (G10LNumber <= 0) {
                return GrblError::GCodeNegativeValueAndZeroNotAllowed;
            }

            if (G10LNumber == 1) { // G10 L1 P<>
                offsetAction = SetOffsetAction::SetToolOffset;
                toolNumber = G10PNumber;
            }
            else if (G10LNumber == 2) { // G10 L2 P<>
                offsetAction = SetOffsetAction::SetCoordinateSystemOffset;
                coordinateSystem = G10PNumber;
            }
            else if (G10LNumber == 2) { // G10 L10 P<>
                offsetAction = SetOffsetAction::SetToolOffsetToCurrentPosition;
                toolNumber = G10PNumber;
            }
            else if (G10LNumber == 2) { // G10 L11 P<>
                offsetAction = SetOffsetAction::SetToolOffsetToCurrentPositionSkipOffset;
                toolNumber = G10PNumber;
            }
            else if (G10LNumber == 20) { // G10 L20 P<>
                offsetAction =
                    SetOffsetAction::SetCoordinateSystemOffsetToCurrentPosition;
                coordinateSystem = G10PNumber;
            }
        }

        if (motionType != MotionType::None && isnan(posX) && isnan(posY) && isnan(posZ)) {
            return GrblError::NoAxisWordsFoundInCommandBlock;
        }
        if ((motionType == MotionType::ArcCW || motionType == MotionType::ArcCCW)) {
            if (isnan(arcI) && isnan(arcJ) && isnan(arcK)) {
                return GrblError::G2G3ArcsNeedAtLeastOneInPlaneAxisWord;
            }
        }

        if (motionType == MotionType::None &&
            (!isnan(posX) || !isnan(posY) || !isnan(posZ))) {
            return GrblError::UnneededAxisWordsFoundInBlock;
        }
        if (motionType != MotionType::ArcCW && motionType != MotionType::ArcCCW &&
            (!isnan(arcI) || !isnan(arcJ) || !isnan(arcK))) {
            return GrblError::UnneededAxisWordsFoundInBlock;
        }

        // Validate all parameters
        if (dwell && isnan(dwellTime)) {
            return GrblError::GCodeDwellTimeMissing;
        }
        if (dwell && dwellTime < 0) {
            return GrblError::GCodeDwellTimeInvalid;
        }

        return GrblError::None;
    }

    void printContent(const SerialInterface &serial)
    {
        serial.println("In the order of priority:");

        switch (feedrateMode) {
            case FeedrateMode::UnitsPerMinute:
                serial.println(" - Set Feedrate mode to Units/min");
                break;
            case FeedrateMode::UnitsPerRevolution:
                serial.println(" - Set Feedrate mode to Units/rev");
                break;
            case FeedrateMode::InverseTime:
                serial.println(
                    " - Set Feedrate mode to Inverse Time (seconds per motion)");
                break;
            default:
                break;
        }

        PRINT_FLOAT(setFeedrate, " - Set Feedrate to %f", setFeedrate);
        PRINT_FLOAT(setSpindleSpeed, " - Set Spindle speed to %f", setSpindleSpeed);
        PRINT_BOOL(prepareTool, " - Prepare tool #%d", toolNumber);
        PRINT_BOOL(changeTool, " - Change to the previously prepared tool");

        switch (spindleAction) {
            case SpindleAction::Stop:
                serial.println(" - Stop the spindle");
                break;
            case SpindleAction::Clockwise:
                serial.println(" - Start the spindle clockwise");
                break;
            case SpindleAction::CounterClockwise:
                serial.println(" - Start the spindle counter-clockwise");
            default:
                break;
        }

        switch (coolantAction) {
            case CoolantAction::Stop:
                serial.println(" - Stop coolant");
                break;
            case CoolantAction::Mist:
                serial.println(" - Start mist coolant");
                break;
            case CoolantAction::Flood:
                serial.println(" - Start flood coolant");
                break;
            default:
                break;
        }

        PRINT_BOOL(dwell, " - Wait a dwell time of %f seconds", dwellTime);

        switch (arcPlaneMode) {
            case ArcPlaneMode::XY:
                serial.println(" - Set Arc plane to XY");
                break;
            case ArcPlaneMode::YZ:
                serial.println(" - Set Arc plane to YZ");
                break;
            case ArcPlaneMode::XZ:
                serial.println(" - Set Arc plane to XZ");
                break;
            default:
                break;
        }

        switch (lengthUnits) {
            case LengthUnits::Metric:
                serial.println(" - Set Machine units to Metric");
                break;
            case LengthUnits::Imperial:
                serial.println(" - Set Machine units to Imperial");
                break;
            default:
                break;
        }

        switch (distanceMode) {
            case DistanceMode::Absolute:
                serial.println(" - Set Distance Mode to Absolute");
                break;
            case DistanceMode::Relative:
                serial.println(" - Set Distance Mode to Relative");
                break;
            default:
                break;
        }

        switch (referencePositionAction) {
            case ReferencePositionAction::SetPrimaryReferencePosition:
                serial.println(" - Set Refpos Action to Primary Reference Position");
                break;
            case ReferencePositionAction::GoToPrimaryReferencePosition:
                serial.println(
                    " - Set Refpos Action to Go-To-Primary Reference Position");
                break;
            case ReferencePositionAction::SetSecondaryReferencePosition:
                serial.println(
                    " - Set Refpos Action to Set Secondary Reference Position");
                break;
            case ReferencePositionAction::GoToSecondaryReferencePosition:
                serial.println(
                    " - Set Refpos Action to Go-To-Secondary Reference Position");
                break;
            default:
                break;
        }

        switch (offsetAction) {
            case SetOffsetAction::SetToolOffset:
                serial.println(" - Set tool offset");
                break;
            case SetOffsetAction::SetCoordinateSystemOffset:
                serial.println(" - Set coordinate system offset");
                break;
            case SetOffsetAction::SetToolOffsetToCurrentPosition:
                serial.println(" - Set tool offset to current position");
                break;
            case SetOffsetAction::SetToolOffsetToCurrentPositionSkipOffset:
                serial.println(" - Set tool offset to current position skip offset");
                break;
            case SetOffsetAction::SetCoordinateSystemOffsetToCurrentPosition:
                serial.println(" - Set coordinate system offset to current position");
                break;
            default:
                break;
        }

        switch (axisOffsetAction) {
            case AxisOffsetAction::SetAxisOffset:
                serial.println(" - Set Axis offset");
                break;
            case AxisOffsetAction::ClearAxisOffset:
                serial.println(" - Clear Axis Offset");
                break;
            default:
                break;
        }

        switch (motionType) {
            case MotionType::Rapid:
                serial.println(" - Set MotionType to Rapid");
                break;
            case MotionType::Feed:
                serial.println(" - Set MotionType to Feed");
                break;
            case MotionType::ArcCW:
                serial.println(" - Set MotionType to CW Arc");
                break;
            case MotionType::ArcCCW:
                serial.println(" - Set MotionType to CCW Arc");
                break;
            default:
                break;
        }

        switch (stopAction) {
            case StopAction::Pause:
                serial.println(" - Pause the program");
                break;
            case StopAction::EndProgram:
                serial.println(" - End the program");
                break;
            case StopAction::OptionalStop:
                serial.println(" - Optional program stop");
                break;
            default:
                break;
        }

        PRINT_FLOAT(posX, "-> Parameter X=%f", posX);
        PRINT_FLOAT(posY, "-> Parameter Y=%f", posY);
        PRINT_FLOAT(posZ, "-> Parameter Z=%f", posZ);
        PRINT_FLOAT(arcI, "-> Parameter I=%f", arcI);
        PRINT_FLOAT(arcJ, "-> Parameter J=%f", arcJ);
        PRINT_FLOAT(arcK, "-> Parameter K=%f", arcK);

        if (isSystemCommand) {
            serial.println("It is a system command:");
            PRINT_BOOL(true, " - Letter: %c", systemCommand.letter);
            PRINT_BOOL(true, " - Index: %i", systemCommand.index);
            PRINT_BOOL(true, " - Value: %f", systemCommand.value);
            PRINT_BOOL(true, " - Value letter: %c", systemCommand.valueLetter);
        }
    }
};

#endif // GCGP_COMMAND_H
#endif // __cplusplus
