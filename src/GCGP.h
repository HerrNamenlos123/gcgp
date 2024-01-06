#ifdef __cplusplus
#ifndef GCGP_H
#define GCGP_H

#include "GCGP/Enums.h"
#include "GCGP/Vector.h"
#include "GCGP/Config.h"
#include "GCGP/GrblInterface.h"

// parse() and response() are the interface functions for this class.
// The response is always only valid for one frame, directly after 
// the call to update().
class GCGP {
public:
    GCGP(const SerialInterface& serialInterface);

    void update();

private:
    GrblInterface m_grblInterface;
};

#endif // GCGP_H
#endif // __cplusplus
