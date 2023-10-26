#ifdef __cplusplus
#ifndef GCGP_SERIAL_H
#define GCGP_SERIAL_H

struct SerialInterface {
    int (*availableFunction)() = nullptr;
    int (*peekFunction)() = nullptr;
    int (*readFunction)() = nullptr;
    void (*writeFunction)(const char*) = nullptr;
    void (*printlnFunction)(const char*) = nullptr;

    int available() const {
        if (availableFunction == nullptr) {
            return 0;
        }
        return availableFunction();
    }

    int peek() const {
        if (peekFunction == nullptr) {
            return 0;
        }
        return peekFunction();
    }

    int read() const {
        if (readFunction == nullptr) {
            return 0;
        }
        return readFunction();
    }

    void write(const char* str) const {
        if (writeFunction == nullptr) {
            return;
        }
        writeFunction(str);
    }

    void println(const char* str) const {
        if (printlnFunction == nullptr) {
            return;
        }
        printlnFunction(str);
    }

    SerialInterface() = default;
    SerialInterface(int (*availableFunction)(), int (*peekFunction)(), int (*readFunction)(), void (*writeFunction)(const char*), void (*printlnFunction)(const char*)) :
        availableFunction(availableFunction), peekFunction(peekFunction), readFunction(readFunction), writeFunction(writeFunction), printlnFunction(printlnFunction) {}
};

#endif // GCGP_SERIAL_H
#endif // __cplusplus