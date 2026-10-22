#include "contract_state.h"
#include <assert.h>

int main(void)
{
    ContractState state={0};
    assert(!contractPointAllowed(54,110));
    assert(!contractPointAllowed(202,110));
    assert(!contractPointAllowed(100,98));
    assert(!contractPointAllowed(100,125));
    assert(contractPointAllowed(55,99));
    assert(contractPointAllowed(201,124));
    assert(!contractStateCanApply(&state));
    assert(!contractStateApply(&state));
    for(unsigned i=0;i<7;i++)contractStateAddInk(&state);
    assert(!state.signedForm);
    assert(!contractStateApply(&state));
    contractStateAddInk(&state);
    assert(state.signedForm);
    assert(contractStateApply(&state));
    assert(contractStateCanApply(&state));
    contractStateClear(&state);
    assert(!state.signedForm&&!state.inkPixels);
    assert(!contractStateCanApply(&state));
    return 0;
}
