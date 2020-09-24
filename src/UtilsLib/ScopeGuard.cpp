#include "pch.h"
#include "ScopeGuard.h"

ScopeGuard::ScopeGuard(std::function<void()> f_)
    : f(f_) {
}

ScopeGuard::~ScopeGuard() {
    f();
}
