#ifdef __cplusplus
#ifndef GCGP_COMMAND_H
#define GCGP_COMMAND_H

#include "GCGP/Enums.h"
#include "GCGP/Config.h"
#include "GCGP/String.h"
#include "GCGP/TokenizedCommand.h"

class Command : public TokenizedCommand {   // https://linuxcnc.org/docs/html/
public:
    // All actions are declared in the order they must be executed.
    FeedrateMode feedrateMode = FeedrateMode::Undefined;                                    // G93, G94, G95
    float setFeedrate = NAN;                                                                // F<>
    float setSpindleSpeed = NAN;                                                            // S<>
    bool prepareTool = false;                                                               // T<> -> Prepare for tool change (can be on same line as M6)
    bool changeTool = false;                                                                // M6  -> This actually changes the tool
    SpindleAction spindleAction = SpindleAction::Undefined;                                 // M3, M4, M5
    CoolandAction coolandAction = CoolandAction::Undefined;                                 // M7, M8, M9
    // OverrideAction overrideAction = OverrideAction::Undefined;                              // M48, M49 not supported        (M50, M51, M52, M53 are not supported)
    bool dwell = false;                                                                     // G4 (P<> must be specified and positive)
    ArcPlaneMode arcPlaneMode = ArcPlaneMode::Undefined;                                    // G17, G18, G19
    LengthUnits lengthUnits = LengthUnits::Undefined;                                       // G20, G21
    // CutterRadiusCompMode cutterRadiusCompMode = CutterRadiusCompMode::Undefined;            // G41, G42 not supported, G40 is ignored
    // CutterLengthCompMode cutterLengthCompMode = CutterLengthCompMode::Undefined;            // G43, G49 not supported
    // CoordinateSystem                                                                        // G54-G59.3 not supported
    // PathBlending                                                                            // G61.1, G64 not supported, G61 is ignored
    DistanceMode distanceMode = DistanceMode::Undefined;                                    // G90, G91
    // RetractMode                                                                             // G98, G99 not supported
    ReferencePositionAction referencePositionAction = ReferencePositionAction::Undefined;   // G28, G28.1, G30, G30.1
    SetOffsetAction offsetAction = SetOffsetAction::Undefined;                              // G10 L<> (tool radius not supported)
    AxisOffsetAction axisOffsetAction = AxisOffsetAction::Undefined;                        // G92, G92.1, G92.2, G92.3
    MotionType motionType = MotionType::Undefined;                                          // G0, G1, G2, G3 | not supported: G33, G38.2, G38.3, G38.4, G38.5, G80, G81, G82, G83, G84, G85, G86, G87, G88, G89
    StopAction stopAction = StopAction::Undefined;                                          // M0, M1, M2, M30

    // These parameters are used by the actions defined above
    int toolNumber = -1;
    int coordinateSystem = -1;
    float dwellTime = NAN;
	float x = NAN;
	float y = NAN;
	float z = NAN;
	float i = NAN;
	float j = NAN;
	float k = NAN;

	Command();
	Command(const StringView& str);

	bool load(const StringView& str);
    void clear();
};

#endif // GCGP_COMMAND_H
#endif // __cplusplus