/*
 *  QuantumProgress.h
 *  QuantumGL
 *
 *  Created by wolfgang on Thu Oct 25 2001.
 *
 */

#include <string>

class AbortException
{
};

bool ShouldAbort();
void CheckAbort() throw(AbortException);
void LockAbort();
void UnlockAbort();

void SetProgressMessage(std::string msg);

void StartDeterminateProgress();
void EndDeterminateProgress();
void Progress(double x);
