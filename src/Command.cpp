
#include "GCGP/Command.h"

Command::Command() {}

Command::Command(const StringView& str) {
    load(str);
}

#define ERROR_RETURN(x) { \
    m_isValid = false; \
    m_errorCode = x; \
    return false; }

bool Command::load(const StringView& str) {
    tokenize(str);
    clear();

    if (!m_isValid) {
        return false;
    }

    bool G10 = false;
    int G10LNumber = -1;
    int G10PNumber = -1;

    for (size_t i = 0; i < m_numTokens; i++) {
        char letter = m_tokenType[i];
        float value = m_tokenValue[i];
        // int32_t intValue = (int32_t)value;
        // bool isInteger = fmodf(value, 1.f) == 0.f;  // Check if the float is a whole number

        switch (letter) {

        case 'G':
            if (value == 0) {           // G0 (Rapid motion)
                if (motionType != MotionType::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                motionType = MotionType::Rapid;
            }
            else if (value == 1.f) {      // G1 (Linear motion)
                if (motionType != MotionType::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                motionType = MotionType::Feed;
            }
            else if (value == 2.f) {      // G2 (CW arc)
                if (motionType != MotionType::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                motionType = MotionType::ArcCW;
            }
            else if (value == 3.f) {      // G3 (CCW arc)
                if (motionType != MotionType::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                motionType = MotionType::ArcCCW;
            }
            else if (value == 4.f) {      // G4 (Dwell time)
                if (dwell) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                dwell = true;
            }
            else if (value == 10.f) {     // G10 (Offsets)
                if (G10) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                G10 = true;
            }
            else if (value == 17.f) {     // G17 (XY Arc plane)
                if (arcPlaneMode != ArcPlaneMode::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                arcPlaneMode = ArcPlaneMode::XY;
            }
            else if (value == 18.f) {     // G18 (XZ Arc plane)
                if (arcPlaneMode != ArcPlaneMode::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                arcPlaneMode = ArcPlaneMode::XZ;
            }
            else if (value == 19.f) {     // G19 (YZ Arc plane)
                if (arcPlaneMode != ArcPlaneMode::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                arcPlaneMode = ArcPlaneMode::YZ;
            }
            else if (value == 20.f) {     // G20 (Imperial)
                if (lengthUnits != LengthUnits::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                lengthUnits = LengthUnits::Imperial;
            }
            else if (value == 21.f) {     // G21 (Metric)
                if (lengthUnits != LengthUnits::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                lengthUnits = LengthUnits::Metric;
            }
            else if (value == 28.f) {     // G28 (Move to reference position)
                if (referencePositionAction != ReferencePositionAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                referencePositionAction = ReferencePositionAction::GoToPrimaryReferencePosition;
            }
            else if (value == 28.1f) {    // G28.1 (Set reference position)
                if (referencePositionAction != ReferencePositionAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                referencePositionAction = ReferencePositionAction::SetPrimaryReferencePosition;
            }
            else if (value == 30.f) {     // G30 (Move to second reference position)
                if (referencePositionAction != ReferencePositionAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                referencePositionAction = ReferencePositionAction::GoToPrimaryReferencePosition;
            }
            else if (value == 30.1f) {    // G30.1 (Set second reference position)
                if (referencePositionAction != ReferencePositionAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                referencePositionAction = ReferencePositionAction::SetPrimaryReferencePosition;
            }
            else if (value == 54.f) {     // G54 (Use coordinate system)
                // Not implemented
            }
            else if (value == 90.f) {     // G90 (Absolute distance)
                if (distanceMode != DistanceMode::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                distanceMode = DistanceMode::Absolute;
            }
            else if (value == 91.f) {     // G91 (Absolute distance)
                if (distanceMode != DistanceMode::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                distanceMode = DistanceMode::Relative;
            }
            else if (value == 92.f) {     // G92 (Coordinate system offset)
                if (axisOffsetAction != AxisOffsetAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                axisOffsetAction = AxisOffsetAction::SetAxisOffset;
            }
            else if (value == 92.1f) {    // G92.1 (Reset Coordinate system offset)
                if (axisOffsetAction != AxisOffsetAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                axisOffsetAction = AxisOffsetAction::ClearAxisOffset;
            }
            else if (value == 92.2f) {    // G92.2 (Reset Coordinate system offset)
                if (axisOffsetAction != AxisOffsetAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                axisOffsetAction = AxisOffsetAction::ClearAxisOffset;
            }
            else if (value == 92.3f) {    // G92.3 (Reset Coordinate system offset)
                if (axisOffsetAction != AxisOffsetAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                axisOffsetAction = AxisOffsetAction::ClearAxisOffset;
            }
            else if (value == 93.f) {     // G93 (Feedrate mode inverse time)
                if (feedrateMode != FeedrateMode::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                feedrateMode = FeedrateMode::InverseTime;
            }
            else if (value == 94.f) {     // G94 (Feedrate mode units per minute)
                if (feedrateMode != FeedrateMode::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                feedrateMode = FeedrateMode::UnitsPerMinute;
            }
            else if (value == 95.f) {     // G95 (Feedrate mode units per revolution)
                ERROR_RETURN(GrblError::TurningFeaturesNotYetImplemented);
            }
            else {
                ERROR_RETURN(GrblError::GCodeUnsupportedGCommand);
            }
            break;
            
        case 'M':
            if (value == 0.f) {             // M0 (Program Pause)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else if (value == 1.f) {        // M1 (Program Pause optional)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else if (value == 2.f) {        // M2 (Program end, reset)
                if (stopAction != StopAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                stopAction = StopAction::EndProgram;
            }
            else if (value == 3.f) {        // M3 (Spindle on, clockwise)
                if (spindleAction != SpindleAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                spindleAction = SpindleAction::Clockwise;
            }
            else if (value == 4.f) {        // M4 (Spindle on, counterclockwise)
                if (spindleAction != SpindleAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                spindleAction = SpindleAction::CounterClockwise;
            }
            else if (value == 5.f) {        // M5 (Spindle off)
                if (spindleAction != SpindleAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                spindleAction = SpindleAction::Stop;
            }
            else if (value == 6.f) {        // M6 (Do Tool change)
                if (changeTool) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                changeTool = true;
            }
            else if (value == 7.f) {        // M7 (Mist coolant on)
                if (coolandAction != CoolandAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                coolandAction = CoolandAction::Mist;
            }
            else if (value == 8.f) {        // M8 (Flood coolant on)
                if (coolandAction != CoolandAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                coolandAction = CoolandAction::Flood;
            }
            else if (value == 9.f) {        // M9 (Coolant off)
                if (coolandAction != CoolandAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                coolandAction = CoolandAction::Stop;
            }
            else if (value == 30.f) {       // M30 (Program end, reset)
                if (stopAction != StopAction::Undefined) ERROR_RETURN(GrblError::GCodeMultipleModalCommandsInOneBlock);
                stopAction = StopAction::EndProgram;
            }
            else if (value == 48.f) {       // M48 (Enable speed and feed overrides)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else if (value == 49.f) {       // M49 (Disable speed and feed overrides)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else if (value == 50.f) {       // M50 (Enable feed override)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else if (value == 51.f) {       // M51 (Spindle speed override)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else if (value == 52.f) {       // M52 (Adaptive feed control)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else if (value == 53.f) {       // M53 (Feed stop control)
                ERROR_RETURN(GrblError::FeatureNotYetImplemented);
            }
            else {
                ERROR_RETURN(GrblError::GCodeUnsupportedMCommand);
            }
            break;

        case 'F':
            if (!isnan(setFeedrate)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            setFeedrate = value;
            break;

        case 'X':
            if (!isnan(x)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            x = value;
            break;

        case 'Y':
            if (!isnan(y)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            y = value;
            break;

        case 'Z':
            if (!isnan(z)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            z = value;
            break;

        case 'I':
            if (!isnan(i)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            i = value;
            break;

        case 'J':
            if (!isnan(j)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            j = value;
            break;

        case 'K':
            if (!isnan(k)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            k = value;
            break;

        case 'T':
            if (toolNumber != -1) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            toolNumber = static_cast<int>(value);
            break;

        case 'S':
            if (!isnan(setSpindleSpeed)) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
            setSpindleSpeed = value;
            break;

        case 'L':
            if (G10) {
                if (G10LNumber != -1) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
                G10LNumber = static_cast<int>(value);
            }
            else {
                ERROR_RETURN(GrblError::GCodeLonelyParameter);
            }
            break;

        case 'P':
            if (G10) {
                if (G10PNumber != -1) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
                G10PNumber = static_cast<int>(value);
            }
            else {
                if (dwell) {
                    if (dwellTime) ERROR_RETURN(GrblError::GCodeMultiplyDefinedParameters);
                    dwellTime = value;
                }
                else {
                    ERROR_RETURN(GrblError::GCodeLonelyParameter);
                }
            }
            break;

        default:
            ERROR_RETURN(GrblError::GCodeUnsupportedCommand);
        }
    }

    // Finalize G10 command
    if (G10) {
        if (G10PNumber == -1) ERROR_RETURN(GrblError::GCodeG10MissingParameter);
        if (G10LNumber == -1) ERROR_RETURN(GrblError::GCodeG10MissingParameter);
        if (G10PNumber < 0) ERROR_RETURN(GrblError::GCodeNegativeValueNotAllowed);
        if (G10LNumber <= 0) ERROR_RETURN(GrblError::GCodeNegativeValueAndZeroNotAllowed);

        if (G10LNumber == 1) {  // G10 L1 P<>
            offsetAction = SetOffsetAction::SetToolOffset;
            toolNumber = G10PNumber;
        }
        else if (G10LNumber == 2) {   // G10 L2 P<>
            offsetAction = SetOffsetAction::SetCoordinateSystemOffset;
            coordinateSystem = G10PNumber;
        }
        else if (G10LNumber == 2) {   // G10 L10 P<>
            offsetAction = SetOffsetAction::SetToolOffsetToCurrentPosition;
            toolNumber = G10PNumber;
        }
        else if (G10LNumber == 2) {   // G10 L11 P<>
            offsetAction = SetOffsetAction::SetToolOffsetToCurrentPositionSkipOffset;
            toolNumber = G10PNumber;
        }
        else if (G10LNumber == 20) {  // G10 L20 P<>
            offsetAction = SetOffsetAction::SetCoordinateSystemOffsetToCurrentPosition;
            coordinateSystem = G10PNumber;
        }
    }

    // Validate all parameters
    if (dwell && isnan(dwellTime)) ERROR_RETURN(GrblError::GCodeDwellTimeMissing);
    if (dwell && dwellTime < 0) ERROR_RETURN(GrblError::GCodeDwellTimeInvalid);

    return true;
}

void Command::clear() {
    motionType = MotionType::Undefined;
    distanceMode = DistanceMode::Undefined;
    unitMode = UnitMode::None;
    arcPlane = ArcPlane::None;
    feedrate = NAN;
    x = NAN;
    y = NAN;
    z = NAN;
    i = NAN;
    j = NAN;
    k = NAN;
    setTo = NAN;
    G10P0L2 = false;
    G10P0L20 = false;
    toolchange = false;
}

// void Command::PrintParameters() const {
//     message("Command: '", commandString, "'");
//     message("Sliced <", numberOfParameters, ">:");

//     for (size_t i = 0; i < numberOfParameters; i++) {
//         message("[", i, "]: ", type[i], (float)value[i]);
//     }
// }

// void Command::PrintInterpretings() const {

//     if (motionType != MG_MotionType::NONE) message("Motion type: String not implemented");
//     if (distanceMode != DistanceMode::NONE) message("Distance mode: ", DISTANCE_MODE_STRING[(int)distanceMode]);
//     if (unitMode != UnitMode::NONE) message("Unit mode: ", UNIT_MODE_STRING[(int)unitMode]);
//     if (arcPlane != MG_ArcPlane::NONE) message("Arc plane: String not implemented");

//     if (!isnan(feedrate)) message("Feed rate: ", feedrate);
//     if (!isnan(x)) message("X: ", x);
//     if (!isnan(y)) message("Y: ", y);
//     if (!isnan(z)) message("Z: ", z);
//     if (!isnan(i)) message("I: ", i);
//     if (!isnan(j)) message("J: ", j);
//     if (!isnan(k)) message("K: ", k);

//     if (!isnan(setTo)) message("Set something to: ", setTo);

//     if (G10P0L2) message("G10L2 was specified");
//     if (G10P0L20) message("G10L20 was specified");

//     if (GetDollarCommand() == (char)-1) message("Dollar command number: $", GetDollarValue());
//     else if (GetDollarCommand() != 0) message("Dollar command char: $", GetDollarCommand());

// }