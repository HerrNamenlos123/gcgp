#ifdef __cplusplus
#ifndef GCGP_SERIAL_H
#define GCGP_SERIAL_H

struct SerialInterface {
    int (*cbAvailable)() = nullptr;
    int (*cbPeek)() = nullptr;
    int (*cbRead)() = nullptr;
    void (*cbWrite)(const char *) = nullptr;

    int available() const
    {
        if (cbAvailable == nullptr) {
            return 0;
        }
        return cbAvailable();
    }

    int peek() const
    {
        if (cbPeek == nullptr) {
            return 0;
        }
        return cbPeek();
    }

    int read() const
    {
        if (cbRead == nullptr) {
            return 0;
        }
        return cbRead();
    }

    void write(const char *str) const
    {
        if (cbWrite == nullptr) {
            return;
        }
        cbWrite(str);
    }

    void print(const char *str) const
    {
        write(str);
    }

    void println(const char *str) const
    {
        write(str);
        write("\n");
    }

    SerialInterface() = default;
    SerialInterface(int (*cbAvailable)(), int (*cbPeek)(), int (*cbRead)(),
                    void (*cbWrite)(const char *), void (*cbPrintln)(const char *))
        : cbAvailable(cbAvailable), cbPeek(cbPeek), cbRead(cbRead), cbWrite(cbWrite)
    {
    }
};

#endif // GCGP_SERIAL_H
#endif // __cplusplus
