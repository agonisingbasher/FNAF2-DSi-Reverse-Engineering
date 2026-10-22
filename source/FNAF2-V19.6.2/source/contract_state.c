#include "contract_state.h"
#include <string.h>

#define SIGN_X0 55
#define SIGN_X1 201
#define SIGN_Y0 99
#define SIGN_Y1 124
#define SIGNATURE_MIN_PIXELS 8

bool contractPointAllowed(int x,int y)
{ return x>=SIGN_X0&&x<=SIGN_X1&&y>=SIGN_Y0&&y<=SIGN_Y1; }

void contractStateClear(ContractState *state)
{ memset(state,0,sizeof(*state)); }

void contractStateAddInk(ContractState *state)
{
    if(state->inkPixels<SIGNATURE_MIN_PIXELS)state->inkPixels++;
    state->signedForm=state->inkPixels>=SIGNATURE_MIN_PIXELS;
}

bool contractStateApply(ContractState *state)
{
    return state->signedForm;
}

bool contractStateCanApply(const ContractState *state)
{ return state->signedForm; }
