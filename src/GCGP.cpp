
#include "GCGP.h"

GCGP::GCGP(const SerialInterface& serialInterface) : m_grblInterface(serialInterface) {
    
}

void GCGP::update() {
    m_grblInterface.update();
}


