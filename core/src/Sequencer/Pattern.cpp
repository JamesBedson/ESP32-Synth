#include "Pattern.h"

// =============================================
// Step
// =============================================

void Step::setNote(const uint8_t& newNote) {
    note = std::min<uint8_t>(
        newNote,
        Constants::MAX_MIDI_RANGE
    );
}

void Step::setVelocity(const uint8_t& newVelocity) {
    velocity = std::min<uint8_t>(
        newVelocity,
        Constants::MAX_MIDI_RANGE
    );
}

void Step::setGate(const bool& newGate) {
    gate = newGate;
}