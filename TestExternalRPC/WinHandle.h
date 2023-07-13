#pragma once

// So I don't have to worry about whether I forgot a "CloseHandle" or not
// https://stackoverflow.com/a/34405788

class WinHandle {
    HANDLE value_;
public:
    WinHandle(std::nullptr_t = nullptr) : value_(nullptr) {}
    WinHandle(HANDLE value) : value_(value == INVALID_HANDLE_VALUE ? nullptr : value) {}

    explicit operator bool() const { return value_ != nullptr; }
    operator HANDLE() const { return value_; }

    friend bool operator ==(WinHandle l, WinHandle r) { return l.value_ == r.value_; }
    friend bool operator !=(WinHandle l, WinHandle r) { return !(l == r); }

    struct Deleter {
        typedef WinHandle pointer;
        void operator()(WinHandle handle) const { CloseHandle(handle); }
    };
};

inline bool operator ==(HANDLE l, WinHandle r) { return WinHandle(l) == r; }
inline bool operator !=(HANDLE l, WinHandle r) { return !(l == r); }
inline bool operator ==(WinHandle l, HANDLE r) { return l == WinHandle(r); }
inline bool operator !=(WinHandle l, HANDLE r) { return !(l == r); }

typedef std::unique_ptr<WinHandle, WinHandle::Deleter> HandlePtr;