#pragma once
#include <stdbool.h>

typedef struct {
    unsigned inkPixels;
    bool signedForm;
} ContractState;

bool contractPointAllowed(int x,int y);
void contractStateClear(ContractState *state);
void contractStateAddInk(ContractState *state);
bool contractStateApply(ContractState *state);
bool contractStateCanApply(const ContractState *state);
