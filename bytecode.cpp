#include "bytecode.h"
#include "caosVM.h"
#include "caosVar.h"
#include "Agent.h"

const char *cnams[] = {
    NULL,
    "EQ",
    "LT",
    "LE",
    "GT",
    "GE",
    "NE",
    NULL
};

void caosOp::execute(caosVM *vm) {
    if (!vm->inst) vm->timeslice -= evalcost;
}

void caosJMP::execute(caosVM *vm) {
    vm->nip = p;
}

void simpleCaosOp::execute(caosVM *vm) {
    caosOp::execute(vm);
    int stackc = vm->valueStack.size();
    (vm->*(ci->handler))();
    int delta = vm->valueStack.size() - stackc;
    if (!vm->result.isNull())
        delta++;
    if (ci->retc != -1 && 
            delta != ci->retc - ci->argc) {
        std::ostringstream oss;
        oss << "return count mismatch for op "
            << ci->fullname << ", delta=" << delta
            << std::endl;
        throw caosException(oss.str());
    }
}

void caosREPS::execute(caosVM *vm) {
    caosOp::execute(vm);
    VM_PARAM_INTEGER(i)
    caos_assert(i >= 0);
    if (i == 0) {
        vm->nip = exit;
        return;
    }
    vm->result.setInt(i - 1);
}

void caosGSUB::execute(caosVM *vm) {
    caosOp::execute(vm);
    callStackItem i;
    i.valueStack = vm->valueStack; // XXX: a bit slow?
    i.nip = vm->nip;
    vm->callStack.push_back(i);
    vm->valueStack.clear();
    vm->nip = targ;
}

void caosCond::execute(caosVM *vm) {
    caosOp::execute(vm);

    VM_PARAM_VALUE(arg2);
    VM_PARAM_VALUE(arg1);

    int cres;
    if (arg2.hasString() && arg1.hasString()) {
        std::string str1 = arg1.getString();
        std::string str2 = arg2.getString();

        if (str1 < str2)
            cres = CLT;
        else if (str1 > str2)
            cres = CGT;
        else
            cres = CEQ;
    } else if (arg2.hasDecimal() && arg1.hasDecimal()) {
        float val1 = arg1.getFloat();
        float val2 = arg2.getFloat();

        if (val1 < val2)
            cres = CLT;
        else if (val1 > val2)
            cres = CGT;
        else
            cres = CEQ;
    } else if (arg2.hasAgent() && arg1.hasAgent()) {
        if (cond != CEQ && cond != CNE)
            throw caosException("invalid comparison for agents");
        Agent *a1, *a2;
        a1 = arg1.getAgent();
        a2 = arg2.getAgent();
        if (a1 == a2)
            cres = CEQ;
        else
            cres = CNE;
        // the next bit is needed for some missing GAME etc
    } else cres = CNE;

    if (cres & cond)
        vm->nip = branch;
}

void caosENUM_POP::execute(caosVM *vm) {
    caosOp::execute(vm);
    VM_PARAM_VALUE(v);
    if (v.isNull()) { // no more values
        vm->nip = exit;
        vm->targ = vm->owner;
        return;
    }
    if (v.getAgent() == NULL) { // killed?
        vm->nip = vm->cip;
        return;
    }
    vm->setTarg(v.getAgent());
}

void caosSTOP::execute(caosVM *vm) {
    vm->stop();
}

void ConstOp::execute(caosVM *vm) {
    vm->valueStack.push_back(constVal);
    caosOp::execute(vm);
}

void opVAxx::execute(caosVM *vm) {
    caosOp::execute(vm);
    vm->valueStack.push_back(&vm->var[index]);
}

void opOVxx::execute(caosVM *vm) {
    caosOp::execute(vm);
    caos_assert(vm->targ);
    vm->valueStack.push_back(&vm->targ->var[index]);
}

void opMVxx::execute(caosVM *vm) {
    caosOp::execute(vm);
    caos_assert(vm->owner);
    vm->valueStack.push_back(&vm->owner->var[index]);
}

void opBytestr::execute(caosVM *vm) {
    caosOp::execute(vm);
    vm->valueStack.push_back(bytestr);
}

