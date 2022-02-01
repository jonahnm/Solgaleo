extern "C" void assertis(bool v) {
        if(!v) {
        *(uintptr_t*)(0x6969) = v;
        }
}