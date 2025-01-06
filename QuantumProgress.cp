#include "QuantumProgress.h"
#include <iostream>

bool shouldAbort = false;
bool abortLocked = false;

bool ShouldAbort() {
    return !abortLocked && shouldAbort;
}

void CheckAbort() throw(AbortException) {
    if (ShouldAbort()) {
        throw AbortException();
    }
}

void LockAbort() {
    abortLocked = true;
}

void UnlockAbort() {
    abortLocked = false;
}

void SetProgressMessage(std::string msg) {
    std::cout << msg << std::endl;
}

void StartDeterminateProgress() {
    // Implementation for progress tracking
}

void EndDeterminateProgress() {
    // Implementation for ending progress tracking
}

void Progress(double x) {
    // Implementation for updating progress
    std::cout << "\rProgress: " << (x * 100) << "%" << std::flush;
}
